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

set(AGG_DEMO_SOURCES
  ${AGG_SOURCES}
  svgpp_agg_render.cpp 
  bmp_header.hpp
  gradient.hpp
  gradient.cpp
  stylable.hpp
  common.hpp
  clip_path.hpp
  clip_path.cpp
)

add_executable(svgpp_agg_render
  ${AGG_DEMO_SOURCES}
)

add_executable(svgpp_agg_render_msxml
  ${AGG_DEMO_SOURCES}
)

target_compile_definitions(svgpp_agg_render_msxml
  PRIVATE USE_MSXML
)

if (MSVC)
  set_target_properties(svgpp_agg_render svgpp_agg_render_msxml PROPERTIES
    COMPILE_FLAGS "/bigobj"
  )
endif()
  