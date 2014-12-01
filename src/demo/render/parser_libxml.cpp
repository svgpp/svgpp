#include "parser_libxml.hpp"
#include <map>
#include <stdexcept>
#include <libxml/parser.h>

namespace
{
  xmlNode * FindChildElementById(xmlNode const * parent, std::string const & id)
  {
    for(xmlNode * node = parent->children; node; node = node->next)
    {
      if (xmlChar const * id_attr = xmlGetProp(node, reinterpret_cast<xmlChar const *>("id")))
        if (boost::range::equal(
          boost::iterator_range<const char *>(boost::as_literal(reinterpret_cast<const char *>(id_attr))), id))
          return node;
      if (xmlNode * child_node = FindChildElementById(node, id))
        return child_node;
    }
    return NULL;
  }
}

class XMLDocument::Impl
{
public:
  Impl(const char * fileName)
  {
    LIBXML_TEST_VERSION    // Macro to check API for match with
                           // the DLL we are using

    /*parse the file and get the DOM */
    if ((doc_ = xmlReadFile(fileName, NULL, 0)) == NULL)
      throw std::runtime_error("Could not parse file");
  }

  ~Impl()
  {
    if (doc_)
      xmlFreeDoc(doc_);
    xmlCleanupParser();
  }

  XMLElement getRoot()
  {
    return xmlDocGetRootElement(doc_);
  }

  XMLElement findElementById(svg_string_t const & id)
  {
    std::pair<element_by_id_t::iterator, bool> ins = element_by_id_.insert(element_by_id_t::value_type(id, XMLElement()));
    if (ins.second)
      ins.first->second = FindChildElementById(getRoot(), id);
    return ins.first->second;
  }

private:
  xmlDoc * doc_;
  typedef std::map<svg_string_t, XMLElement> element_by_id_t;
  element_by_id_t element_by_id_;
};

XMLDocument::XMLDocument()
{}

XMLDocument::~XMLDocument()
{}

void XMLDocument::load(const char * fileName)
{
  impl_.reset(new Impl(fileName));
}

XMLElement XMLDocument::getRoot()
{
  return impl_->getRoot();
}

XMLElement XMLDocument::findElementById(svg_string_t const & id)
{
  return impl_->findElementById(id);
}
