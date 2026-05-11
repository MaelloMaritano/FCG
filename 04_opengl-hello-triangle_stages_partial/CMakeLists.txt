cmake_minimum_required(VERSION 3.5...4.0)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "Minimum policy version for all subprojects")

project(SFMLOpenGLExample LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_library(common INTERFACE)
target_compile_features(common INTERFACE cxx_std_17)
target_compile_options(common INTERFACE
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wall -DGL_SILENCE_DEPRECATION>
  $<$<CXX_COMPILER_ID:MSVC>: /W4>
)

# Find OpenGL
find_package(OpenGL REQUIRED)

SET( f00 "00_hello-gl" )
add_executable( ${f00} ${f00}.cc )
target_link_libraries( ${f00} PRIVATE common SFML::Window OpenGL::GL )

SET( f01 "01_hello-triangle" )
add_executable( ${f01} ${f01}.cc )
target_link_libraries( ${f01} PRIVATE common SFML::Window OpenGL::GL )

SET( f02 "02_refactor" )
add_executable( ${f02} ${f02}.cc )
target_link_libraries( ${f02} PRIVATE common SFML::Window OpenGL::GL )

SET( f03 "03_hotshaders" )
add_executable( ${f03} ${f03}.cc )
target_link_libraries( ${f03} PRIVATE common SFML::Window OpenGL::GL )

