// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

// Microsoft XML headers must be already included by user

#include <svgpp/definitions.hpp>
#include <svgpp/detail/namespace.hpp>
#include <svgpp/policy/xml/fwd.hpp>
#include <boost/noncopyable.hpp>
#include <boost/range/iterator_range.hpp>

#ifndef SVGPP_MSXML_NAMESPACE
# define SVGPP_MSXML_NAMESPACE 
#endif

#ifndef SVGPP_MSXML_BOOST_INTRUSIVE_PTR_DEFINED
namespace boost
{
  inline void intrusive_ptr_add_ref(SVGPP_MSXML_NAMESPACE::IXMLDOMNode * ptr)
  { ptr->AddRef(); }

  inline void intrusive_ptr_release(SVGPP_MSXML_NAMESPACE::IXMLDOMNode * ptr)
  { ptr->Release(); }

  inline void intrusive_ptr_add_ref(SVGPP_MSXML_NAMESPACE::IXMLDOMNamedNodeMap * ptr)
  { ptr->AddRef(); }

  inline void intrusive_ptr_release(SVGPP_MSXML_NAMESPACE::IXMLDOMNamedNodeMap * ptr)
  { ptr->Release(); }
}
#endif

#include <boost/intrusive_ptr.hpp>

namespace svgpp
{

namespace msxml_detail
{

class bstr_t: boost::noncopyable
{
public:
  bstr_t()
    : str_(NULL)
  {}

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
  bstr_t(bstr_t && src)
    : str_(src.str_)
  {
    src.str_ = NULL;
  }
#else
  bstr_t(bstr_t & src)
    : str_(src.str_)
  {
    src.str_ = NULL;
  }

  struct ref
  {
    BSTR ptr_;
      
    explicit ref(BSTR ptr): ptr_(ptr) { }
  };
  
  bstr_t(ref __ref) throw()
      : str_(__ref.ptr_) 
  {}

  operator ref() throw()
  { 
    BSTR str = str_;
    str_ = NULL;
    return ref(str); 
  } 
#endif

  ~bstr_t()
  {
    if (str_)
      ::SysFreeString(str_);
  }

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
  bstr_t & operator=(bstr_t && src)
  {
    if (str_)
      ::SysFreeString(str_);
    str_ = src.str_;
    src.str_ = NULL;
    return *this;
  }
#else
  bstr_t & operator=(bstr_t & src)
  {
    if (str_)
      ::SysFreeString(str_);
    str_ = src.str_;
    src.str_ = NULL;
    return *this;
  }

  bstr_t & operator=(ref r)
  {
    if (str_)
      ::SysFreeString(str_);
    str_ = r.ptr_;
    return *this;
  }
#endif

  void assign(BSTR val)
  {
    if (str_)
      ::SysFreeString(str_);
    str_ = val;
  }

  BSTR * operator&()
  {
    if (str_)
      ::SysFreeString(str_);
    str_ = NULL;
    return &str_;
  }

  boost::iterator_range<wchar_t const *> get_range() const
  {
    return boost::iterator_range<wchar_t const *>(str_, str_ + ::SysStringLen(str_));
  }

private:
  BSTR str_;
};

struct bstr_policy
{
  typedef boost::iterator_range<wchar_t const *> string_type;

  static string_type get_string_range(bstr_t const & str)
  {
    return str.get_range();
  }
};

typedef boost::intrusive_ptr<SVGPP_MSXML_NAMESPACE::IXMLDOMNode> node_ptr;
typedef boost::intrusive_ptr<SVGPP_MSXML_NAMESPACE::IXMLDOMNamedNodeMap> attribute_map_ptr;
typedef boost::intrusive_ptr<SVGPP_MSXML_NAMESPACE::IXMLDOMAttribute> attribute_ptr;

class attribute_iterator: boost::noncopyable
{
public:
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
  attribute_iterator(attribute_iterator && src)
  {
    attribute_map_.swap(src.attribute_map_);
    current_attribute_.swap(src.current_attribute_);
  }
#else
  attribute_iterator(attribute_iterator const & src)
    : attribute_map_(src.attribute_map_)
    , current_attribute_(src.current_attribute_)
  {}
#endif

