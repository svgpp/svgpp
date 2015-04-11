// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/noncopyable.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>

namespace svgpp
{

template<class IteratorT>
class css_style_iterator:
  public boost::iterator_facade< 
    css_style_iterator<IteratorT>,
    const std::pair<boost::iterator_range<IteratorT>, boost::iterator_range<IteratorT> >,
    boost::forward_traversal_tag 
  >,
  boost::noncopyable
{
public:
  css_style_iterator(IteratorT begin, IteratorT end)
    : it_(begin)
    , end_(end)
  {
    increment(); 
  }

  css_style_iterator(css_style_iterator const & src)
    : it_(src.it_)
    , end_(src.end_)
    , value_(src.value_)
  {
  }

  bool eof() const
  {
    return value_.first.begin() == end_;
  }

private:
  friend class boost::iterator_core_access;

  const typename css_style_iterator<IteratorT>::value_type & dereference() const
  {
    return value_;
  }

  void increment()
  {
    for(;;)
    {
      bool skip = false;
      IteratorT name_begin = end_, 
        name_end = end_; // not used, suppressing "uninitialized" warning
      for(; it_ != end_; ++it_)
      {
        switch (*it_)
        {
        case ' ': case '\r': case '\n': case '\t': 
          break;
        case ';':
          skip = true;
          break;
        case ':':
          if (name_begin == end_)
          {
            ++it_;
            skip = true;
          }
          else
          {
            IteratorT value_begin = end_, 
              value_end = end_; // not used, suppressing "uninitialized" warning
            for(++it_; it_ != end_ && !skip && *it_ != ';'; ++it_)
              switch (*it_)
              {
              case ' ': case '\r': case '\n': case '\t': 
                break;
              case ':': 
                skip = true;
                break;
              default:
                if (value_begin == end_)
                  value_begin = it_;
                value_end = it_;
              }
            if (!skip)
            {
              if (value_begin != end_)
              {
                if (it_ != end_)
                  ++it_;
                value_.first  = typename css_style_iterator<IteratorT>::value_type::first_type(name_begin, ++name_end);
                value_.second = typename css_style_iterator<IteratorT>::value_type::second_type(value_begin, ++value_end);
                return;
              }
              else
                skip = true;
            }
          }
          break;
        default:
          if (name_begin == end_)
             name_begin = it_;
          name_end = it_;
        }
        if (skip)
          break;
      }
      if (skip)
      {
        for(; it_ != end_ && *it_ != ';'; ++it_)
          ;
        if (it_ == end_ || ++it_ == end_)
          break;
      }
      else
        break;
    }
    value_.first  = typename css_style_iterator<IteratorT>::value_type::first_type(end_, end_);
  }

  bool equal(const css_style_iterator & other) const
  {
    return it_ == other.it_;
  }

  IteratorT it_;
  IteratorT const end_;
  typename css_style_iterator<IteratorT>::value_type value_;
};

}