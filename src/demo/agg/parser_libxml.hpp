#pragma once

#include <libxml/tree.h>
#include <svgpp/policy/xml/libxml2.hpp>
#include <boost/noncopyable.hpp>
#include <memory>

typedef xmlNode * XMLElement;
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