#include <svgpp/adapter/path.hpp>
#include <svgpp/adapter/path_markers.hpp>
#include <svgpp/parser/path_data.hpp>

#include <gtest/gtest.h>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/tuple/tuple_io.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/optional/optional_io.hpp>

inline bool operator==(boost::optional<double> const & lhs, boost::optional<double> const & rhs)
{
  if (!lhs && !rhs)
    return true;
  if (!lhs || !rhs)
    return false;
  return std::fabs(*lhs - *rhs) < 1e-8;
}

namespace 
{
  double const deg = boost::math::constants::degree<double>();

  typedef boost::tuple<svgpp::marker_vertex, double, double, boost::optional<double> > MarkerInstance;
  typedef std::vector<MarkerInstance> MarkerSequence;  

  std::ostream & operator<< (std::ostream & os, MarkerInstance const & val)
  {
    return boost::tuples::operator<<(os, val);
  }

  struct Context
  {
    Context()
      : added_count_(0)
    {}

    void marker(svgpp::marker_vertex v, 
      double x, double y, double directionality, unsigned marker_index)
    {
      add(MarkerInstance(v, x, y, directionality), marker_index);
    }

    void marker(svgpp::marker_vertex v, 
      double x, double y, svgpp::tag::orient_fixed, unsigned marker_index)
    {
      add(MarkerInstance(v, x, y, boost::optional<double>()), marker_index);
    }

    void marker_get_config(svgpp::marker_config & start, svgpp::marker_config & mid, svgpp::marker_config & end) const
    {
      start = config_start_;
      mid = config_mid_;
      end = config_end_;
    }

    svgpp::marker_config config_start_, config_mid_, config_end_;

    MarkerSequence const & log() const 
    { 
      EXPECT_EQ(log_.size(), added_count_);
      return log_; 
    }

  private:
    void add(MarkerInstance const & m, unsigned marker_index)
    {
      ASSERT_LE(marker_index, log_.size() + 1);

      if (marker_index > log_.size())
      {
        log_.push_back(MarkerInstance());
      }
      if (marker_index != log_.size())
      {
        log_[marker_index] = m;
      }
      else
        log_.push_back(m);
      ++added_count_;
    }

    int added_count_;
    MarkerSequence log_;
  };

  void DoTest(Context & context, std::string const & path_string, MarkerSequence const & expected_markers)
  {
    typedef svgpp::path_markers_adapter<Context> markers_adapter_t;
    markers_adapter_t markers_adapter(context);
    //svgpp::path_adapter<markers_adapter_t, svgpp::path_policies_no_shorthands> path_adapter(markers_adapter);
    svgpp::value_parser<svgpp::tag::type::path_data>::parse(
      svgpp::tag::attribute::d(), markers_adapter, //path_adapter,
      path_string, svgpp::tag::source::attribute());
    EXPECT_EQ(expected_markers, context.log());
  }

  void DoConfigTests(std::string const & path_string, MarkerSequence const & expected_markers)
  {
    svgpp::marker_config c[3];
    for(c[0] = svgpp::marker_none; c[0] <= svgpp::marker_orient_auto; c[0] = svgpp::marker_config(int(c[0]) + 1))
      for(c[1] = svgpp::marker_none; c[1] <= svgpp::marker_orient_auto; c[1] = svgpp::marker_config(int(c[1]) + 1))
        for(c[2] = svgpp::marker_none; c[2] <= svgpp::marker_orient_auto; c[2] = svgpp::marker_config(int(c[2]) + 1))
        {
          Context context;
          context.config_start_ = c[0];
          context.config_mid_ = c[1];
          context.config_end_ = c[2];

          MarkerSequence filtered_markers;
          for(MarkerSequence::const_iterator m = expected_markers.begin();
            m != expected_markers.end(); ++m)
          {
            int idx;
            switch (m->get<0>())
            {
            case svgpp::marker_start: idx = 0; break;
            case svgpp::marker_mid: idx = 1; break;
            case svgpp::marker_end: idx = 2; break;
            default:
              ADD_FAILURE();
            }
            switch (c[idx])
            {
            case svgpp::marker_none: break;
            case svgpp::marker_orient_fixed:
            {
              MarkerInstance m2 = *m;
              m2.get<3>().reset();
              filtered_markers.push_back(m2);
              break;
            }
            case svgpp::marker_orient_auto:
              filtered_markers.push_back(*m);
              break;
            default:
              ADD_FAILURE();
            }
          }

          DoTest(context, path_string, filtered_markers);
        }
  }
}

