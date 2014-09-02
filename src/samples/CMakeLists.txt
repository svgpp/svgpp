cmake_minimum_required (VERSION 2.6)

project(svgpp_samples)

include_directories(
  ../../include
  ../../third_party
)

set(SVGPP_INCLUDE ../../include/svgpp)

add_executable( SamplePath
  sample_path.cpp 
)

add_executable( SampleGradient
  sample_gradient.cpp 
)

add_executable( SampleFilter
  sample_filter.cpp 
)

#add_executable( svg_dom_sample
#  svg_dom_sample.cpp
#)
