// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <cmath>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/mpl/if.hpp>
#include <boost/parameter.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <svgpp/detail/adapt_context.hpp>
#include <svgpp/policy/transform_events.hpp>
#include <svgpp/policy/transform.hpp>

namespace svgpp
{

namespace detail
{

template<class Context, class Number, class EventsPolicy>
class passthrough_transform_adapter
{
public:
  typedef Number number_type;

  passthrough_transform_adapter(Context & context)
    : context(context)
  {
  }

  Context & get_output_context() const { return context; }

  void on_exit_attribute() const
  {
  }

  void transform_matrix(const boost::array<number_type, 6> & matrix)
  {
    EventsPolicy::transform_matrix(context, matrix);
  }

  void transform_translate(number_type tx, number_type ty)
  {
    EventsPolicy::transform_translate(context, tx, ty);
  }

  void transform_translate(number_type tx)
  {
    EventsPolicy::transform_translate(context, tx);
  }

  void transform_scale(number_type sx, number_type sy)
  {
    EventsPolicy::transform_scale(context, sx, sy);
  }

  void transform_scale(number_type scale)
  {
    EventsPolicy::transform_scale(context, scale);
  }

  void transform_rotate(number_type angle)
  {
    EventsPolicy::transform_rotate(context, angle);
  }

  void transform_rotate(number_type angle, number_type cx, number_type cy)
  {
    EventsPolicy::transform_rotate(context, angle, cx, cy);
  }

  void transform_skew_x(number_type angle)
  {
    EventsPolicy::transform_skew_x(context, angle);
  }

  void transform_skew_y(number_type angle)
  {
    EventsPolicy::transform_skew_y(context, angle);
  }

protected:
  Context & context;
};

template<class Context, class Number, class EventsPolicy>
class join_transform_adapter: boost::noncopyable
{
public:
  typedef Number number_type;

  join_transform_adapter(Context & context)
    : context(context)
  {
    matrix[0] = 1; matrix[2] = 0; matrix[4] = 0;
    matrix[1] = 0; matrix[3] = 1; matrix[5] = 0;
  }

  Context & get_output_context() const { return context; }

  void on_exit_attribute() const
  {
    EventsPolicy::transform_matrix(context, matrix);
  }

  void transform_matrix(const boost::array<number_type, 6> &matrix2)
  {
    boost::array<number_type, 6> new_matrix;
    for(size_t i = 0; i < 2; ++i)
    {
      for(size_t j = 0; j < 3; ++j)
        new_matrix[i + j * 2] = matrix[i] * matrix2[j * 2] + matrix[i + 2] * matrix2[j * 2 + 1];
      new_matrix[i + 4] += matrix[i + 4];
    }
    matrix = new_matrix;
  }

protected:
  Context & context;

private:
  boost::array<number_type, 6> matrix;
};

template<class Base>
class matrix_only_transform_adapter: public Base
{
public:
  typedef typename Base::number_type number_type;

  template<class Context>
  matrix_only_transform_adapter(Context & context)
    : Base(context)
  {
  }

  void transform_translate(number_type tx, number_type ty)
  {
    const boost::array<number_type, 6> matrix = {{1, 0, 0, 1, tx, ty}};
    this->transform_matrix(matrix);
  }

  void transform_scale(number_type sx, number_type sy)
  {
    const boost::array<number_type, 6> matrix = {{sx, 0, 0, sy, 0, 0}};
    this->transform_matrix(matrix);
  }

  void transform_rotate(number_type angle)
  {
    BOOST_STATIC_ASSERT(boost::is_floating_point<number_type>::value);
    number_type cosa = std::cos(angle * boost::math::constants::degree<number_type>()), 
      sina = std::sin(angle * boost::math::constants::degree<number_type>());
    const boost::array<number_type, 6> matrix = {{cosa, sina, -sina, cosa, 0, 0}};
    this->transform_matrix(matrix);
  }

  void transform_skew_x(number_type angle)
  {
    BOOST_STATIC_ASSERT(boost::is_floating_point<number_type>::value);
    const boost::array<number_type, 6> matrix = 
      {{1, 0, std::tan(angle * boost::math::constants::degree<number_type>()), 1, 0, 0}};
    this->transform_matrix(matrix);
  }