  attribute_iterator(attribute_map_ptr const & attribute_map)
    : attribute_map_(attribute_map)
  {
    BOOST_ASSERT(attribute_map_);
    advance();
  }

  void advance()
  {
    BOOST_ASSERT(attribute_map_);
    if (!attribute_map_)
      return;
    current_attribute_.reset();
    SVGPP_MSXML_NAMESPACE::IXMLDOMNode * node = NULL;
    if (attribute_map_->nextNode(&node) == S_OK)
    {
      SVGPP_MSXML_NAMESPACE::IXMLDOMAttribute * attr = NULL;
      HRESULT hr = node->QueryInterface(SVGPP_MSXML_NAMESPACE::IID_IXMLDOMAttribute, (void**)&attr);
      node->Release();
      BOOST_ASSERT(hr == S_OK);
      if (hr == S_OK)
        attribute_ptr(attr, false).swap(current_attribute_);
    }
  }

  bool is_end() const
  {
    BOOST_ASSERT(attribute_map_);
    return !attribute_map_ || !current_attribute_;
  }

  SVGPP_MSXML_NAMESPACE::IXMLDOMAttribute * operator->() const
  {
    return current_attribute_.get();
  }

  attribute_ptr const & operator*() const { return current_attribute_; }

private:
  attribute_map_ptr attribute_map_;
  attribute_ptr current_attribute_;
};

}

namespace policy { namespace xml
{

template<>
struct attribute_iterator<msxml_detail::attribute_iterator>: 
  msxml_detail::bstr_policy
{
  typedef msxml_detail::attribute_iterator iterator_type;
  typedef msxml_detail::bstr_t attribute_name_type;
  typedef msxml_detail::bstr_t attribute_value_type;
  typedef msxml_detail::attribute_ptr saved_value_type;

  static void advance(iterator_type & xml_attribute)
  {
    xml_attribute.advance();
  }

  static bool is_end(iterator_type const & xml_attribute)
  {
    return xml_attribute.is_end();
  }

  static BOOST_SCOPED_ENUM(detail::namespace_id) get_namespace(iterator_type const & xml_attribute)
  {
    attribute_name_type uri;
    if (S_OK != xml_attribute->get_namespaceURI(&uri))
      return detail::namespace_id::svg;
    string_type ns_uri = get_string_range(uri);
    if (boost::range::equal(detail::xml_namespace_uri<wchar_t>(), ns_uri))
      return detail::namespace_id::xml;
    else if (boost::range::equal(detail::xlink_namespace_uri<wchar_t>(), ns_uri))
      return detail::namespace_id::xlink;
    else
      return detail::namespace_id::other;
  }

  static attribute_name_type get_local_name(iterator_type const & xml_attribute)
  {
    attribute_name_type str;
    BOOST_VERIFY(S_OK == xml_attribute->get_baseName(&str));
    return str;
  }

  static attribute_value_type get_value(iterator_type const & xml_attribute)
  {
    attribute_value_type str;
    BOOST_VERIFY(S_OK == xml_attribute->get_text(&str));
    return str;
  }

  static attribute_value_type get_value(saved_value_type const & xml_attribute)
  {
    attribute_value_type str;
    BOOST_VERIFY(S_OK == xml_attribute->get_text(&str));
    return str;
  }

  static saved_value_type save_value(iterator_type const & xml_attribute)
  {
    return *xml_attribute;
  }

  // Used only in Error Policy
  static msxml_detail::attribute_ptr get_attribute(iterator_type const & xml_attribute)
  {
    return *xml_attribute;
  }
};

template<>
struct element_iterator<msxml_detail::node_ptr>:
  msxml_detail::bstr_policy
{
  typedef msxml_detail::node_ptr iterator_type;
  typedef msxml_detail::bstr_t element_name_type;
  typedef msxml_detail::bstr_t element_text_type;
  typedef msxml_detail::attribute_iterator attribute_enumerator_type;

  static void advance_element(iterator_type & xml_node)
  {
    find_next<false, false>(xml_node);
  }

  static void advance_element_or_text(iterator_type & xml_node)
  {
    find_next<false, true>(xml_node);
  }

  static bool is_end(iterator_type const & xml_node)
  {
    return !xml_node;
  }

  static bool is_text(iterator_type const & xml_node)
  {
    BOOST_ASSERT(xml_node);
    SVGPP_MSXML_NAMESPACE::DOMNodeType type;
    return xml_node && S_OK == xml_node->get_nodeType(&type) && type != 1 /*NODE_ELEMENT*/;
  }

  static element_name_type get_local_name(iterator_type const & xml_node)
  {
    element_name_type str;
    BOOST_VERIFY(S_OK == xml_node->get_baseName(&str));
    return str;
  }

  static element_text_type get_text(iterator_type const & xml_node)
  {
    element_text_type str;
    BOOST_VERIFY(S_OK == xml_node->get_text(&str));
    return str;
  }

  static attribute_enumerator_type get_attributes(iterator_type const & xml_node)
  {
    SVGPP_MSXML_NAMESPACE::IXMLDOMNamedNodeMap * attributeMap = NULL;
    if (S_OK == xml_node->get_attributes(&attributeMap))
      return attribute_enumerator_type(msxml_detail::attribute_map_ptr(attributeMap, false));
    else
    {
      BOOST_ASSERT(false);
      return attribute_enumerator_type(msxml_detail::attribute_map_ptr());
    }
  }

  static iterator_type get_child_elements(iterator_type const & xml_node)
  {
    SVGPP_MSXML_NAMESPACE::IXMLDOMNode * first_child = NULL;
    if (S_OK == xml_node->get_firstChild(&first_child))
    {
      iterator_type out_it(first_child, false);
      find_next<true, false>(out_it);
      return out_it;
    }
    else
      return iterator_type();
  }

  static iterator_type get_child_elements_and_texts(iterator_type const & xml_node)
  {
    SVGPP_MSXML_NAMESPACE::IXMLDOMNode * first_child = NULL;
    if (S_OK == xml_node->get_firstChild(&first_child))
    {
      iterator_type out_it(first_child, false);
      find_next<true, true>(out_it);
      return out_it;
    }
    else
      return iterator_type();
  }

private:
  template<bool CheckPassedNode, bool TextsAlso>
  static void find_next(iterator_type & xml_node)
  {
    if (!xml_node)
      return;

    bool first_step = true;
    for(;;)
    {
      if (!CheckPassedNode || !first_step)
      {
        IXMLDOMNode * nextSibling = NULL;
        switch(xml_node->get_nextSibling(&nextSibling))
        {
        case S_OK:
          iterator_type(nextSibling, false).swap(xml_node);
          break;
        default:
          BOOST_ASSERT(false);
        case S_FALSE:
          xml_node.reset();
          return;
        }
      }
      first_step = false;

      SVGPP_MSXML_NAMESPACE::DOMNodeType type;
      if (S_OK != xml_node->get_nodeType(&type))
      {
        BOOST_ASSERT(false);
        xml_node.reset();
        return;
      }

      switch(type)
      {
      case 1: /*NODE_ELEMENT*/
      {
        msxml_detail::bstr_t uri;
        if (S_OK == xml_node->get_namespaceURI(&uri)
          && boost::range::equal(detail::svg_namespace_uri<wchar_t>(), uri.get_range()))
          return;
        break;
      }
      case 3: /*NODE_TEXT*/
      case 4: /*NODE_CDATA_SECTION*/
        if (TextsAlso)
          return;
        break;
      }
    }
  }
};

template<>
struct element_iterator<SVGPP_MSXML_NAMESPACE::IXMLDOMElement *>
  : element_iterator<msxml_detail::node_ptr>
{};

template<>
struct element_iterator<SVGPP_MSXML_NAMESPACE::IXMLDOMNode *>
  : element_iterator<msxml_detail::node_ptr>
{};

}}}
