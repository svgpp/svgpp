// Copyright David Abrahams 2005. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef EXBOOST_PARAMETER_AUX_RESULT_OF0_DWA2005511_HPP_EXBOOST
# define EXBOOST_PARAMETER_AUX_RESULT_OF0_DWA2005511_HPP_EXBOOST

# include <boost/utility/result_of.hpp>

// A metafunction returning the result of invoking a nullary function
// object of the given type.

#ifndef BOOST_NO_RESULT_OF

# include <boost/utility/result_of.hpp>
namespace exboost { using namespace boost; namespace parameter { namespace aux {
template <class F>
struct result_of0 : result_of<F()>
{};

}}} // namespace exboost::parameter::aux_

#else

namespace exboost { using namespace boost; namespace parameter { namespace aux {
template <class F>
struct result_of0
{
    typedef typename F::result_type type;
};

}}} // namespace exboost::parameter::aux_

#endif


#endif // EXBOOST_PARAMETER_AUX_RESULT_OF0_DWA2005511_HPP_EXBOOST
