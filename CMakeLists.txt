cmake_minimum_required (VERSION 3.5.1)

project(aRibeiroTests)

# "For this is how God loved the world:
# he gave his only Son, so that everyone
# who believes in him may not perish
# but may have eternal life."
#
# John 3:16

set( ARIBEIRO_FIND_LIBS None CACHE STRING "Use find_package or download from GitHUB." )
set_property(CACHE ARIBEIRO_FIND_LIBS PROPERTY STRINGS None UsingFindPackage DownloadSource)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

ENABLE_LANGUAGE(CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake-modules)

#the path we need to get the custom lib versions
set(LIBS_REPOSITORY_URL "https://github.com/A-Ribeiro/public_libs/raw/main")
set(GIT_DOWNLOAD_METHOD SSH) #HTTPS

include(cmake/include.cmake)

if(ARIBEIRO_FIND_LIBS STREQUAL DownloadSource)

    include(cmake/libs/libaRibeiroCore.cmake)
    include(cmake/libs/libaRibeiroPlatform.cmake)

elseif(ARIBEIRO_FIND_LIBS STREQUAL UsingFindPackage)

    find_package(aRibeiroCore REQUIRED)
    add_library(aRibeiroCore INTERFACE)
    target_link_libraries(aRibeiroCore INTERFACE ${ARIBEIROCORE_LIBRARIES})
    include_directories(${ARIBEIROCORE_INCLUDE_DIR})

    find_package(aRibeiroPlatform REQUIRED)
    add_library(aRibeiroPlatform INTERFACE)
    target_link_libraries(aRibeiroPlatform INTERFACE ${ARIBEIROPLATFORM_LIBRARIES})
    include_directories(${ARIBEIROPLATFORM_INCLUDE_DIR})

else()

    message(STATUS "You need to choose the ARIBEIRO_FIND_LIBS method")
    return()

endif()


#include the projects

add_subdirectory(src/test-core)
add_subdirectory(src/test-thread)
add_subdirectory(src/test-queue)
add_subdirectory(src/test-ipc)
add_subdirectory(src/test-socket-tcp)
add_subdirectory(src/test-socket-udp)

add_subdirectory(src/debug-console)
