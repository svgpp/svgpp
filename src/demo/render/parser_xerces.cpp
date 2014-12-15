#include "parser_xerces.hpp"

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <sstream>

using namespace xercesc;

class XMLDocument::Impl
{
public:
  std::auto_ptr<XercesDOMParser> parser_;
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
  std::basic_string<XMLCh> xml_id(
    reinterpret_cast<XMLCh const *>(&*boost::begin(id)),
    reinterpret_cast<XMLCh const *>(&*boost::end(id)));
  return impl_->parser_->getDocument()->getElementById(xml_id.c_str());
}
