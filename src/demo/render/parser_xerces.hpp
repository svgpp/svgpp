#pragma once

#include <xercesc/dom/DOM.hpp>
#include <svgpp/policy/xml/xerces.hpp>
#include <memory>

typedef xercesc::DOMNode const * XMLElement;
typedef std::basic_string<svgpp::xerces_detail::get_char_type<>::type> svg_string_t;

class XMLDocument
{
public:
  XMLDocument();
  ~XMLDocument();

  void load(const char * fileName);
  XMLElement getRoot() const;

  XMLElement findElementById(svg_string_t const & id);

private:
  class Impl;
  std::auto_ptr<Impl> impl_;
};
