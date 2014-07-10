#pragma once

#include <cmath>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/mpl/if.hpp>
#include <boost/parameter.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <svgpp/policy/load_transform.hpp>
#include <svgpp/policy/transform.hpp>

namespace svgpp
{

BOOST_PARAMETER_TEMPLATE_KEYWORD(transform_policy)
BOOST_PARAMETER_TEMPLATE_KEYWORD(load_transform_policy)

template<class TransformPolicy>
struct need_transform_adapter: 
  boost::mpl::bool_<
       TransformPolicy::join_transforms 
    || TransformPolicy::no_rotate_about_point
    || TransformPolicy::no_shorthands 
    || TransformPolicy::only_matrix_transform>
{
};

namespace detail
{

template<class Context, class Number, class LoadPolicy>
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

  void append_transform_matrix(const boost::array<number_type, 6> & matrix)
  {
    LoadPolicy::append_transform_matrix(context, matrix);
  }

  void append_transform_translate(number_type tx, number_type ty)
  {
    LoadPolicy::append_transform_translate(context, tx, ty);
  }

  void append_transform_translate(number_type tx)
  {
    LoadPolicy::append_transform_translate(context, tx);
  }

  void append_transform_scale(number_type sx, number_type sy)
  {
    LoadPolicy::append_transform_scale(context, sx, sy);
  }

  void append_transform_scale(number_type scale)
  {
    LoadPolicy::append_transform_scale(context, scale);
  }

  void append_transform_rotate(number_type angle)
  {
    LoadPolicy::append_transform_rotate(context, angle);
  }

  void append_transform_rotate(number_type angle, number_type cx, number_type cy)
  {
    LoadPolicy::append_transform_rotate(context, angle, cx, cy);
  }

  void append_transform_skew_x(number_type angle)
  {
    LoadPolicy::append_transform_skew_x(context, angle);
  }

  void append_transform_skew_y(number_type angle)
  {
    LoadPolicy::append_transform_skew_y(context, angle);
  }

protected:
  Context & context;
};

template<class Context, class Number, class LoadPolicy>
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
    LoadPolicy::set_transform_matrix(context, matrix);
  }

  void append_transform_matrix(const boost::array<number_type, 6> &matrix2)
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

  void append_transform_translate(number_type tx, number_type ty)
  {
    const boost::array<number_type, 6> matrix = {{1, 0, 0, 1, tx, ty}};
    this->append_transform_matrix(matrix);
  }

  void append_transform_scale(number_type sx, number_type sy)
  {
    const boost::array<number_type, 6> matrix = {{sx, 0, 0, sy, 0, 0}};
    this->append_transform_matrix(matrix);
  }

  void append_transform_rotate(number_type angle)
  {
    BOOST_STATIC_ASSERT(boost::is_floating_point<number_type>::value);
    number_type cosa = std::cos(angle * boost::math::constants::degree<number_type>()), 
      sina = std::sin(angle * boost::math::constants::degree<number_type>());
    const boost::array<number_type, 6> matrix = {{cosa, sina, -sina, cosa, 0, 0}};
    this->append_transform_matrix(matrix);
  }

  void append_transform_skew_x(number_type angle)
  {
    BOOST_STATIC_ASSERT(boost::is_floating_point<number_type>::value);
    const boost::array<number_type, 6> matrix = 
      {{1, 0, std::tan(angle * boost::math::constants::degree<number_type>()), 1, 0, 0}};
    this->append_transform_matrix(matrix);
  }

  void append_transform_skew_y(number_type angle)
  {
    BOOST_STATIC_ASSERT(boost::is_floating_point<number_type>::value);
    const boost::array<number_type, 6> matrix = 
      {{1, std::tan(angle * boost::math::constants::degree<number_type>()), 0, 1, 0, 0}};
    this->append_transform_matrix(matrix);
  }
};

