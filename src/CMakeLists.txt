cmake_minimum_required (VERSION 2.8)

project(svgpp)

find_package(Boost REQUIRED)

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

add_subdirectory(demo/agg)
add_subdirectory(samples)
add_subdirectory(test)
