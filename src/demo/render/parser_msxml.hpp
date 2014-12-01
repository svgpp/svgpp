#pragma once

#ifndef NOMINMAX
# define NOMINMAX 1
#endif
#include <MsXml2.h>
#include <windows.h>
#undef small
#include <svgpp/policy/xml/msxml.hpp>
#include <boost/noncopyable.hpp>

typedef boost::intrusive_ptr<IXMLDOMNode> XMLElement;
typedef std::wstring svg_string_t;

class XMLDocument
{
public:
  XMLDocument();
  ~XMLDocument();

  void load(const char * fileName);
  XMLElement const & getRoot() { return root_; }

  XMLElement findElementById(svg_string_t const & id);

private:
  XMLElement root_;
};

