// Copyright Daniel Wallin, David Abrahams 2005. Use, modification and
// distribution is subject to the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef EXBOOST_PARAMETER_VOID_050329_HPP_EXBOOST
#define EXBOOST_PARAMETER_VOID_050329_HPP_EXBOOST

namespace exboost { using namespace boost; namespace parameter {

// A placemarker for "no argument passed."
// MAINTAINER NOTE: Do not make this into a metafunction
struct void_ {};

namespace aux
{

  inline void_& void_reference()
  {
      static void_ instance;
      return instance;
  }

} // namespace aux

}} // namespace exboost::parameter

#endif // EXBOOST_PARAMETER_VOID_050329_HPP_EXBOOST

