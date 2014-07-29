// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/assert.hpp>
#include <boost/iterator/iterator_facade.hpp>

namespace svgpp { namespace detail 
{

template <class UnderlyingIterator>
class finite_function_iterator
: public boost::iterator_facade<
    finite_function_iterator<UnderlyingIterator>,
    typename UnderlyingIterator::result_type,
    boost::single_pass_traversal_tag,
    typename UnderlyingIterator::result_type const &
  >
{
  typedef finite_function_iterator<UnderlyingIterator> this_type;
public:
  finite_function_iterator()
    : f_(NULL)
  {}

  finite_function_iterator(UnderlyingIterator & f) 
    : f_(&f)
  {
    increment();
  }

  void increment() 
  {
    if (!f_->get_next(value_))
      f_ = NULL;
  }

  typename this_type::reference & dereference() const 
  {
    return value_;
  }

  bool equal(finite_function_iterator const & other) const 
  {
    // Only comparison with end iterator is supported
    BOOST_ASSERT(f_ == NULL || other.f_ == NULL);
    return f_ == other.f_;
  }

private:
  UnderlyingIterator * f_; // f_ == NULL means end iterator
  typename this_type::value_type value_;
};

}}
