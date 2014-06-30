#pragma once

namespace svgpp { namespace detail 
{

template<class OriginalErrorPolicy, class GetOriginalContextFunc>
struct delegate_error_policy
{
  template<class Context, class AttributeTag>
  static bool negative_value(Context & context, AttributeTag tag)
  {
    return OriginalErrorPolicy::negative_value(GetOriginalContextFunc::get_original_context(context), tag);
  }
};

}}