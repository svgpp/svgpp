// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/noncopyable.hpp>
#include <boost/spirit/home/qi/parse.hpp>

namespace svgpp { namespace detail
{

template<
  class Value,
  class StringIterator, 
  class ItemExpression, 
  class SeparatorExpression, 
  class PrePostSkipper = boost::spirit::qi::unused_type
>
class parse_list_iterator: boost::noncopyable
{
public:
  typedef Value result_type;

  parse_list_iterator(StringIterator const & begin, StringIterator const & end,
    ItemExpression const & item_expression, 
    SeparatorExpression const & separator_expression,
    PrePostSkipper const & pre_post_skipper = PrePostSkipper())
    : current_(begin)
    , end_(end)
    , item_expression_(item_expression)
    , separator_expression_(separator_expression)
    , pre_post_skipper_(pre_post_skipper)
    , first_item_(true)
    , error_(false)
  {}

  bool get_next(Value & value)
  {
    typedef
        typename boost::spirit::result_of::compile<boost::spirit::qi::domain, PrePostSkipper>::type
    skipper_type;
    skipper_type const skipper = boost::spirit::compile<boost::spirit::qi::domain>(pre_post_skipper_);

    if (first_item_)
    {
      first_item_ = false;
      boost::spirit::qi::skip_over(current_, end_, skipper);
      if (current_ == end_)
        return false;
      if (boost::spirit::qi::parse(current_, end_, item_expression_, value))
        return true;
      else
      {
        error_ = true;
        return false;
      }
    }
    else
    {
      StringIterator cur = current_;
      if (boost::spirit::qi::parse(current_, end_, separator_expression_)
        && boost::spirit::qi::parse(current_, end_, item_expression_, value))
        return true;
      else
      {
        current_ = cur;
        boost::spirit::qi::skip_over(current_, end_, skipper);
        if (current_ != end_)
          error_ = true;

        return false;
      }
    }
  }

  bool error() const
  {
    return error_;
  }

private:
  StringIterator current_;
  StringIterator const end_;
  ItemExpression const & item_expression_;
  SeparatorExpression const & separator_expression_;
  PrePostSkipper const & pre_post_skipper_;
  bool first_item_;
  bool error_;
};

}}