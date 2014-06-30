#pragma once

#include <boost/exception/detail/attribute_noreturn.hpp>
#include <boost/format.hpp>
#include <boost/range/iterator.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/detail/attribute_name.hpp>
#include <stdexcept>

namespace svgpp
{
  
template<class XMLElement>
class xml_element_error: public std::exception
{
public:
  xml_element_error(XMLElement const & element)
    : element_(element)
  {}

  XMLElement const & element() const // NB: returned reference may be invalid at the catch scope - it depends on XML layer
    { return element_; }

private:
  XMLElement const & element_;
};

template<class XMLElement>
class unknown_element_error: public xml_element_error<XMLElement>
{
public:
  unknown_element_error(XMLElement const & element)
    : xml_element_error(element)
  {}
  
  template<class Name>
  unknown_element_error(XMLElement const & element, Name const & name)
    : xml_element_error(element)
    , message_((boost::format("Unknown SVG element: \"%s\"") % std::string(boost::begin(name), boost::end(name))).str())
  {}

  virtual const char * what() const
  {
    return message_.empty() ? "Unknown SVG element" : message_.c_str();
  }

private:
  std::string const message_;
};

template<class XMLElement>
class unexpected_element_error: public xml_element_error<XMLElement>
{
public:
  unexpected_element_error(XMLElement const & element)
    : xml_element_error(element)
  {}
  
  virtual const char * what() const
  {
    return "Unexpected SVG element";
  }
};

template<class XMLAttribute>
class xml_attribute_error: public std::exception
{
public:
  xml_attribute_error(XMLAttribute const & attribute)
    : attribute_(attribute)
  {}

  XMLAttribute const & attribute() const // NB: returned reference may be invalid - it depends on XML layer
    { return attribute_; }

private:
  XMLAttribute const & attribute_;
};

template<class XMLAttribute>
class unknown_attribute_error: public xml_attribute_error<XMLAttribute>
{
public:
  unknown_attribute_error(XMLAttribute const & attribute)
    : xml_attribute_error(attribute)
  {}
  
  template<class Name>
  unknown_attribute_error(XMLAttribute const & attribute, Name const & name)
    : xml_attribute_error(attribute)
    , message_((boost::format("Unknown SVG attribute: \"%s\"") % std::string(boost::begin(name), boost::end(name))).str())
  {}

  virtual const char * what() const
  {
    return message_.empty() ? "Unknown SVG attribute" : message_.c_str();
  }

private:
  std::string const message_;
};

template<class XMLAttribute>
class unknown_css_property_error: public xml_attribute_error<XMLAttribute>
{
public:
  unknown_css_property_error(XMLAttribute const & attribute)
    : xml_attribute_error(attribute)
  {}
  
  template<class Name>
  unknown_css_property_error(XMLAttribute const & attribute, Name const & name)
    : xml_attribute_error(attribute)
    , message_((boost::format("Unknown CSS property: \"%s\"") % std::string(boost::begin(name), boost::end(name))).str())
  {}

  virtual const char * what() const
  {
    return message_.empty() ? "Unknown CSS property" : message_.c_str();
  }

private:
  std::string const message_;
};

// TODO: add information about XML element
class required_attribute_not_found_error: public std::exception
{
public:
  required_attribute_not_found_error(const char * name)
    : name_(name)
    , message_((boost::format("Required SVG attribute \"%s\" not found") % name).str())
  {}

  virtual const char * what() const { return message_.c_str(); }
  std::string const & name() const { return name_; }

private:
  std::string const name_;
  std::string const message_;
};

class negative_value_error: public std::exception
{
public:
  negative_value_error(const char * name)
    : name_(name)
    , message_((boost::format("Negative value of attribute \"%s\"") % name).str())
  {}

  virtual const char * what() const { return message_.c_str(); }
  std::string const & name() const { return name_; }

private:
  std::string const name_;
  std::string const message_;
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
class invalid_value_error: public std::exception
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

