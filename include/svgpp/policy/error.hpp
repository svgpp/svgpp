// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/exception/all.hpp>
#include <boost/format.hpp>
#include <boost/range/iterator.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/detail/attribute_name.hpp>
#include <svgpp/detail/namespace.hpp>
#include <stdexcept>

// BOOST_NORETURN was introduced in Boost 1.56
#ifdef BOOST_NORETURN
#define SVGPP_NORETURN BOOST_NORETURN
#else
#define SVGPP_NORETURN
#endif

namespace svgpp
{
  
namespace tag { namespace error_info
{
  struct xml_element;
  struct xml_attribute;
}}

class exception_base: public virtual boost::exception, public virtual std::exception
{};

class unknown_element_error: public exception_base
{
public:
  unknown_element_error()
  {}
  
  template<class Name>
  unknown_element_error(Name const & name)
    : message_((boost::format("Unknown SVG element: \"%s\"") % std::string(boost::begin(name), boost::end(name))).str())
  {}

  virtual ~unknown_element_error() throw() {}

  virtual const char * what() const throw()
  {
    return message_.empty() ? "Unknown SVG element" : message_.c_str();
  }

private:
  std::string const message_;

  BOOST_DELETED_FUNCTION(unknown_element_error& operator= (unknown_element_error const&))
};

class unexpected_element_error: public exception_base
{
public:
  virtual const char * what() const throw()
  {
    return "Unexpected SVG element";
  }
};

// Exception is thrown only for SVG namespace (or unspecified namespace) attributes
class unknown_attribute_error: public exception_base
{
public:
  unknown_attribute_error()
  {}
  
  template<class Name>
  unknown_attribute_error(Name const & name)
    : message_((boost::format("Unknown attribute: \"%s\"") % std::string(boost::begin(name), boost::end(name))).str())
  {}

  virtual ~unknown_attribute_error() throw() {}

  virtual const char * what() const throw()
  {
    return message_.empty() ? "Unknown attribute" : message_.c_str();
  }

private:
  std::string const message_;

  BOOST_DELETED_FUNCTION(unknown_attribute_error& operator= (unknown_attribute_error const&))
};

class unexpected_attribute_error: public exception_base
{
public:
  unexpected_attribute_error(const char * name)
    : name_(name)
    , message_((boost::format("Unexpected attribute \"%s\"") % name).str())
  {}

  virtual ~unexpected_attribute_error() throw() {}

  virtual const char * what() const throw() { return message_.c_str(); }
  std::string const & name() const { return name_; }

private:
  std::string const name_;
  std::string const message_;

  BOOST_DELETED_FUNCTION(unexpected_attribute_error& operator= (unexpected_attribute_error const&))
};

class unknown_css_property_error: public exception_base
{
public:
  unknown_css_property_error()
  {}
  
  template<class Name>
  unknown_css_property_error(Name const & name)
    : message_((boost::format("Unknown CSS property: \"%s\"") % std::string(boost::begin(name), boost::end(name))).str())
  {}

  virtual ~unknown_css_property_error() throw() {}

  virtual const char * what() const throw()
  {
    return message_.empty() ? "Unknown CSS property" : message_.c_str();
  }

private:
  std::string const message_;

  BOOST_DELETED_FUNCTION(unknown_css_property_error& operator= (unknown_css_property_error const&))
};

class required_attribute_not_found_error: public exception_base
{
public:
  required_attribute_not_found_error(const char * name)
    : name_(name)
    , message_((boost::format("Required SVG attribute \"%s\" not found") % name).str())
  {}

  virtual ~required_attribute_not_found_error() throw() {}

  virtual const char * what() const throw() { return message_.c_str(); }
  std::string const & name() const { return name_; }

private:
  std::string const name_;
  std::string const message_;

  BOOST_DELETED_FUNCTION(required_attribute_not_found_error& operator= (required_attribute_not_found_error const&))
};

class negative_value_error: public exception_base
{
public:
  negative_value_error(const char * name)
    : name_(name)
    , message_((boost::format("Negative value of attribute \"%s\"") % name).str())
  {}

  virtual ~negative_value_error() throw() {}

  virtual const char * what() const throw() { return message_.c_str(); }
  std::string const & name() const { return name_; }

private:
  std::string const name_;
  std::string const message_;

  BOOST_DELETED_FUNCTION(negative_value_error& operator= (negative_value_error const&))
};


namespace detail
{
  template<class Range>
  struct is_char_range
  {
    typedef boost::is_same<
      char,
      typename boost::remove_const<
        typename std::iterator_traits<
          typename boost::range_iterator<Range>::type
        >::value_type
      >::type
    > type;
  };
}

template<class Char>
class invalid_value_error: public exception_base
{
public:
  typedef std::basic_string<Char> value_type;

  template<class Range>
  invalid_value_error(const char * name, Range const & value,
    typename boost::enable_if<typename detail::is_char_range<Range>::type>::type * = NULL)
    : attributeOrCSSPropertyName_(name)
    , value_(boost::begin(value), boost::end(value))
    , message_((boost::format("Invalid value of SVG attribute (or property) \"%s\": \"%s\"") 
      % attributeOrCSSPropertyName_ % value_).str())
  {}

  template<class Range>
  invalid_value_error(const char * name, Range const & value,
    typename boost::disable_if<typename detail::is_char_range<Range>::type>::type * = NULL)
    : attributeOrCSSPropertyName_(name)
    , value_(boost::begin(value), boost::end(value))
    , message_((boost::format("Invalid value of SVG attribute (or property) \"%s\"") 
      % attributeOrCSSPropertyName_).str())
  {}

