#pragma once

#include <boost/noncopyable.hpp>
#include <boost/spirit/home/qi/parse.hpp>

namespace svgpp { namespace detail
{

template<class StringIterator, class ItemExpression, class SeparatorExpression, class Value>
class parse_list_iterator: boost::noncopyable
{
public:
  typedef Value result_type;

  parse_list_iterator(StringIterator const & first, StringIterator const & last,
    ItemExpression const & item_expression, SeparatorExpression const & separator_expression)
    : current_(first)
    , last_(last)
    , item_expression_(item_expression)
    , separator_expression_(separator_expression)
    , error_(false)
  {}

  bool get_next(Value & value)
  {
    if (current_ == last_)
      return false;
    else
    {
      if (!boost::spirit::qi::parse(current_, last_, item_expression_, value))
      {
        error_ = true;
        return false;
      }
      if (current_ != last_)
        if (!boost::spirit::qi::parse(current_, last_, separator_expression_))
        {
          error_ = true;
          return false;
        }
    }
    return true;
  }

  bool error() const
  {
    return error_;
  }

private:
  StringIterator current_;
  StringIterator const last_;
  ItemExpression const & item_expression_;
  SeparatorExpression const & separator_expression_;
  bool error_;
};

}}