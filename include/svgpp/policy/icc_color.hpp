#pragma once

#include <svgpp/factory/icc_color_stub.hpp>

namespace svgpp { namespace policy { namespace icc_color
{

template<class Context, class ICCColorFactory = const typename Context::icc_color_factory>
struct forward_to_method
{
  typedef Context context_type;
  typedef ICCColorFactory icc_color_factory_type;

  static icc_color_factory_type & icc_color_factory(Context & context)
  {
    return context.icc_color_factory();
  }
};

namespace
{
  factory::icc_color::stub const default_factory_instance;
}

template<class Context>
struct default_policy
{
  typedef Context context_type;
  typedef factory::icc_color::stub const icc_color_factory_type;

  static icc_color_factory_type & icc_color_factory(Context const &)
  {
    return default_factory_instance;
  }
};

}}}