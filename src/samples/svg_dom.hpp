#pragma once

#include <memory>
#include <vector>
#include <map>
#include <boost/noncopyable.hpp>

namespace svgpp { namespace dom {

class SVGLength 
{
public:
  // Length Unit Types
  static const unsigned short SVG_LENGTHTYPE_UNKNOWN = 0;
  static const unsigned short SVG_LENGTHTYPE_NUMBER = 1;
  static const unsigned short SVG_LENGTHTYPE_PERCENTAGE = 2;
  static const unsigned short SVG_LENGTHTYPE_EMS = 3;
  static const unsigned short SVG_LENGTHTYPE_EXS = 4;
  static const unsigned short SVG_LENGTHTYPE_PX = 5;
  static const unsigned short SVG_LENGTHTYPE_CM = 6;
  static const unsigned short SVG_LENGTHTYPE_MM = 7;
  static const unsigned short SVG_LENGTHTYPE_IN = 8;
  static const unsigned short SVG_LENGTHTYPE_PT = 9;
  static const unsigned short SVG_LENGTHTYPE_PC = 10;

  double value() const { return value_; }
  unsigned short unitType() const { return unitType_; }

  void setUnitType(unsigned short t) { unitType_ = t; }
  void setValue(double val) { value_ = val; }

private:
  unsigned short unitType_;
  double value_;
};

template<class Ch>
class SVGElement: boost::noncopyable
{
public:
  typedef std::vector<std::shared_ptr<SVGElement const> > Children;
  typedef std::basic_string<Ch> DOMString;

  SVGElement(SVGElement const * parent)
    : parent_(parent)
  {
  }

  virtual ~SVGElement() {}

  void appendChild(std::shared_ptr<SVGElement> const & child)
  {
    children_.push_back(child);
  }

  Children const & children() const { return children_; }

  template<class ElementTag>
  void on_enter_element(ElementTag) {} // TODO: remove
  void on_exit_element() {} // TODO: remove

private:
  SVGElement const * parent_;
  Children children_;
};

template<class Ch>
class SVGStylable
{
public:
  void set_baseline_shift(SVGLength const & value) {}
  template<class SpecialValueTag>
  void set_baseline_shift(SpecialValueTag tag) {}

  void set_property(Ch const * name, Ch const * value)
  {
    properties_[name] = value;
  }

private:
  std::map<std::basic_string<Ch>, std::basic_string<Ch> > properties_;
};

template<class Ch>
class SVGSVGElement:
  public SVGElement<Ch>,
  public SVGStylable<Ch>
{
public:
  SVGSVGElement(SVGElement const * parent)
    : SVGElement(parent)
  {
  }

  DOMString contentScriptType() const { return contentScriptType_; }
  DOMString contentStyleType()  const { return contentStyleType_; }

  template<class Value>
  void set(svgpp::contentScriptType_attribute_tag, Value const & val)
  {
    contentScriptType_.assign(boost::begin(val), boost::end(val));
  }

  template<class Value>
  void set(svgpp::contentStyleType_attribute_tag, Value const & val)
  {
    contentStyleType_.assign(boost::begin(val), boost::end(val));
  }

private:
  DOMString contentScriptType_;
  DOMString contentStyleType_;
};

template<class Ch>
class SVGRectElement:
  public SVGElement<Ch>,
  public SVGStylable<Ch>
{
public:
  SVGRectElement(SVGElement const & parent)
    : SVGElement(&parent)
  {
  }

  SVGLength const & x() const { return x_; }
  SVGLength const & y() const { return y_; }

  void set_x(SVGLength const & val) { x_ = val; }
  void set_y(SVGLength const & val) { y_ = val; }

private:
  SVGLength x_, y_;
};

}}