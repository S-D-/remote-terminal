add_library(networking connection.c)
target_include_directories(networking PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
