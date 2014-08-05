#pragma once

#include "common.hpp"
#include <boost/gil/typedefs.hpp>
#include <boost/shared_ptr.hpp>

class IFilterView
{
public:
  virtual ~IFilterView() {}

  virtual boost::gil::rgba8c_view_t view() = 0;
};

typedef boost::shared_ptr<IFilterView> IFilterViewPtr;

class Filters
{
public:
  Filters(XMLDocument & xml_document)
    : xml_document_(xml_document)
  {}

  struct Input
  {
    IFilterViewPtr sourceGraphic_;
    IFilterViewPtr backgroundImage_;
    IFilterViewPtr fillPaint_;
    IFilterViewPtr strokePaint_;
  };

  IFilterViewPtr get(
    svg_string_t const & id, 
    length_factory_t const &,
    Input const & input);

private:
  XMLDocument & xml_document_;
};