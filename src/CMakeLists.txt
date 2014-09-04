cmake_minimum_required (VERSION 2.6)

project(svgpp)

include_directories(
  # Here should be path to Boost root if not in system-wide paths
)

link_directories(
  # Here should be path to Boost libraries dir (stage/lib) if not in system-wide paths
)

if (MSVC)
  set_directory_properties(PROPERTIES 
    COMPILE_DEFINITIONS _VARIADIC_MAX=10
  ) 
endif()

add_subdirectory(demo/agg)
add_subdirectory(samples)
add_subdirectory(test)
