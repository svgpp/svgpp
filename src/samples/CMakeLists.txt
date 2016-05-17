cmake_minimum_required (VERSION 2.8)

project(svgpp_samples)

include_directories(
  ../../include
  ../../third_party
)

if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()

add_executable( Sample01a sample01a.cpp )
add_executable( Sample01b sample01b.cpp )
add_executable( Sample01c sample01c.cpp )
add_executable( Sample01d sample01d.cpp )
add_executable( Sample01e sample01e.cpp )
add_executable( Sample01f sample01f.cpp )
add_executable( Sample01g sample01g.cpp svgpp_parser_impl.cpp)
add_executable( Sample01h sample01h.cpp svgpp_parser_impl.cpp)
add_executable( Sample01i sample01i.cpp svgpp_parser_impl.cpp)
add_executable( Sample01j sample01j.cpp sample01j_impl.cpp)

add_executable( SampleGradient sample_gradient.cpp )
add_executable( SampleFilter sample_filter.cpp )
add_executable( SampleAnimation sample_animation.cpp )
add_executable( SampleIRI sample_iri.cpp )
add_executable( SampleTransform01 sample_transform01.cpp )
add_executable( SampleTransform02 sample_transform02.cpp )
add_executable( SampleValue01 sample_value01.cpp )
add_executable( SampleValue02 sample_value02.cpp )
