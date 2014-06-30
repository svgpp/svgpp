#include <svgpp/forward_calls_to_class.hpp>
#include <svgpp/value_parsers/spirit.hpp>

struct Context: boost::noncopyable
{
  typedef std::tuple<double, double, double, double> Rect;

  Context()
    : call_count_(0)
  {
  }

  void set_baseline_shift(double value)
  {
    ++call_count_;
    baseline_shift_ = value;
  }

  template<class SpecialValueTag>
  void set_baseline_shift(SpecialValueTag tag)
  {
    ++call_count_;
    baseline_shift_ = tag;
  }

  size_t call_count_;

  struct not_set {};
  boost::variant<not_set, double, 
    svgpp::baseline_shift_value_tags::baseline> baseline_shift_;
};

struct Context2: boost::noncopyable
{
  void set_baseline_shift(double);
  template<class SpecialValueTag>
  void set_baseline_shift(SpecialValueTag);
};

template<class Units> struct units_scale;

template<> struct units_scale<svgpp::length_units_cm_tag>
{
  inline static double val() { return 10.0; }
};

template<> struct units_scale<svgpp::length_units_in_tag>
{
  inline static double val() { return 25.4; }
};

namespace svgpp { 

template<>
struct forward_calls_to_class<Context2>
{
  static const bool value = true;
};
  
template<> struct value_type_traits<Context>
{
  typedef double length_type;
};

template<> struct value_type_traits<Context2>
{
  typedef double length_type;
};

template<class SpecialValueTag>
inline void set_baseline_shift(Context & value, SpecialValueTag tag)
{
  value.set_baseline_shift(tag);
}

inline void set_baseline_shift(Context & value, double val)
{
  value.set_baseline_shift(val);
}

template<class Units>
void set_length(double & length, double val, Units)
{
  length = val;// * units_scale<Units>::val();
}

void set_length(double & length, double val, length_units_em_tag)
{
  length = val;
}

inline Context::Rect context_element_start(Context & context, element_rect_tag)
{
  return Context::Rect();
}

int main()
{
  using namespace svgpp;
  using namespace svgpp::value_parser::spirit;
  const char str[] = "sub";
  Context2 context;
  parse(attribute_baseline_shift_tag(), context, str, str + strlen(str));
  return 0;
}