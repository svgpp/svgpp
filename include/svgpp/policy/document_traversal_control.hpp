// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

namespace svgpp { namespace policy { namespace document_traversal_control
{

template<class Context>
struct stub
{
  static bool proceed_to_element_content(Context &)
  { return true; }

  static bool proceed_to_next_child(Context &)
  { return true; }

  template<class XMLElement>
  static bool process_child(Context &, XMLElement &)
  { return true; }
};

template<class Context>
struct default_policy: stub<Context>
{};

}}}
