#include "parser_msxml.hpp"

#include <comip.h>
#include <comdef.h>
#include <sstream>

XMLDocument::XMLDocument()
{
  HRESULT hr;
  if (FAILED(hr = CoInitialize(NULL)))
  {
    std::ostringstream ostr;
    ostr << "CoInitialize failed with result 0x" << std::hex << hr << "\n" << std::dec;
    throw std::runtime_error(ostr.str());
  }
}

XMLDocument::~XMLDocument()
{
  root_.reset(); // Must be done before CoUninitialize
  CoUninitialize();
}

void XMLDocument::load(const char * fileName)
{
  //init
  HRESULT hr;

  _com_ptr_t<_com_IIID<IXMLDOMDocument, &IID_IXMLDOMDocument> > docPtr;
  if (FAILED(hr = docPtr.CreateInstance(L"Msxml2.DOMDocument.3.0")))
  {
    std::ostringstream ostr;
    ostr << "Error creating DOMDocument 0x" << std::hex << hr << "\n" << std::dec;
    throw std::runtime_error(ostr.str());
  }

  // Load a document.
  docPtr->put_async(VARIANT_FALSE);
  docPtr->put_resolveExternals(VARIANT_FALSE);
  docPtr->put_validateOnParse(VARIANT_FALSE);
  VARIANT_BOOL load_result;
  if (S_OK != (hr = docPtr->load(_variant_t(fileName), &load_result)))
  {
    _com_ptr_t<_com_IIID<IXMLDOMParseError, &IID_IXMLDOMParseError> > parseError;
    if (SUCCEEDED(docPtr->get_parseError(&parseError)))
    {
      _bstr_t reason;
      if (S_OK == parseError->get_reason(reason.GetAddress()))
      {
        std::ostringstream ostr;
        ostr << "Parse error: " << std::string(reason.GetBSTR(), reason.GetBSTR() + reason.length()) << "\n";
        throw std::runtime_error(ostr.str());
      }
    }
    std::ostringstream ostr;
    ostr << "Error loading XML document 0x" << std::hex << hr << "\n" << std::dec;
    throw std::runtime_error(ostr.str());
  }

  _com_ptr_t<_com_IIID<IXMLDOMElement, &IID_IXMLDOMElement> > root;
  if (FAILED(hr = docPtr->get_documentElement(&root)))
  {
    std::ostringstream ostr;
    ostr << "Error getting documentElement 0x" << std::hex << hr << "\n" << std::dec;
    throw std::runtime_error(ostr.str());
  }

  root_ = root.GetInterfacePtr();
}

XMLElement XMLDocument::findElementById(svg_string_t const & id)
{
  std::wstring xpath = L"//*[@id='" + id + L"']"; // TODO: escape id string, check namespace
  _com_ptr_t<_com_IIID<IXMLDOMNode, &IID_IXMLDOMNode> > node;
  if (S_OK != root_->selectSingleNode(_bstr_t(xpath.c_str()), &node))
    return XMLElement();
  return XMLElement(node.GetInterfacePtr());
}
