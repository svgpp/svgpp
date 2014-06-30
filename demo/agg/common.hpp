#pragma once

#include <agg_color_rgba.h>
#include <svgpp/integer_color_factory.hpp>
#include <rapidxml_ns/rapidxml_ns.hpp>
#include <map>

struct agg_rgba8_color_factory
{
  typedef agg::rgba8 color_type;

  static color_type create(unsigned char r, unsigned char g, unsigned char b)
  {
    return agg::rgba8(r, g, b);
  }
};

typedef svgpp::color_factory_percentage_adapter<agg_rgba8_color_factory> color_factory;

class XMLDocument
{
public:
  XMLDocument(rapidxml_ns::xml_node<> const & root)
    : root_(root)
  {}

  rapidxml_ns::xml_node<> const * find_element_by_id(std::string const & id);

private:
  typedef std::map<std::string, rapidxml_ns::xml_node<> const *> element_by_id_t;

  rapidxml_ns::xml_node<> const & root_;
  element_by_id_t element_by_id_;

  static rapidxml_ns::xml_node<> const * find_element_by_id(rapidxml_ns::xml_node<> const * parent, std::string const & id);
};