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