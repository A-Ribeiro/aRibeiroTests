cmake_minimum_required (VERSION 3.5.1)

project(aRibeiroTests)

# "For this is how God loved the world:
# he gave his only Son, so that everyone
# who believes in him may not perish
# but may have eternal life."
#
# John 3:16

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

ENABLE_LANGUAGE(CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake-modules)

#the path we need to get the custom lib versions
set(LIBS_REPOSITORY_URL "https://github.com/A-Ribeiro/public_libs/raw/main")

include(cmake/include.cmake)
include(cmake/libs/libaRibeiroCore.cmake)
include(cmake/libs/libaRibeiroPlatform.cmake)
include(cmake/libs/libaRibeiroData.cmake)

#include the projects

add_subdirectory(src/test-core)
add_subdirectory(src/test-thread)
add_subdirectory(src/test-queue)
add_subdirectory(src/test-ipc)
add_subdirectory(src/test-socket-tcp)
add_subdirectory(src/test-socket-udp)

add_subdirectory(src/debug-console)
