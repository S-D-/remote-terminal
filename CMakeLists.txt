project(remote-terminal)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")

#include(FindGLIB.cmake)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB_PKG glib-2.0)
if (GLIB_PKG_FOUND)
    message(Found glib-2.0)
    message(${GLIB_PKG_INCLUDE_DIRS})
    message(${GLIB_PKG_LIBRARIES})
endif(GLIB_PKG_FOUND)
find_path(GLIB_INCLUDE_DIR NAMES glib.h PATH_SUFFIXES glib-2.0)
include_directories(${GLIB_PKG_INCLUDE_DIRS})

pkg_check_modules(GIO_PKG gio-2.0)
if (GIO_PKG_FOUND)
    message(Found gio-2.0)
endif(GIO_PKG_FOUND)
find_path(GIO_INCLUDE_DIR NAMES gio.h PATH_SUFFIXES gio-2.0)
include_directories(${GIO_PKG_INCLUDE_DIRS})

find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

#add_executable(${PROJECT_NAME} ${SRC_LIST})
add_subdirectory(networking)
add_subdirectory(server)
add_subdirectory(utils)
add_subdirectory(client)

if(UNIX)
  add_definitions(-D__UNIX__)
elseif(WIN32)
  add_definitions(-D__WIN32__)
endif()

message(${SRC_LIST})
SET(servername SPServer)
add_executable(${servername} servermain.c)
target_link_libraries(${servername} LINK_PUBLIC server utils networking ${LIBXML2_LIBRARIES} ${GLIB_PKG_LIBRARIES} ${GIO_PKG_LIBRARIES} )

SET(clientname SPClient)
add_executable(${clientname} clientmain.c)
target_link_libraries(${clientname} LINK_PUBLIC client networking utils ${GLIB_PKG_LIBRARIES} ${GIO_PKG_LIBRARIES} )