  virtual ~invalid_value_error() throw() {}

  virtual const char * what() const throw() { return message_.c_str(); }
  std::string const & name() const { return attributeOrCSSPropertyName_; }
  value_type const & value() const { return value_; }

private:
  value_type const value_;
  std::string const attributeOrCSSPropertyName_;
  std::string const message_;

  BOOST_DELETED_FUNCTION(invalid_value_error& operator= (invalid_value_error const&))
};

namespace policy { namespace error 
{

template<class Context>
struct raise_exception
{
  typedef Context context_type;

  template<class XMLElement, class ElementName>
  SVGPP_NORETURN static bool unknown_element(Context const &, 
    XMLElement const & element, ElementName const & name,
    typename boost::enable_if<typename detail::is_char_range<ElementName>::type>::type * = NULL)
  {
    throw unknown_element_error(name) << boost::error_info<tag::error_info::xml_element, XMLElement>(element);
  }

  template<class XMLElement, class ElementName>
  SVGPP_NORETURN static bool unknown_element(Context const &, 
    XMLElement const & element, ElementName const &,
    typename boost::disable_if<typename detail::is_char_range<ElementName>::type>::type * = NULL)
  {
    throw unknown_element_error() << boost::error_info<tag::error_info::xml_element, XMLElement>(element);
  }

  template<class XMLAttribute, class AttributeName>
  static bool unknown_attribute(Context const &, 
    XMLAttribute const & attribute, 
    AttributeName const & name,
    BOOST_SCOPED_ENUM(detail::namespace_id) namespace_id,
    tag::source::attribute,
    typename boost::enable_if<typename detail::is_char_range<AttributeName>::type>::type * = NULL)
  {
    if (namespace_id == detail::namespace_id::svg)
      throw unknown_attribute_error(name) 
        << boost::error_info<tag::error_info::xml_attribute, XMLAttribute>(attribute);
    else
      return true;
  }

  template<class XMLAttribute, class AttributeName>
  static bool unknown_attribute(Context const &, 
    XMLAttribute const & attribute, 
    AttributeName const &,
    BOOST_SCOPED_ENUM(detail::namespace_id) namespace_id,
    tag::source::attribute,
    typename boost::disable_if<typename detail::is_char_range<AttributeName>::type>::type * = NULL)
  {
    if (namespace_id == detail::namespace_id::svg)
      throw unknown_attribute_error() 
        << boost::error_info<tag::error_info::xml_attribute, XMLAttribute>(attribute);
    else
      return true;
  }

  template<class XMLAttribute, class AttributeName>
  SVGPP_NORETURN static bool unknown_attribute(Context const &, 
    XMLAttribute const & attribute, 
    AttributeName const & name,
    tag::source::css,
    typename boost::enable_if<typename detail::is_char_range<AttributeName>::type>::type * = NULL)
  {
    throw unknown_css_property_error(name) 
      << boost::error_info<tag::error_info::xml_attribute, XMLAttribute>(attribute);
  }

  template<class XMLAttribute, class AttributeName>
  SVGPP_NORETURN static bool unknown_attribute(Context const &, 
    XMLAttribute const & attribute, 
    AttributeName const &,
    tag::source::css,
    typename boost::disable_if<typename detail::is_char_range<AttributeName>::type>::type * = NULL)
  {
    throw unknown_css_property_error() 
      << boost::error_info<tag::error_info::xml_attribute, XMLAttribute>(attribute);
  }

  SVGPP_NORETURN static bool unexpected_attribute(Context const &, 
    detail::attribute_id id, tag::source::attribute)
  {
    throw unexpected_attribute_error(attribute_name<char>::by_id(id));
  }
  
  template<class AttributeTag>
  SVGPP_NORETURN static bool required_attribute_not_found(Context const &, 
    AttributeTag)
  {
    throw required_attribute_not_found_error(attribute_name<char>::get<AttributeTag>());
  }

  template<class AttributeTag, class AttributeValue>
  SVGPP_NORETURN static bool parse_failed(Context const &, AttributeTag,
    AttributeValue const & value)
  {
    typedef typename boost::remove_const<
        typename std::iterator_traits<
          typename boost::range_iterator<AttributeValue>::type
        >::value_type
      >::type char_t;
    throw invalid_value_error<char_t>(attribute_name<char>::get<AttributeTag>(), value);
  }

  template<class XMLElement>
  SVGPP_NORETURN static bool unexpected_element(Context const &, 
    XMLElement const & element)
  {
    throw unexpected_element_error() 
      << boost::error_info<tag::error_info::xml_element, XMLElement>(element);
  }

  template<class AttributeTag>
  SVGPP_NORETURN static bool negative_value(Context const &, AttributeTag)
  {
    throw negative_value_error(attribute_name<char>::get<AttributeTag>());
  }

  typedef boost::exception intercepted_exception_type;

  template<class XMLElement>
  SVGPP_NORETURN static bool add_element_info(intercepted_exception_type & e, 
    XMLElement const & element)
  {
    typedef boost::error_info<tag::error_info::xml_element, XMLElement> error_info;
    if (boost::get_error_info<error_info>(e) == NULL)
      e << error_info(element);
    throw;
  }
};

template<class Context>
struct default_policy: raise_exception<Context>
{};

}}}