  virtual const char * what() const { return message_.c_str(); }
  std::string const & name() const { return attributeOrCSSPropertyName_; }
  value_type const & value() const { return value_; }

private:
  value_type const value_;
  std::string const attributeOrCSSPropertyName_;
  std::string const message_;
};

namespace policy { namespace error 
{

template<class Context>
struct raise_exception
{
  template<class XMLElement, class ElementName>
  BOOST_ATTRIBUTE_NORETURN static bool unknown_element(Context const &, 
    XMLElement const & element, ElementName const & name,
    typename boost::enable_if<typename detail::is_char_range<ElementName>::type>::type * = NULL)
  {
    throw unknown_element_error<XMLElement>(element, name);
  }

  template<class XMLElement, class ElementName>
  BOOST_ATTRIBUTE_NORETURN static bool unknown_element(Context const &, 
    XMLElement const & element, ElementName const &,
    typename boost::disable_if<typename detail::is_char_range<ElementName>::type>::type * = NULL)
  {
    throw unknown_element_error<XMLElement>(element);
  }

  template<class XMLElement>
  BOOST_ATTRIBUTE_NORETURN static bool element_cant_be_child(Context const &, 
    XMLElement const &)
  {
    throw std::runtime_error("Unknown SVG element");
  }

  template<class XMLAttributesIterator, class AttributeName>
  BOOST_ATTRIBUTE_NORETURN static bool unknown_attribute(Context const &, 
    XMLAttributesIterator const & attribute, 
    AttributeName const & name,
    tag::source::attribute,
    typename boost::enable_if<typename detail::is_char_range<AttributeName>::type>::type * = NULL)
  {
    throw unknown_attribute_error<XMLAttributesIterator>(attribute, name);
  }

  template<class XMLAttributesIterator, class AttributeName>
  BOOST_ATTRIBUTE_NORETURN static bool unknown_attribute(Context const &, 
    XMLAttributesIterator const & attribute, 
    AttributeName const &,
    tag::source::attribute,
    typename boost::disable_if<typename detail::is_char_range<AttributeName>::type>::type * = NULL)
  {
    throw unknown_attribute_error<XMLAttributesIterator>(attribute);
  }

  template<class XMLAttributesIterator, class AttributeName>
  BOOST_ATTRIBUTE_NORETURN static bool unknown_attribute(Context const &, 
    XMLAttributesIterator const & attribute, 
    AttributeName const & name,
    tag::source::css,
    typename boost::enable_if<typename detail::is_char_range<AttributeName>::type>::type * = NULL)
  {
    throw unknown_css_property_error<XMLAttributesIterator>(attribute, name);
  }

  template<class XMLAttributesIterator, class AttributeName>
  BOOST_ATTRIBUTE_NORETURN static bool unknown_attribute(Context const &, 
    XMLAttributesIterator const & attribute, 
    AttributeName const &,
    tag::source::css,
    typename boost::disable_if<typename detail::is_char_range<AttributeName>::type>::type * = NULL)
  {
    throw unknown_css_property_error<XMLAttributesIterator>(attribute);
  }

  template<class AttributeTag>
  BOOST_ATTRIBUTE_NORETURN static bool required_attribute_not_found(Context const &, 
    AttributeTag)
  {
    throw required_attribute_not_found_error(attribute_name<char>::get<AttributeTag>());
  }

  template<class AttributeTag, class AttributeValue>
  BOOST_ATTRIBUTE_NORETURN static bool parse_failed(Context const &, AttributeTag,
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
  BOOST_CONSTEXPR static bool unexpected_element(Context const &, 
    XMLElement const & element)
  {
    throw unexpected_element_error<XMLElement>(element);;
  }

  template<class AttributeTag>
  BOOST_ATTRIBUTE_NORETURN static bool negative_value(Context const &, AttributeTag)
  {
    throw negative_value_error(attribute_name<char>::get<AttributeTag>());
  }
};

}}}