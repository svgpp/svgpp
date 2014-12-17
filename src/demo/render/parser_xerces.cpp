#include "parser_xerces.hpp"

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <sstream>
#include <map>

using namespace xercesc;

namespace
{
  XMLCh const xml_id_string[] = { 'i', 'd', 0 };

  XMLElement FindChildElementById(XMLElement parent, std::basic_string<XMLCh> const & id)
  {
    for(XMLElement node = parent->getFirstChild(); node; node = node->getNextSibling())
      if (node->getNodeType() == DOMNode::ELEMENT_NODE)
      {
        DOMElement const * el = static_cast<DOMElement const *>(node);
        if (XMLCh const * id_attr = el->getAttribute(xml_id_string))
          if (id == id_attr)
            return node;
        if (XMLElement child_node = FindChildElementById(node, id))
          return child_node;
      }
    return NULL;
  }
}

class XMLDocument::Impl
{
public:
  std::auto_ptr<XercesDOMParser> parser_;

  typedef std::map<svg_string_t, XMLElement> element_by_id_t;
  element_by_id_t element_by_id_;
};

XMLDocument::XMLDocument()
  : impl_(new Impl)
{
  try {
      XMLPlatformUtils::Initialize();
  }
  catch (const XMLException& toCatch) 
  {
      char* message = XMLString::transcode(toCatch.getMessage());
      std::ostringstream ostr;
      ostr << "Error during Xerces initialization: "
            << message;
      XMLString::release(&message);
      throw std::runtime_error(ostr.str());
  }
}

XMLDocument::~XMLDocument()
{
}

void XMLDocument::load(const char * fileName)
{
  impl_->parser_.reset(new XercesDOMParser);
  impl_->parser_->setValidationScheme(XercesDOMParser::Val_Always);
  impl_->parser_->setDoNamespaces(true);    // optional

  try 
  {
    impl_->parser_->parse(fileName);
  }
  catch (const XMLException& toCatch) 
  {
    char* message = XMLString::transcode(toCatch.getMessage());
    std::ostringstream ostr;
    ostr << "Error loading XML document: "
          << message;
    XMLString::release(&message);
    throw std::runtime_error(ostr.str());
  }
  catch (const DOMException& toCatch)
  {
    char* message = XMLString::transcode(toCatch.getMessage());
    std::ostringstream ostr;
    ostr << "Error loading XML document: "
          << message;
    XMLString::release(&message);
    throw std::runtime_error(ostr.str());
  }
  catch (...)
  {
    throw std::runtime_error("Unexpected Exception");
  }
}

XMLElement XMLDocument::getRoot() const
{
  return impl_->parser_->getDocument()->getDocumentElement();
}

XMLElement XMLDocument::findElementById(svg_string_t const & id)
{
  std::pair<Impl::element_by_id_t::iterator, bool> ins = 
    impl_->element_by_id_.insert(Impl::element_by_id_t::value_type(id, XMLElement()));
  if (ins.second)
  {
    std::basic_string<XMLCh> xml_id(
      reinterpret_cast<XMLCh const *>(id.c_str()),
      reinterpret_cast<XMLCh const *>(id.c_str() + id.size()));
    ins.first->second = FindChildElementById(getRoot(), xml_id);
  }
  return ins.first->second;
}