#pragma once

#include <svgpp/factory/length.hpp>

namespace svgpp { namespace policy { namespace length
{

template<class Context, class LengthFactory = const typename Context::length_factory>
struct forward_to_method
{
  typedef Context context_type;
  typedef LengthFactory length_factory_type;

  static length_factory_type & length_factory(Context & context)
  {
    return context.length_factory();
  }
};

namespace
{
  factory::length::default_factory const default_factory_instance;
}

template<class Context>
struct default_policy
{
  typedef Context context_type;
  typedef factory::length::default_factory const length_factory_type;

  static length_factory_type & length_factory(Context const &)
  {
    return default_factory_instance;
  }
};

}}}