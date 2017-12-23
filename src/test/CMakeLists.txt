cmake_minimum_required (VERSION 2.8)

project(svgpp_test)

set(GTEST_DIR ../../third_party/googletest/googletest)
set(GMOCK_DIR ../../third_party/googletest/googlemock)

include_directories(
  ../../include
  ../../third_party
  ${GTEST_DIR}
  ${GTEST_DIR}/include
  ${GMOCK_DIR}
  ${GMOCK_DIR}/include
)

if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /W4")
endif()

file(GLOB_RECURSE SVGPP_HEADERS RELATIVE  ${CMAKE_CURRENT_SOURCE_DIR}/../../include/svgpp ${CMAKE_CURRENT_SOURCE_DIR}/../../include/svgpp/*.*)

foreach(i ${SVGPP_HEADERS})
  get_filename_component(_dir ${i} PATH)
  set(_pregroup "Headers/${_dir}")
  set(_name "../../include/svgpp/${i}")
  list(APPEND SOURCES
    ${_name}
  )
  string(REPLACE "/" "\\" _group ${_pregroup})
  source_group(${_group} FILES ${_name})
endforeach()

add_executable( ParserGTest
  ${SOURCES}
  basic_shapes_test.cpp 
  color_grammar_test.cpp 
  dictionary_test.cpp
  attribute_traversal_test.cpp 
  css_style_iterator_test.cpp 
	clock_value_grammar_test.cpp
  document_traversal_a_test.cpp  
  icc_color_grammar_test.cpp 
  length_factory_test.cpp 
  list_of_points_test.cpp 
  #path_adapter_test.cpp 
  path_grammar_test.cpp 
  path_markers_adapter_test.cpp 
  polyline_markers_test.cpp 
  preserveAspectRatio_test.cpp 
  #supported_value_parsers_test.cpp 
  transform_grammar_test.cpp 
  transform_strict_grammar_test.cpp 
  urange_grammar_test.cpp 
  value_parser_test.cpp
  value_parser_length_test.cpp 
  value_parser_paint_ie_test.cpp 
  value_parser_paint_test.cpp 
	value_parser_path_test.cpp 
	value_parser_transform_test.cpp 
  ${GMOCK_DIR}/src/gmock_main.cc 
  ${GMOCK_DIR}/src/gmock-all.cc 
  ${GTEST_DIR}/src/gtest-all.cc 
)

if (UNIX)
  target_link_libraries(ParserGTest
    pthread
  )
endif()

set(CHECK_HEADERS_DIR check_headers)

file(GLOB_RECURSE SVGPP_HEADERS_HPP RELATIVE  ${CMAKE_CURRENT_SOURCE_DIR}/../../include/svgpp ${CMAKE_CURRENT_SOURCE_DIR}/../../include/svgpp/*.hpp)

foreach(i ${SVGPP_HEADERS_HPP})
  if (NOT ${i} MATCHES "policy/xml/.*")
    string(REPLACE "/" "_" _name ${i})
    string(REPLACE "." "_" _file_name ${_name})
    set(_file_name "${CMAKE_BINARY_DIR}/${CHECK_HEADERS_DIR}/check_${_file_name}.cpp")
    #message(${_file_name})
    file(WRITE ${_file_name} "#include <svgpp/${i}>")
    list(APPEND CHECK_HEADERS_SOURCES ${_file_name})
  endif()
endforeach()


add_library(CheckHeadersAutonomy STATIC
  ${CHECK_HEADERS_SOURCES}
)

if (1)
	add_executable(FullParseTest
		full_parse_main.cpp
		full_parse_test.cpp
		full_parse_test2.cpp
		full_parse_test_common.hpp
	)

	if (UNIX)
		target_link_libraries(FullParseTest
			boost_timer
			boost_system
		)
	endif()
endif()