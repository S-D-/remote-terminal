#include "connection.h"
#include <stdlib.h>

/**
 * Sends message prepended with 1 byte of the message size information.
 * If message size >= 255 then message is sent by chunks of 254 chars with size info equal to 255.
 * @brief connetion_send_message
 * @param connection
 * @param message
 * @param size
 */
void connection_send_message(Connection* connection, const char* message, size_t size)
{
    GError * error = NULL;
    GOutputStream * ostream = g_io_stream_get_output_stream (G_IO_STREAM (connection->gSockConnection));
    //g_io_channel_set_encoding((GIOStream*) ostream, "", NULL);
    size_t tmpSize = size;
    gsize bytesWritten;
    while (tmpSize >= 255) {
        unsigned char s = 255u;
        g_output_stream_write_all(ostream, &s,1, &bytesWritten, NULL,&error);
        if (error != NULL)
        {
            g_error (error->message);
            g_error_free (error);
            return;
        }
        g_output_stream_write_all(ostream,&message[size - tmpSize],254,
                &bytesWritten,NULL,&error);
        if (error != NULL)
        {
            g_error (error->message);
            g_error_free (error);
            return;
        }
        tmpSize -= 254;
    }
    g_output_stream_write_all(ostream, &tmpSize, 1, &bytesWritten, NULL, &error);
    if (error != NULL)
    {
        g_error (error->message);
        g_error_free (error);
        return;
    }
    g_output_stream_write_all(ostream, &message[size - tmpSize], tmpSize,
            &bytesWritten, NULL, &error);
    if (error != NULL)
    {
        g_error (error->message);
        g_error_free (error);
        return;
    }
}

char* connection_read_message(Connection* connection, size_t* readSize)
{
    GInputStream * istream = g_io_stream_get_input_stream (G_IO_STREAM(connection->gSockConnection));
    //g_io_channel_set_encoding(istream, "", NULL);
    gsize bytesRead;
    GError* error = NULL;
    unsigned char msgSize;
    gboolean success = g_input_stream_read_all(istream,
                            &msgSize,
                            1,
                            &bytesRead,
                            NULL,
                            &error);
    if (!bytesRead || !success) {
        return NULL;
    }
    size_t resTotalSize = 0;
    gchar* message = NULL;
    while (msgSize >= 255) {
        g_print("reading cycle...\n");
        resTotalSize += 254;
        message = realloc(message, resTotalSize);
        success = g_input_stream_read_all(istream,
                                          &message[resTotalSize - 254],
                254,
                &bytesRead,
                NULL,
                &error);
        if (!success) {
            return NULL;
        }
        success = g_input_stream_read_all(istream,
                                &msgSize,
                                1,
                                &bytesRead,
                                NULL,
                                NULL);
        if (!success) {
            return NULL;
        }
        g_print("reading cycle.\n");
    }
    resTotalSize += msgSize;
    message = realloc(message, resTotalSize);
    success = g_input_stream_read_all(istream,
                            &message[resTotalSize - msgSize],
            msgSize,
            &bytesRead,
            NULL,
            NULL);
    if (!success) {
        return NULL;
    }
    *readSize = resTotalSize;
    return message;
}

void connection_close(Connection *connection)
{
    GError* error = NULL;
    g_io_stream_close((GIOStream*)connection->gSockConnection, NULL, &error);
    g_object_unref(connection->gSockConnection);
}
