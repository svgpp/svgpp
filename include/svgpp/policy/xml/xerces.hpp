// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

// Xerces headers must be already included by user

#include <svgpp/definitions.hpp>
#include <svgpp/detail/namespace.hpp>
#include <svgpp/policy/xml/fwd.hpp>
#include <boost/noncopyable.hpp>
#include <boost/range/iterator_range.hpp>

namespace svgpp
{

namespace xerces_detail
{

template<class WCharType = wchar_t, class Enable = void>
struct get_char_type
{
  typedef char16_t type;
};

template<class WCharType>
struct get_char_type<WCharType, typename boost::enable_if_c<sizeof(WCharType) == 2>::type>
{
  typedef wchar_t type;
};

struct string_policy
{
protected:
  typedef get_char_type<>::type char_type;

public:
  typedef boost::iterator_range<char_type const *> string_type;

  static string_type get_string_range(XMLCh const * str)
  {
    return string_type(
      reinterpret_cast<char_type const *>(str),
      reinterpret_cast<char_type const *>(str) + xercesc::XMLString::stringLen(str));
  }
};

class attribute_iterator
{
public:
  attribute_iterator(xercesc::DOMNamedNodeMap const * attribute_map)
    : attribute_map_(attribute_map)
    , current_attribute_(0)
  {
    BOOST_ASSERT(attribute_map_);
  }

  attribute_iterator(attribute_iterator const & src)
    : attribute_map_(src.attribute_map_)
    , current_attribute_(src.current_attribute_)
  {}

  void advance()
  {
    ++current_attribute_;
  }

  bool is_end() const
  {
    return current_attribute_ >= attribute_map_->getLength();
  }

  xercesc::DOMAttr const * operator->() const
  {
    return get();
  }

  xercesc::DOMAttr const * get() const
  {
    return static_cast<xercesc::DOMAttr const *>(attribute_map_->item(current_attribute_));
  }

private:
  xercesc::DOMNamedNodeMap const * attribute_map_;
  XMLSize_t current_attribute_;
};

} // namespace xerces_detail

namespace policy { namespace xml
{

template<>
struct attribute_iterator<xerces_detail::attribute_iterator>: 
  xerces_detail::string_policy
{
  typedef xerces_detail::attribute_iterator iterator_type;
  typedef const XMLCh * attribute_name_type;
  typedef const XMLCh * attribute_value_type;
  typedef const XMLCh * saved_value_type;

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
    XMLCh const * uri = xml_attribute->getNamespaceURI();
    if (uri == NULL)
      return detail::namespace_id::svg;
    string_type ns_uri = get_string_range(uri);
    if (boost::range::equal(detail::xml_namespace_uri<char_type>(), ns_uri))
      return detail::namespace_id::xml;
    else if (boost::range::equal(detail::xlink_namespace_uri<char_type>(), ns_uri))
      return detail::namespace_id::xlink;
    else
      return detail::namespace_id::other;
  }

  static attribute_name_type get_local_name(iterator_type const & xml_attribute)
  {
    return xml_attribute->getLocalName();
  }

  static attribute_value_type get_value(iterator_type const & xml_attribute)
  {
    return xml_attribute->getValue();
  }

  static attribute_value_type get_value(saved_value_type const & saved_value)
  {
    return saved_value;
  }

  static saved_value_type save_value(iterator_type const & xml_attribute)
  {
    return xml_attribute->getValue();
  }

  // Used only in Error Policy
  static xercesc::DOMAttr const * get_attribute(iterator_type const & xml_attribute)
  {
    return xml_attribute.get();
  }
};

template<>
struct element_iterator<xercesc::DOMNode const *>
  : xerces_detail::string_policy
{
  typedef xercesc::DOMNode const * iterator_type;
  typedef const XMLCh * element_name_type;
  typedef const XMLCh * element_text_type;
  typedef xerces_detail::attribute_iterator attribute_enumerator_type;

  static void advance_element(iterator_type & xml_node)
  {
    find_next<false, false>(xml_node);
  }

  static void advance_element_or_text(iterator_type & xml_node)
  {
    find_next<false, true>(xml_node);
  }

  static bool is_end(iterator_type xml_node)
  {
    return !xml_node;
  }

  static bool is_text(iterator_type xml_node)
  {
    return xml_node->getNodeType() != xercesc::DOMNode::ELEMENT_NODE;
  }

  static element_name_type get_local_name(iterator_type xml_node)
  {
    return xml_node->getLocalName();
  }

  static element_text_type get_text(iterator_type xml_node)
  {
    return static_cast<xercesc::DOMCharacterData const *>(xml_node)->getData();
  }

  static attribute_enumerator_type get_attributes(iterator_type xml_node)
  {
    return attribute_enumerator_type(xml_node->getAttributes());
  }

  static iterator_type get_child_elements(iterator_type xml_node)
  {
    iterator_type child = xml_node->getFirstChild();
    if (child)
      find_next<true, false>(child);
    return child;
  }

  static iterator_type get_child_elements_and_texts(iterator_type const & xml_node)
  {
    iterator_type child = xml_node->getFirstChild();
    if (child)
      find_next<true, true>(child);
    return child;
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
        xml_node = xml_node->getNextSibling();
        if (!xml_node)
          return;
      }
      first_step = false;

      switch(xml_node->getNodeType())
      {
      case xercesc::DOMNode::ELEMENT_NODE: 
      {
        if (boost::range::equal(detail::svg_namespace_uri<char_type>(), get_string_range(xml_node->getNamespaceURI())))
          return;
        break;
      }
      case xercesc::DOMNode::TEXT_NODE:
      case xercesc::DOMNode::CDATA_SECTION_NODE:
        if (TextsAlso)
          return;
        break;
      default:
        break;
      }
    }
  }
};

template<>
struct element_iterator<xercesc::DOMElement *>
  : element_iterator<xercesc::DOMNode const *>
{};

template<>
struct element_iterator<xercesc::DOMElement const *>
  : element_iterator<xercesc::DOMNode const *>
{};

}}}
