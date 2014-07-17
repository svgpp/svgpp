#pragma once

namespace svgpp { namespace policy { namespace document_traversal_control
{

struct stub
{
  template<class Context>
  static bool proceed_to_element_content(Context &)
  {
    return true;
  }

  template<class Context>
  static bool proceed_to_next_child(Context &)
  {
    return true;
  }
};

}}}
