#pragma once

#include "common.hpp"

class Filters
{
public:
  Filters(XMLDocument & xml_document)
    : xml_document_(xml_document)
  {}

  void get(
    svg_string_t const & id, 
    length_factory_t const &/*, 
    get_bounding_box_func_t const & get_bounding_box*/);

private:
  XMLDocument & xml_document_;
};