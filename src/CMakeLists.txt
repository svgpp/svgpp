cmake_minimum_required (VERSION 2.8)

project(svgpp)

cmake_policy(SET CMP0167 OLD)
find_package(Boost REQUIRED)

if (Boost_FOUND)
  # Exempt incompatible versions.
  if ((108100 LESS_EQUAL ${Boost_VERSION}) AND (108400 GREATER ${Boost_VERSION}))
    message(WARNING " Boost version ${Boost_VERSION} contained an ODR violation.\n"
            "    See https://github.com/boostorg/phoenix/pull/112 for a fix or use a newer boost version [1.84.0 at least].")
  endif()
endif()

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