template<class Context, class TransformPolicy, class LoadPolicy>
struct transform_adapter_base
{
  typedef typename boost::mpl::if_c<
    TransformPolicy::join_transforms,
      matrix_only_transform_adapter<
        join_transform_adapter<Context, typename TransformPolicy::number_type, LoadPolicy> 
      >,
      typename boost::mpl::if_c<
        TransformPolicy::only_matrix_transform,
          matrix_only_transform_adapter<
            passthrough_transform_adapter<Context, typename TransformPolicy::number_type, LoadPolicy> 
          >,
          passthrough_transform_adapter<Context, typename TransformPolicy::number_type, LoadPolicy>
        >::type
      >::type type;
};

} // namespace detail

template<
  class Context, 
  class TransformPolicy = typename policy::transform::by_context<Context>::type,
  class LoadPolicy = policy::load_transform::default_policy<Context>,
  class Enable = void>
class transform_adapter;

template<
  class Context, 
  class TransformPolicy, 
  class LoadPolicy>
class transform_adapter<Context, TransformPolicy, LoadPolicy, 
  typename boost::enable_if_c<
       TransformPolicy::no_shorthands 
    || TransformPolicy::only_matrix_transform 
    || TransformPolicy::join_transforms>::type
>:
  public detail::transform_adapter_base<Context, TransformPolicy, LoadPolicy>::type
{
  typedef typename detail::transform_adapter_base<Context, TransformPolicy, LoadPolicy>::type base;

public:
  typedef typename TransformPolicy::number_type number_type;

  transform_adapter(Context & context)
    : base(context)
  {
  }

  using base::append_transform_translate;
  using base::append_transform_scale;
  using base::append_transform_rotate;

  void append_transform_translate(number_type tx)
  {
    append_transform_translate(tx, 0);
  }

  void append_transform_scale(number_type scale)
  {
    append_transform_scale(scale, scale);
  }

  void append_transform_rotate(number_type angle, number_type cx, number_type cy)
  {
    append_transform_translate(cx, cy);
    append_transform_rotate(angle);
    append_transform_translate(-cx, -cy);
  }
};

template<
  class Context, 
  class TransformPolicy, 
  class LoadPolicy>
class transform_adapter<Context, TransformPolicy, LoadPolicy, 
  typename boost::disable_if_c<
       TransformPolicy::no_shorthands 
    || TransformPolicy::only_matrix_transform 
    || TransformPolicy::join_transforms>::type
>:
  public detail::transform_adapter_base<Context, TransformPolicy, LoadPolicy>::type
{
  typedef typename detail::transform_adapter_base<Context, TransformPolicy, LoadPolicy>::type base;

public:
  transform_adapter(Context & context)
    : base(context)
  {
  }
};

namespace detail
{

template<
  class OutputContext, 
  class TransformPolicy, 
  class LoadTransformPolicy = policy::load_transform::default_policy<OutputContext>,
  class Enable = void>
struct transform_adapter_if_needed
{
  typedef OutputContext type;
  typedef type & holder_type;
  typedef LoadTransformPolicy load_transform_policy;

  void on_exit_attribute(type const &) {}

  static OutputContext & get_original_context(holder_type & adapted_context)
  {
    return adapted_context;
  }
};

template<
  class OutputContext, 
  class TransformPolicy, 
  class LoadTransformPolicy>
struct transform_adapter_if_needed<OutputContext, TransformPolicy, LoadTransformPolicy, 
  typename boost::enable_if<need_transform_adapter<TransformPolicy> >::type>
{
  typedef transform_adapter<OutputContext, TransformPolicy, LoadTransformPolicy> type;
  typedef type holder_type;
  typedef policy::load_transform::forward_to_method<type> load_transform_policy;

  static void on_exit_attribute(type & adapter) 
  {
    adapter.on_exit_attribute();
  }

  static OutputContext & get_original_context(holder_type & adapted_context)
  {
    return adapted_context.get_output_context();
  }
};

}

}