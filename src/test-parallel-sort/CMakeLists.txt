project (test-parallel-sort)

FILE( GLOB_RECURSE PUBLIC_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h)
FILE( GLOB_RECURSE SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.cpp)

add_executable(${PROJECT_NAME} ${PUBLIC_HEADERS} ${SRC})

define_source_group(${PUBLIC_HEADERS} ${SRC})

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "TESTS")

target_link_libraries( ${PROJECT_NAME} aRibeiroCore aRibeiroPlatform )