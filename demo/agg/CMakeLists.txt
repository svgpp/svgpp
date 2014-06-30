cmake_minimum_required (VERSION 2.6)

project(svgpp_agg_render)

set(AGG_PATH "../../../third_party/agg")

include_directories(
  ../../..
  ${AGG_PATH}/include
)

set(AGG_SOURCES
  ${AGG_PATH}/src/agg_curves.cpp 
  ${AGG_PATH}/src/agg_trans_affine.cpp 
  ${AGG_PATH}/src/agg_vcgen_stroke.cpp 
)
source_group("AGG" FILES ${AGG_SOURCES})

add_executable( svgpp_agg_render
  ${AGG_SOURCES}
  svgpp_agg_render.cpp 
  bmp_header.hpp
  gradient.hpp
  gradient.cpp
  stylable.hpp
  common.hpp
)

if (MSVC)
  set_target_properties(svgpp_agg_render PROPERTIES
    COMPILE_FLAGS "/bigobj"
  )
endif()
  