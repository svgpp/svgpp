// Copyright Daniel Wallin 2006. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EXBOOST_PARAMETER_SET_060912_HPP_EXBOOST
# define EXBOOST_PARAMETER_SET_060912_HPP_EXBOOST

# include <boost/detail/workaround.hpp>

# if !BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x564))
#  include <boost/mpl/insert.hpp>
#  include <boost/mpl/set/set0.hpp>
#  include <boost/mpl/has_key.hpp>

namespace exboost { using namespace boost; namespace parameter { namespace aux {

typedef mpl::set0<> set0;

template <class Set, class K>
struct insert_
{
    typedef typename mpl::insert<Set, K>::type type;
};

template <class Set, class K>
struct has_key_
{
    typedef typename mpl::has_key<Set, K>::type type;
};

}}} // namespace exboost::parameter::aux

# else

#  include <boost/mpl/list.hpp>
#  include <boost/mpl/end.hpp>
#  include <boost/mpl/find.hpp>
#  include <boost/mpl/not.hpp>
#  include <boost/mpl/push_front.hpp>

namespace exboost { using namespace boost; namespace parameter { namespace aux {

typedef mpl::list0<> set0;

template <class Set, class K>
struct insert_
{
    typedef typename mpl::push_front<Set, K>::type type;
};

template <class Set, class K>
struct has_key_
{
    typedef typename mpl::find<Set, K>::type iter;
    typedef mpl::not_<
        is_same<iter, typename mpl::end<Set>::type>
    > type;
};

}}} // namespace exboost::parameter::aux

# endif


#endif // EXBOOST_PARAMETER_SET_060912_HPP_EXBOOST

