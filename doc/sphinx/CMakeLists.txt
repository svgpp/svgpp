cmake_minimum_required (VERSION 2.6)

project(Documentation)

add_custom_target(
  Documentation ALL 
  SOURCES 
    basic_shapes.rst 
    color.rst 
    document_traversal.rst
    error.rst 
    index.rst 
    iri.rst 
    length.rst 
    lesson01.rst 
	markers.rst 
    overview.rst 
    path.rst 
	text.rst
    transform.rst 
    value.rst 
    value_parser.rst 
	viewport.rst 
)

if (WIN32)
  add_custom_command(TARGET Documentation
    POST_BUILD
    COMMAND make.bat html
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
endif (WIN32)    