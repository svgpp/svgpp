// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

namespace svgpp { namespace policy { namespace notify 
{

template<class Context>
struct forward_to_method
{
  template<class EventTag>
  static bool notify(Context & context, EventTag tag)
  {
    return context.notify(tag);
  }
};

template<class Context>
struct default_policy: forward_to_method<Context>
{};

}}}