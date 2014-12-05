// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/noncopyable.hpp>
#include <boost/pointee.hpp>

namespace svgpp { namespace factory { namespace context 
{

template<class ParentContext, class ElementTag>
class same: boost::noncopyable
{
public:
  typedef ParentContext type;

  template<class XMLElement>
  same(ParentContext & context, XMLElement const &)
    : context_(context)
  {
    context_.on_enter_element(ElementTag());
  }

  type & get() const { return context_; }

  void on_exit_element() const
  {
    context_.on_exit_element();
  }

private:
  type & context_;
};

template<class ChildContext>
class on_stack: boost::noncopyable
{
public:
  typedef ChildContext type;

  template<class ParentContext, class XMLElement>
  on_stack(ParentContext & context, XMLElement const &)
    : context_(context)
  {}

  type & get() { return context_; }

  void on_exit_element() 
  {
    context_.on_exit_element();
  }

private:
  type context_;
};

template<class ChildContext>
class on_stack_with_xml_element: boost::noncopyable
{
public:
  typedef ChildContext type;

  template<class ParentContext, class XMLElement>
  on_stack_with_xml_element(ParentContext & context, XMLElement const & xml_element)
    : context_(context, xml_element)
  {}

  type & get() { return context_; }

  void on_exit_element() 
  {
    context_.on_exit_element();
  }

private:
  type context_;
};

template<
  class ElementTag, 
  class ChildContextPtr, 
  class ChildContext = typename boost::pointee<ChildContextPtr>::type 
>
class get_ptr_from_parent: boost::noncopyable
{
public:
  typedef ChildContext type;

  template<class ParentContext, class XMLElement>
  get_ptr_from_parent(ParentContext & context, XMLElement const &)
    : context_(context.get_child_context(ElementTag()))
  {}

  type & get() const { return *context_; }

  void on_exit_element() const
  {
    context_->on_exit_element();
  }

private:
  ChildContextPtr context_;
};

}}}
