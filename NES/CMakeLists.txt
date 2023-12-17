file(GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(NES ${SOURCE_FILES})

target_compile_options(NES INTERFACE
  "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused; -Werror>"
  "$<${msvc_cxx}:$<BUILD_INTERFACE:-W4;-WX>"
)

target_include_directories(NES PUBLIC
                                    "${PROJECT_BINARY_DIR}"
                                    "${PROJECT_SOURCE_DIR}")