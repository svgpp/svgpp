#pragma once

#define BOOST_PARAMETER_MAX_ARITY 15

#include <agg_color_rgba.h>
#include <svgpp/factory/integer_color.hpp>
#include <svgpp/factory/unitless_length.hpp>
#ifdef USE_MSXML
# define NOMINMAX
# include <MsXml2.h>
# include <windows.h>
# include <svgpp/xml/msxml.hpp>
#else
# include <rapidxml_ns/rapidxml_ns.hpp>
# include <svgpp/xml/rapidxml_ns.hpp>
#endif
#include <boost/function.hpp>
#include <boost/tuple/tuple.hpp>
#include <map>

struct agg_rgba8_color_factory
{
  typedef agg::rgba8 color_type;

  static color_type create(unsigned char r, unsigned char g, unsigned char b)
  {
    return agg::rgba8(r, g, b);
  }
};

typedef svgpp::factory::color::percentage_adapter<agg_rgba8_color_factory> color_factory_t;
typedef svgpp::factory::length::unitless<> length_factory_t;
typedef boost::tuple<double, double, double, double> bounding_box_t;
typedef boost::function<bounding_box_t()> get_bounding_box_func_t;

#ifdef USE_MSXML
typedef boost::intrusive_ptr<IXMLDOMNode> XMLElement;
typedef std::wstring svg_string_t;
#else
typedef rapidxml_ns::xml_node<> const * XMLElement;
typedef std::string svg_string_t;
#endif

class XMLDocument
{
public:
  XMLDocument(XMLElement const & root)
    : root_(root)
  {}

  XMLElement find_element_by_id(svg_string_t const & id);

private:
#ifndef USE_MSXML
  typedef std::map<svg_string_t, XMLElement> element_by_id_t;
  element_by_id_t element_by_id_;
#endif

  XMLElement const root_;
};