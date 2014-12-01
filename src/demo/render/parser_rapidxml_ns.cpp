#include "parser_rapidxml_ns.hpp"
#include <rapidxml_ns/rapidxml_ns_utils.hpp>
#include <map>

namespace
{
  rapidxml_ns::xml_node<> const * FindChildElementById(rapidxml_ns::xml_node<> const * parent, std::string const & id)
  {
    for(rapidxml_ns::xml_node<> const * node = parent->first_node(); node; node = node->next_sibling())
    {
      if (rapidxml_ns::xml_attribute<> const * id_attr = node->first_attribute("id"))
        if (boost::range::equal(boost::iterator_range<const char *>(id_attr->value(), id_attr->value() + id_attr->value_size()), id))
          return node;
      if (rapidxml_ns::xml_node<> const * child_node = FindChildElementById(node, id))
        return child_node;
    }
    return NULL;
  }
}

class XMLDocument::Impl
{
public:
  Impl(const char * fileName)
    : xml_file_(fileName)
  {
    doc_.parse<rapidxml_ns::parse_no_string_terminators>(xml_file_.data());  
  }

  XMLElement getRoot()
  {
    return doc_.first_node();
  }

  XMLElement findElementById(svg_string_t const & id)
  {
    std::pair<element_by_id_t::iterator, bool> ins = element_by_id_.insert(element_by_id_t::value_type(id, XMLElement()));
    if (ins.second)
      ins.first->second = FindChildElementById(getRoot(), id);
    return ins.first->second;
  }

private:
  rapidxml_ns::file<> xml_file_;
  rapidxml_ns::xml_document<> doc_;
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