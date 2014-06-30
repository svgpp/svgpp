#include <boost/parameter.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/transform_view.hpp>
#include <boost/preprocessor.hpp>
#include <boost/variant.hpp>
#include <string>

#define PARAMS_NUM 7

BOOST_PARAMETER_TEMPLATE_KEYWORD(a1)
BOOST_PARAMETER_TEMPLATE_KEYWORD(a2)
BOOST_PARAMETER_TEMPLATE_KEYWORD(a3)
BOOST_PARAMETER_TEMPLATE_KEYWORD(a4)
BOOST_PARAMETER_TEMPLATE_KEYWORD(a5)

template<
  BOOST_PP_ENUM_BINARY_PARAMS(PARAMS_NUM, class A, = boost::parameter::void_ BOOST_PP_INTERCEPT)>
struct forwarded
{
  typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::a4>
    , boost::parameter::optional<tag::a5>
  >::bind<BOOST_PP_ENUM_PARAMS(PARAMS_NUM, A)>::type args;
  typedef typename boost::parameter::value_type<args, tag::a4, 
    void >::type a4_type;
  typedef typename boost::parameter::value_type<args, tag::a5, 
    void >::type a5_type;
};

template<
  BOOST_PP_ENUM_BINARY_PARAMS(PARAMS_NUM, class A, = boost::parameter::void_ BOOST_PP_INTERCEPT)>
struct test
{
  typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::a1>
    , boost::parameter::optional<tag::a2>
    , boost::parameter::optional<tag::a3 > 
  >::bind<BOOST_PP_ENUM_PARAMS(PARAMS_NUM, A)>::type args;
  typedef typename boost::parameter::value_type<args, tag::a1, 
    void >::type a1_type;
  typedef typename boost::parameter::value_type<args, tag::a2, 
    void >::type a2_type;
  typedef typename boost::parameter::value_type<args, tag::a3, 
    void >::type a3_type;

  typedef forwarded<BOOST_PP_ENUM_PARAMS(PARAMS_NUM, A)> inner;
};

typedef test<a3<bool>, a4<char>, a1<long>, a2<int>, a5<std::string> > test_impl;
BOOST_MPL_ASSERT((boost::is_same<test_impl::a1_type, long>));
BOOST_MPL_ASSERT((boost::is_same<test_impl::a2_type, int>));
BOOST_MPL_ASSERT((boost::is_same<test_impl::a3_type, bool>));
BOOST_MPL_ASSERT((boost::is_same<test_impl::inner::a4_type, char>));
BOOST_MPL_ASSERT((boost::is_same<test_impl::inner::a5_type, std::string>));

namespace NS1
{
  struct not_set;

  template<class T1>
  struct context_policies
  {
    typedef T1 t1;
  };

  template<class T1, class T2>
  struct s1
  {
    typedef typename boost::mpl::if_<
      boost::is_same<T1, not_set>,
      boost::mpl::quote1<context_policies>,
      boost::mpl::always<T1>
    >::type get_t1;

    typedef typename get_t1::template apply<T2>::type t1;
  };

  BOOST_MPL_ASSERT((boost::is_same<s1<not_set, bool>::t1, context_policies<bool> >));
  BOOST_MPL_ASSERT((boost::is_same<s1<double, bool>::t1, double> ));
}

namespace NS2
{
  struct a;
  struct b;
  struct c;

  typedef boost::mpl::vector<a> processed_elements;
  typedef boost::mpl::vector<> ignored_elements;

  typedef 
    boost::mpl::if_<
      boost::mpl::not_<boost::mpl::empty<processed_elements> >,
      boost::mpl::contains<processed_elements, boost::mpl::_1>,
      boost::mpl::not_<boost::mpl::contains<ignored_elements, boost::mpl::_1> >  
    >::type is_element_processed;

  static_assert(boost::mpl::apply<is_element_processed, a>::type::value, "");
  static_assert(!boost::mpl::apply<is_element_processed, b>::type::value, "");
  static_assert(!boost::mpl::apply<is_element_processed, c>::type::value, "");
}

namespace NS2_A
{
  struct a;
  struct b;
  struct c;

  typedef boost::mpl::vector<> processed_elements;
  typedef boost::mpl::vector<a> ignored_elements;

  typedef 
    boost::mpl::if_<
    boost::mpl::not_<boost::mpl::empty<processed_elements> >,
    boost::mpl::contains<processed_elements, boost::mpl::_1>,
    boost::mpl::not_<boost::mpl::contains<ignored_elements, boost::mpl::_1> >  
    >::type is_element_processed;

  static_assert(!boost::mpl::apply<is_element_processed, a>::type::value, "");
  static_assert(boost::mpl::apply<is_element_processed, b>::type::value, "");
  static_assert(boost::mpl::apply<is_element_processed, c>::type::value, "");
}

namespace NS3
{
  template <class U> ::boost::type_traits::yes_type requires_length_factory_tester(typename U::require_length_factory *);
  template <class U> ::boost::type_traits::no_type requires_length_factory_tester(...);


  template<class ValueParser>
  struct parser_requires_length_factory: 
    boost::mpl::bool_<sizeof(requires_length_factory_tester<ValueParser>(0)) == sizeof(::boost::type_traits::yes_type)> {};

  struct AA
  {
    struct require_length_factory;
  };

  struct BB
  {
  };


  static_assert(parser_requires_length_factory<AA>::value == true, "2");
  static_assert(parser_requires_length_factory<BB>::value == false, "3");
}

namespace NS4
{
  struct S1 {};
  struct S2 {};
  struct S3 {};
  struct A {};

  typedef boost::mpl::vector<S1, S2, S3> vals;
  typedef boost::mpl::transform_view<
    vals, boost::mpl::pair<A, boost::mpl::_1> > rect_attrs;

  static_assert(boost::mpl::contains<rect_attrs, boost::mpl::pair<A, S1> >::value, "");
  static_assert(!boost::mpl::contains<rect_attrs, boost::mpl::pair<A, A> >::value, "");
  static_assert(!boost::mpl::contains<rect_attrs, A>::value, "");
  static_assert(!boost::mpl::contains<rect_attrs, S1>::value, "");
}

namespace NS5
{

  size_t const sz = sizeof("abcde");
  static_assert(sz == 6, "!");
}

