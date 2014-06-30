#pragma once

#include <svgpp/detail/names_dictionary.hpp>
#include <svgpp/detail/namespace.hpp>

namespace svgpp { namespace detail 
{

template<class AttributeName>
inline attribute_id attribute_name_to_id(namespace_id attribute_namespace, AttributeName const & attribute_name)
{
  switch (attribute_namespace)
  {
  case namespace_id::svg:
    return svg_attribute_name_to_id_dictionary::find(attribute_name);
  case namespace_id::xml:
    return xml_attribute_name_to_id_dictionary::find(attribute_name);
  case namespace_id::xlink:
    return xlink_attribute_name_to_id_dictionary::find(attribute_name);
  default:
    return unknown_attribute_id;
  }
}

}}