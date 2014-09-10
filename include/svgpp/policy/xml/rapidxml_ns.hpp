// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

// RapidXML NS headers must be already included by user

#include <svgpp/definitions.hpp>
#include <svgpp/detail/namespace.hpp>
#include <svgpp/policy/xml/fwd.hpp>
#include <boost/range/iterator_range.hpp>

namespace svgpp { namespace policy { namespace xml
{

template<class Ch>
struct attribute_iterator<rapidxml_ns::xml_attribute<Ch> const *>
{
  typedef rapidxml_ns::xml_attribute<Ch> const * iterator_type;
  typedef boost::iterator_range<Ch const *> string_type;
  typedef string_type attribute_name_type;
  typedef string_type attribute_value_type;
  typedef string_type saved_value_type;

  static string_type get_string_range(string_type const & str)
  { 
    return str;
  }

  static void advance(iterator_type & xml_attribute)
  {
    xml_attribute = xml_attribute->next_attribute();
  }

  static bool is_end(iterator_type xml_attribute)
  {
    return xml_attribute == NULL;
  }

  static BOOST_SCOPED_ENUM(detail::namespace_id) get_namespace(iterator_type xml_attribute)
  {
    if (xml_attribute->namespace_uri_size() == 0)
      return detail::namespace_id::svg;
    boost::iterator_range<Ch const *> ns_uri(xml_attribute->namespace_uri(), 
      xml_attribute->namespace_uri() + xml_attribute->namespace_uri_size());
    if (boost::range::equal(detail::xml_namespace_uri<Ch>(), ns_uri))
      return detail::namespace_id::xml;
    else if (boost::range::equal(detail::xlink_namespace_uri<Ch>(), ns_uri))
      return detail::namespace_id::xlink;
    return detail::namespace_id::other;
  }

  static attribute_name_type get_local_name(iterator_type xml_attribute)
  {
    return attribute_name_type(xml_attribute->local_name(), xml_attribute->local_name() + xml_attribute->local_name_size());
  }

  static attribute_value_type get_value(iterator_type xml_attribute)
  {
    return attribute_value_type(xml_attribute->value(), xml_attribute->value() + xml_attribute->value_size());
  }

  static saved_value_type save_value(iterator_type xml_attribute)
  {
    return attribute_value_type(xml_attribute->value(), xml_attribute->value() + xml_attribute->value_size());
  }

  // Used only in Error Policy
  static iterator_type get_attribute(iterator_type xml_attribute)
  {
    return xml_attribute;
  }
};

template<class Ch>
struct attribute_iterator<rapidxml_ns::xml_attribute<Ch> *>
  : attribute_iterator<rapidxml_ns::xml_attribute<Ch> const *>
{
  static void advance(typename rapidxml_ns::xml_attribute<Ch> *& xml_attribute)
  {
    xml_attribute = xml_attribute->next_attribute();
  }
};

template<class Ch>
struct element_iterator<rapidxml_ns::xml_node<Ch> const *>
{
  typedef rapidxml_ns::xml_node<Ch> const * iterator_type;
  typedef boost::iterator_range<Ch const *> string_type;
  typedef boost::iterator_range<Ch const *> element_name_type;
  typedef boost::iterator_range<Ch const *> element_text_type;
  typedef rapidxml_ns::xml_attribute<Ch> const * attribute_enumerator_type;

  static string_type get_string_range(string_type const & str)
  { 
    return str;
  }

  static void advance_element(iterator_type & xml_element)
  {
    xml_element = xml_element->next_sibling(); 
    find_next<false>(xml_element);
  }

  static void advance_element_or_text(iterator_type & xml_element)
  {
    xml_element = xml_element->next_sibling(); 
    find_next<true>(xml_element);
  }

  static bool is_end(iterator_type xml_element)
  {
    return xml_element == NULL;
  }

  static bool is_text(iterator_type xml_element)
  {
    return xml_element->type() != rapidxml_ns::node_element;
  }

  static element_name_type get_local_name(iterator_type xml_element)
  {
    return element_name_type(xml_element->local_name(), xml_element->local_name() + xml_element->local_name_size());
  }

  static element_text_type get_text(iterator_type xml_element)
  {
    return element_text_type(xml_element->value(), xml_element->value() + xml_element->value_size());
  }

  static attribute_enumerator_type get_attributes(iterator_type xml_element)
  {
    return xml_element->first_attribute();
  }

  static iterator_type get_child_elements(iterator_type xml_element)
  {
    iterator_type child_element = xml_element->first_node();
    find_next<false>(child_element);
    return child_element;
  }

  static iterator_type get_child_elements_and_texts(iterator_type xml_element)
  {
    iterator_type child_element = xml_element->first_node();
    find_next<true>(child_element);
    return child_element;
  }

private:
  template<bool TextsAlso>
  static void find_next(iterator_type & xml_element)
  {
    // TODO: optimize namespace checking by saving pointer to last namespace_uri() known to be SVG
    for(; xml_element; xml_element = xml_element->next_sibling())
    {
      switch(xml_element->type())
      {
      case rapidxml_ns::node_element:
      {
        boost::iterator_range<Ch const *> ns_uri(xml_element->namespace_uri(), 
          xml_element->namespace_uri() + xml_element->namespace_uri_size());
        if (boost::range::equal(detail::svg_namespace_uri<Ch>(), ns_uri))
          return;
        break;
      }
      case rapidxml_ns::node_data:
      case rapidxml_ns::node_cdata:
        if (TextsAlso)
          return;
        break;
      default:
        break;
      }
    }
  }
};

template<class Ch>
struct element_iterator<rapidxml_ns::xml_node<Ch> *>
  : element_iterator<rapidxml_ns::xml_node<Ch> const *>
{
};

}}}
