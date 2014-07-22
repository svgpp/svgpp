// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

namespace svgpp { namespace policy { namespace iri
{

struct distinguish_local
{
  static const bool distinguish_local_references = true;
};

struct raw
{
  static const bool distinguish_local_references = false;
};

typedef distinguish_local default_policy;

template<class Context>
struct by_context
{
  typedef default_policy type;
};

}}}