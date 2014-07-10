#pragma once

#include <boost/exception/all.hpp>
#include <boost/format.hpp>
#include <boost/range/iterator.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/detail/attribute_name.hpp>
#include <stdexcept>

namespace svgpp
{
  
namespace tag { namespace error_info
{
  struct xml_element;
  struct xml_attribute;
}}

class unknown_element_error: public virtual boost::exception, public virtual std::exception
{
public:
  unknown_element_error()
  {}
  
  template<class Name>
  unknown_element_error(Name const & name)
    : message_((boost::format("Unknown SVG element: \"%s\"") % std::string(boost::begin(name), boost::end(name))).str())
  {}

  virtual const char * what() const
  {
    return message_.empty() ? "Unknown SVG element" : message_.c_str();
  }

private:
  std::string const message_;
};

class unexpected_element_error: public virtual boost::exception, public virtual std::exception
{
public:
  virtual const char * what() const
  {
    return "Unexpected SVG element";
  }
};

class unknown_attribute_error: public virtual boost::exception, public virtual std::exception
{
public:
  unknown_attribute_error()
  {}
  
  template<class Name>
  unknown_attribute_error(Name const & name)
    : message_((boost::format("Unknown SVG attribute: \"%s\"") % std::string(boost::begin(name), boost::end(name))).str())
  {}

  virtual const char * what() const
  {
    return message_.empty() ? "Unknown SVG attribute" : message_.c_str();
  }

private:
  std::string const message_;
};

class unknown_css_property_error: public virtual boost::exception, public virtual std::exception
{
public:
  unknown_css_property_error()
  {}
  
  template<class Name>
  unknown_css_property_error(Name const & name)
    : message_((boost::format("Unknown CSS property: \"%s\"") % std::string(boost::begin(name), boost::end(name))).str())
  {}

  virtual const char * what() const
  {
    return message_.empty() ? "Unknown CSS property" : message_.c_str();
  }

private:
  std::string const message_;
};

// TODO: add information about XML element
class required_attribute_not_found_error: public virtual boost::exception, public virtual std::exception
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

class negative_value_error: public virtual boost::exception, public virtual std::exception
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
class invalid_value_error: public virtual boost::exception, public virtual std::exception
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
  typedef Context context_type;

  template<class XMLElement, class ElementName>
  BOOST_ATTRIBUTE_NORETURN static bool unknown_element(Context const &, 
    XMLElement const & element, ElementName const & name,
    typename boost::enable_if<typename detail::is_char_range<ElementName>::type>::type * = NULL)
  {
    throw unknown_element_error(name) << boost::error_info<tag::error_info::xml_element, XMLElement const &>(element);
  }

  template<class XMLElement, class ElementName>
  BOOST_ATTRIBUTE_NORETURN static bool unknown_element(Context const &, 
    XMLElement const & element, ElementName const &,
    typename boost::disable_if<typename detail::is_char_range<ElementName>::type>::type * = NULL)
  {
    throw unknown_element_error() << boost::error_info<tag::error_info::xml_element, XMLElement const &>(element);
  }

  template<class XMLAttributesIterator, class AttributeName>
  BOOST_ATTRIBUTE_NORETURN static bool unknown_attribute(Context const &, 
    XMLAttributesIterator const & attribute, 
    AttributeName const & name,
    tag::source::attribute,
    typename boost::enable_if<typename detail::is_char_range<AttributeName>::type>::type * = NULL)
  {
    throw unknown_attribute_error(name) 
      << boost::error_info<tag::error_info::xml_attribute, XMLAttributesIterator const &>(attribute);
  }

  template<class XMLAttributesIterator, class AttributeName>
  BOOST_ATTRIBUTE_NORETURN static bool unknown_attribute(Context const &, 
    XMLAttributesIterator const & attribute, 
    AttributeName const &,
    tag::source::attribute,
    typename boost::disable_if<typename detail::is_char_range<AttributeName>::type>::type * = NULL)
  {
    throw unknown_attribute_error() 
      << boost::error_info<tag::error_info::xml_attribute, XMLAttributesIterator const &>(attribute);
  }

  template<class XMLAttributesIterator, class AttributeName>
  BOOST_ATTRIBUTE_NORETURN static bool unknown_attribute(Context const &, 
    XMLAttributesIterator const & attribute, 
    AttributeName const & name,
    tag::source::css,
    typename boost::enable_if<typename detail::is_char_range<AttributeName>::type>::type * = NULL)
  {
    throw unknown_css_property_error(name) 
      << boost::error_info<tag::error_info::xml_attribute, XMLAttributesIterator const &>(attribute);
  }

  template<class XMLAttributesIterator, class AttributeName>
  BOOST_ATTRIBUTE_NORETURN static bool unknown_attribute(Context const &, 
    XMLAttributesIterator const & attribute, 
    AttributeName const &,
    tag::source::css,
    typename boost::disable_if<typename detail::is_char_range<AttributeName>::type>::type * = NULL)
  {
    throw unknown_css_property_error() 
      << boost::error_info<tag::error_info::xml_attribute, XMLAttributesIterator const &>(attribute);
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
    throw unexpected_element_error() 
      << boost::error_info<tag::error_info::xml_element, XMLElement const &>(element);
  }

  template<class AttributeTag>
  BOOST_ATTRIBUTE_NORETURN static bool negative_value(Context const &, AttributeTag)
  {
    throw negative_value_error(attribute_name<char>::get<AttributeTag>());
  }
};

template<class Context>
struct default_policy: raise_exception<Context>
{};

}}}