  void transform_skew_y(number_type angle)
  {
    BOOST_STATIC_ASSERT(boost::is_floating_point<number_type>::value);
    const boost::array<number_type, 6> matrix = 
      {{1, std::tan(angle * boost::math::constants::degree<number_type>()), 0, 1, 0, 0}};
    this->transform_matrix(matrix);
  }
};

template<class Context, class TransformPolicy, class EventsPolicy, class Number>
struct transform_adapter_base
{
  typedef typename boost::mpl::if_c<
    TransformPolicy::join_transforms,
      matrix_only_transform_adapter<
        join_transform_adapter<Context, Number, EventsPolicy> 
      >,
      typename boost::mpl::if_c<
        TransformPolicy::only_matrix_transform,
          matrix_only_transform_adapter<
            passthrough_transform_adapter<Context, Number, EventsPolicy> 
          >,
          passthrough_transform_adapter<Context, Number, EventsPolicy>
        >::type
      >::type type;
};

} // namespace detail

template<
  class Context, 
  class TransformPolicy = typename policy::transform::by_context<Context>::type,
  class EventsPolicy = policy::transform_events::default_policy<Context>,
  class Number = double,
  class Enable = void>
class transform_adapter;

template<
  class Context, 
  class TransformPolicy, 
  class EventsPolicy,
  class Number
>
class transform_adapter<Context, TransformPolicy, EventsPolicy, Number,
  typename boost::enable_if_c<
       TransformPolicy::no_shorthands 
    || TransformPolicy::only_matrix_transform 
    || TransformPolicy::join_transforms>::type
>:
  public detail::transform_adapter_base<Context, TransformPolicy, EventsPolicy, Number>::type
{
  typedef typename detail::transform_adapter_base<Context, TransformPolicy, EventsPolicy, Number>::type base;

public:
  typedef Number number_type;

  transform_adapter(Context & context)
    : base(context)
  {}

  using base::transform_translate;
  using base::transform_scale;
  using base::transform_rotate;

  void transform_translate(number_type tx)
  {
    transform_translate(tx, 0);
  }

  void transform_scale(number_type scale)
  {
    transform_scale(scale, scale);
  }

  void transform_rotate(number_type angle, number_type cx, number_type cy)
  {
    transform_translate(cx, cy);
    transform_rotate(angle);
    transform_translate(-cx, -cy);
  }
};

template<
  class Context, 
  class TransformPolicy, 
  class EventsPolicy,
  class Number
>
class transform_adapter<Context, TransformPolicy, EventsPolicy, Number,
  typename boost::disable_if_c<
       TransformPolicy::no_shorthands 
    || TransformPolicy::only_matrix_transform 
    || TransformPolicy::join_transforms>::type
>:
  public detail::transform_adapter_base<Context, TransformPolicy, EventsPolicy, Number>::type
{
  typedef typename detail::transform_adapter_base<Context, TransformPolicy, EventsPolicy, Number>::type base;

public:
  transform_adapter(Context & context)
    : base(context)
  {}
};

namespace detail
{

template<class TransformPolicy>
struct need_transform_adapter: 
  boost::mpl::bool_<
       TransformPolicy::join_transforms 
    || TransformPolicy::no_rotate_about_point
    || TransformPolicy::no_shorthands 
    || TransformPolicy::only_matrix_transform>
{};

template<class OriginalContext, class Enable = void>
struct transform_adapter_if_needed
{
private:
  struct adapter_stub
  {
    template<class Context> adapter_stub(Context const &) {}
  };

public:
  typedef adapter_stub type;
  typedef OriginalContext adapted_context;
  typedef OriginalContext & adapted_context_holder;

  static OriginalContext & adapt_context(OriginalContext & context, adapter_stub &)
  {
    return context;
  }

  static void on_exit_attribute(type const &) {}
};

template<class OriginalContext>
struct transform_adapter_if_needed<OriginalContext, 
  typename boost::enable_if<need_transform_adapter<typename detail::unwrap_context<OriginalContext, tag::transform_policy>::policy> >::type>
{
private:
  typedef typename detail::unwrap_context<OriginalContext, tag::transform_policy>::policy transform_policy;
  typedef typename detail::unwrap_context<OriginalContext, tag::transform_events_policy>::policy original_transform_events_policy;
  typedef typename detail::unwrap_context<OriginalContext, tag::number_type>::policy number_type;

public:
  typedef transform_adapter<
    typename original_transform_events_policy::context_type, 
    transform_policy, 
    original_transform_events_policy,
    number_type
  > type;

  typedef const adapted_context_wrapper<
    OriginalContext, 
    type, 
    tag::transform_events_policy, 
    policy::transform_events::forward_to_method<type>
  > adapted_context;
  typedef adapted_context adapted_context_holder;

  static adapted_context adapt_context(OriginalContext & context, type & adapter)
  {
    return adapted_context(context, adapter);
  }

  static void on_exit_attribute(type & adapter) 
  {
    adapter.on_exit_attribute();
  }
};

} // namespace detail

}