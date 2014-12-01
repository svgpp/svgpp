cmake_minimum_required (VERSION 2.8)

project(svgpp)

find_package(Boost REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(
  ${Boost_INCLUDE_DIRS}
)

link_directories(
  ${Boost_LIBRARY_DIRS}
)

if (MSVC)
  set_directory_properties(PROPERTIES 
    COMPILE_DEFINITIONS _VARIADIC_MAX=10
  ) 
endif()

add_subdirectory(demo/render)
add_subdirectory(samples)
add_subdirectory(test)
