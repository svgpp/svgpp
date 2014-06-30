#pragma once

#include <boost/parameter.hpp>
#include <boost/noncopyable.hpp>
#include <svgpp/detail/names_dictionary.hpp>

namespace svgpp
{

BOOST_PARAMETER_TEMPLATE_KEYWORD(xml_attribute_iterator_policy)
BOOST_PARAMETER_TEMPLATE_KEYWORD(css_name_to_id_policy)

struct css_name_to_id_policy_default
{
  template<class Range>
  static detail::attribute_id find(Range const & str)
  {
    return detail::css_property_name_to_id_dictionary::find_ignore_case(str);
  }
};

struct css_name_to_id_policy_case_sensitive
{
  template<class Range>
  static detail::attribute_id find(Range const & str)
  {
    return detail::css_property_name_to_id_dictionary::find(str);
  }
};

namespace detail
{
  template<class ErrorPolicy, class Context>
  class missing_attribute_visitor: boost::noncopyable
  {
  public:
    missing_attribute_visitor(Context & context)
      : context_(context)
    {
    }

    template<class AttributeTag>
    bool operator()(AttributeTag attribute) const
    {
      return ErrorPolicy::required_attribute_not_found(context_, attribute);
    }

  private:
    Context & context_;
  };
}

}