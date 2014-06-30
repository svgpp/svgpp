#include <svgpp/parser/spirit.hpp>
#include <svgpp/parser/misc.hpp>
#include <svgpp/parser/simple_enumeration.hpp>
#include <svgpp/detail/attribute_id_to_tag.hpp>
#include <svgpp/traits/attribute_without_parser.hpp>
#include <svgpp/traits/attribute_type.hpp>
#include <boost/type_traits/detail/yes_no_type.hpp>
#include <boost/tti/has_static_member_function.hpp>
#include <gtest/gtest.h>


BOOST_TTI_HAS_STATIC_MEMBER_FUNCTION(parse)

namespace
{
  template<class ValueType>
  boost::type_traits::yes_type value_parser_exists(svgpp::value_parser<ValueType> *);

  template<class ValueType>
  boost::type_traits::no_type value_parser_exists(...);

  template<class ElementTag>
  struct check_attribute
  {
    template<class AttributeTag>
    void operator()(AttributeTag, boost::enable_if<svgpp::traits::attribute_without_parser<AttributeTag> >::type * = NULL)
    {
    }

    template<class AttributeTag>
    void operator()(AttributeTag, boost::disable_if<svgpp::traits::attribute_without_parser<AttributeTag> >::type * = NULL)
    {
      typedef svgpp::traits::attribute_type<ElementTag, AttributeTag>::type value_type;

      //SCOPED_TRACE(typeid(value_type).name());
      EXPECT_TRUE((has_static_member_function_parse<value_type, bool>::value)) << typeid(value_type).name();
    }
  };

  template<class ElementTag>
  void check_element()
  {
    check_attribute<ElementTag> check;
    for(int id = 0; id < svgpp::detail::attribute_count; ++id)
      svgpp::detail::attribute_id_to_tag(ElementTag(),
        static_cast<svgpp::detail::attribute_id>(id), check);
  }
}

TEST(value_parser, supported_types)
{
#define SVGPP_ON(element_name, name) check_element<svgpp::tag::element::element_name>();
#include <svgpp/detail/enumerate_all_elements.inc>
}