using namespace boost::assign;

TEST(path_markers_adapter, test1)
{
  DoConfigTests("M0 0 L 10 10", list_of
    (MarkerInstance(svgpp::marker_start, 0, 0, 45.0 * deg))
    (MarkerInstance(svgpp::marker_end, 10, 10, 45.0 * deg))
    );
}

TEST(path_markers_adapter, test2)
{
  DoConfigTests("M0 0 h 10 v 10 h -10 z " "M20 20 h 10 v 10 h -10 z", list_of
    (MarkerInstance(svgpp::marker_start, 0, 0, -45.0 * deg))
    (MarkerInstance(svgpp::marker_mid, 10, 0, 45.0 * deg))
    (MarkerInstance(svgpp::marker_mid, 10, 10, 135.0 * deg))
    (MarkerInstance(svgpp::marker_mid, 0, 10, -135.0 * deg))
    (MarkerInstance(svgpp::marker_mid, 0, 0, -45.0 * deg))
    (MarkerInstance(svgpp::marker_mid, 20, 20, -45.0 * deg))
    (MarkerInstance(svgpp::marker_mid, 30, 20, 45.0 * deg))
    (MarkerInstance(svgpp::marker_mid, 30, 30, 135.0 * deg))
    (MarkerInstance(svgpp::marker_mid, 20, 30, -135.0 * deg))
    (MarkerInstance(svgpp::marker_end, 20, 20, -45.0 * deg))
    );
}

TEST(path_markers_adapter, test3)
{
  DoConfigTests("M0 0 h 10 h 0 h 0 v 10 h -10 z", list_of
    (MarkerInstance(svgpp::marker_start, 0, 0, -45.0 * deg))
    (MarkerInstance(svgpp::marker_mid, 10, 0, 45.0 * deg))
    (MarkerInstance(svgpp::marker_mid, 10, 0, 45.0 * deg))
    (MarkerInstance(svgpp::marker_mid, 10, 0, 45.0 * deg))
    (MarkerInstance(svgpp::marker_mid, 10, 10, 135.0 * deg))
    (MarkerInstance(svgpp::marker_mid, 0, 10, -135.0 * deg))
    (MarkerInstance(svgpp::marker_end, 0, 0, -45.0 * deg))
    );
}

TEST(path_markers_adapter, test4)
{
  DoConfigTests("M0 0 h 0 h 0 h 10 v 10 h -10 z", list_of
    (MarkerInstance(svgpp::marker_start, 0, 0, -45.0 * deg))
    (MarkerInstance(svgpp::marker_mid, 0, 0, 0.0))
    (MarkerInstance(svgpp::marker_mid, 0, 0, 0.0))
    (MarkerInstance(svgpp::marker_mid, 10, 0, 45.0 * deg))
    (MarkerInstance(svgpp::marker_mid, 10, 10, 135.0 * deg))
    (MarkerInstance(svgpp::marker_mid, 0, 10, -135.0 * deg))
    (MarkerInstance(svgpp::marker_end, 0, 0, -45.0 * deg))
    );
}

TEST(path_markers_adapter, test5)
{
  DoConfigTests("M0 0 M 10 10", list_of
    (MarkerInstance(svgpp::marker_start, 0, 0, 0.0))
    (MarkerInstance(svgpp::marker_end, 10, 10, 0.0))
    );
}

TEST(path_markers_adapter, test6)
{
  DoConfigTests("M0 0 Z M 10 10 Z", list_of
    (MarkerInstance(svgpp::marker_start, 0, 0, 0.0))
    (MarkerInstance(svgpp::marker_mid, 0, 0, 0.0))
    (MarkerInstance(svgpp::marker_mid, 10, 10, 0.0))
    (MarkerInstance(svgpp::marker_end, 10, 10, 0.0))
    );
}
