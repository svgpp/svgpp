#include <svgpp/parser/transform_list.hpp>

// Only compilation is checked

using namespace svgpp;

namespace
{
  struct TransformContext1
  {
    void transform_matrix(const boost::array<double, 6> & matrix)
    {
    }
  };

  struct TransformContext2
  {
    void transform_matrix(const boost::array<double, 6> & matrix)
    {
    }

    void transform_translate(double tx, double ty)
    {
    }

    void transform_translate(double tx)
    {
    }

    void transform_scale(double sx, double sy)
    {
    }

    void transform_scale(double scale)
    {
    }

    void transform_rotate(double angle)
    {
    }

    void transform_rotate(double angle, double cx, double cy)
    {
    }

    void transform_skew_x(double angle)
    {
    }

    void transform_skew_y(double angle)
    {
    }
  };
}

void CheckTransform()
{
  std::string testStr;

  {
    TransformContext1 ctx;
    value_parser<tag::type::transform_list>::parse(
      tag::attribute::transform(), ctx, testStr, tag::source::attribute());
  }

  {
    TransformContext2 ctx;
    value_parser<
      tag::type::transform_list,
      transform_policy<policy::transform::raw>
    >::parse(
      tag::attribute::transform(), ctx, testStr, tag::source::attribute());
  }
}