#pragma once

#include <rapidxml_ns/rapidxml_ns.hpp>
#include <svgpp/policy/xml/rapidxml_ns.hpp>
#include <boost/noncopyable.hpp>
#include <memory>

typedef rapidxml_ns::xml_node<> const * XMLElement;
typedef std::string svg_string_t;

class XMLDocument
{
public:
  XMLDocument();
  ~XMLDocument();

  void load(const char * fileName);
  XMLElement getRoot();

  XMLElement findElementById(svg_string_t const & id);

private:
  class Impl;
  std::auto_ptr<Impl> impl_;
};