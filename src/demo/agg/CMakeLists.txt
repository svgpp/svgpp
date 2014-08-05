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
  ${AGG_PATH}/src/agg_vcgen_dash.cpp 
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
  filter.hpp
  filter.cpp
)

add_executable(svgpp_agg_render
  ${AGG_DEMO_SOURCES}
  parser_rapidxml_ns.hpp 
  parser_rapidxml_ns.cpp 
)

target_compile_definitions(svgpp_agg_render
  PRIVATE SVG_PARSER_RAPIDXML_NS
)

add_executable(svgpp_agg_render_msxml
  ${AGG_DEMO_SOURCES}
  parser_msxml.hpp 
  parser_msxml.cpp 
)

target_compile_definitions(svgpp_agg_render_msxml
  PRIVATE SVG_PARSER_MSXML
)

add_executable(svgpp_agg_render_libxml
  ${AGG_DEMO_SOURCES}
  parser_libxml.hpp
  parser_libxml.cpp
)

target_compile_definitions(svgpp_agg_render_libxml
  PRIVATE SVG_PARSER_LIBXML
)

set_property(TARGET svgpp_agg_render_libxml
  APPEND PROPERTY INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../../../third_party/libxml2/include"
)

if (MSVC)
  set_target_properties(
    svgpp_agg_render svgpp_agg_render_msxml svgpp_agg_render_libxml 
    PROPERTIES COMPILE_FLAGS "/bigobj"
  )
endif()
  