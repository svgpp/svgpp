#pragma once

#include <svgpp/definitions.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/math/constants/constants.hpp>
#include <vector>
#include <cmath>

/* TODO:
When a 'closepath' command is followed by a command other than a 'moveto' command, then the orientation of the marker corresponding to the 'closepath' command is calculated assuming that:
the path segment going into the vertex is the path segment corresponding to the closepath
the path segment coming out of the vertex is the first path segment of the subsequent subpath
*/

namespace svgpp
{

namespace tag
{
  struct orient_fixed {};
}

// TODO: class enums for C++11
enum marker_vertex { marker_start = 1, marker_mid = 2, marker_end = 3 };
enum marker_config { marker_none = 0, marker_orient_ignore = 1, marker_orient_auto = 2 };

struct marker_directionality_policy_radians
{
  typedef double directionality_type;

  static BOOST_CONSTEXPR directionality_type undetermined_directionality() // "align with the positive x-axis in user space"
  {
    return 0;
  }

  template<class Coordinate>
  static directionality_type segment_directionality(Coordinate dx, Coordinate dy)
  {
    return std::atan2(dy, dx);
  }

  static directionality_type bisector_directionality(directionality_type in_segment, directionality_type out_segment)
  {
    directionality_type dir = (in_segment + out_segment) * 0.5;
    if (std::fabs(in_segment - out_segment) > boost::math::constants::pi<double>())
      if (dir < 0)
        return dir + boost::math::constants::pi<double>();
      else
        return dir - boost::math::constants::pi<double>();
    return dir;
  }
};

struct default_marker_policy
{
  typedef marker_directionality_policy_radians directionality_policy;

  static const bool always_calculate_auto_orient = false; // Doesn't call marker_get_config if true
};

namespace detail
{
  template<class Context>
  struct default_load_marker_policy
  {
    template<class Coordinate, class Directionality>
    static void marker(Context & context, marker_vertex v, 
      Coordinate x, Coordinate y, Directionality directionality, unsigned marker_index)
    {
      context.marker(v, x, y, directionality, marker_index);
    }

    static void marker_get_config(Context & context, marker_config & start, marker_config & mid, marker_config & end)
    {
      context.marker_get_config(start, mid, end);
    }
  };

  template<class LoadMarkerPolicy, bool AlwaysCalculateAutoOrient>
  struct path_markers_adapter_config;

  template<class LoadMarkerPolicy>
  struct path_markers_adapter_config<LoadMarkerPolicy, false>
  {
  public:
    template<class Context>
    void request_config(Context & context)
    {
      LoadMarkerPolicy::marker_get_config(context, config_start_, config_mid_, config_end_);
    }

    template<class Context, class Coordinate>
    static void call_marker_orient_fixed(Context & context, marker_vertex v, 
      Coordinate x, Coordinate y, unsigned marker_index)
    {
      LoadMarkerPolicy::marker(context, v, x, y, tag::orient_fixed(), marker_index);
    }

    marker_config start() const { return config_start_; }
    marker_config mid() const { return config_mid_; }
    marker_config end() const { return config_end_; }

  private:
    marker_config config_start_, config_mid_, config_end_;
  };

  template<class LoadMarkerPolicy>
  struct path_markers_adapter_config<LoadMarkerPolicy, true>
  {
  public:
    template<class Context>
    void request_config(Context &)
    {}

    template<class Context, class Coordinate>
    void call_marker_orient_fixed(Context &, marker_vertex, 
      Coordinate, Coordinate, unsigned)
    {
      BOOST_ASSERT(false);
    }

    BOOST_CONSTEXPR marker_config start() const { return marker_orient_auto; }
    BOOST_CONSTEXPR marker_config mid() const { return marker_orient_auto; }
    BOOST_CONSTEXPR marker_config end() const { return marker_orient_auto; }
  };
}

template<
  class OutputContext, 
  class Coordinate, 
  class MarkerPolicy = default_marker_policy,
  class LoadMarkerPolicy = detail::default_load_marker_policy<OutputContext> 
>
struct path_markers_adapter: boost::noncopyable
{
public:
  typedef Coordinate coordinate_type;
  typedef typename MarkerPolicy::directionality_policy::directionality_type directionality_type;

  path_markers_adapter(OutputContext & context)
    : context_(context)
    , in_path_(false)
    , in_subpath_(false)
    , subpath_first_segment_(true)
    , points_without_directionality_(0)
    , next_marker_index_(0)
  {}

  void path_move_to(
    coordinate_type x, 
    coordinate_type y, tag::absolute_coordinate tag)
  {
    if (!in_path_)
    {
      // "Moveto" must be the first command in path - checked by grammar
      in_path_ = true;
      config_.request_config(context_);
    }

    if (in_subpath_)
      exit_subpath(false);

    in_subpath_ = true;
    last_x_ = x;
    last_y_ = y;
    subpath_start_x_ = x;
    subpath_start_y_ = y;

    switch (config_.start())
    {
    case marker_none: break;
    case marker_orient_ignore: 
      config_.call_marker_orient_fixed(context_, marker_start, x, y, next_marker_index_++);
      break;
    case marker_orient_auto:
      last_start_marker_index_ = next_marker_index_++;
      break;
    }
  }

  void path_line_to(
    coordinate_type x, 
    coordinate_type y, tag::absolute_coordinate tag)
  {
    in_subpath_ = true;

    line_to(x, y);
  }

  void path_quadratic_bezier_to(
    coordinate_type x1, 
    coordinate_type y1, 
    coordinate_type x, 
    coordinate_type y, 
    tag::absolute_coordinate tag)
  {
    in_subpath_ = true;
  }

  void path_cubic_bezier_to(
    coordinate_type x1, 
    coordinate_type y1, 
    coordinate_type x2, 
    coordinate_type y2, 
    coordinate_type x, 
    coordinate_type y, 
    tag::absolute_coordinate tag)
  {
    in_subpath_ = true;
  }

  void path_elliptical_arc_to(
    coordinate_type rx, 
    coordinate_type ry, 
    coordinate_type x_axis_rotation, 
    bool large_arc_flag, bool sweep_flag, 
    coordinate_type x, 
    coordinate_type y, 
    tag::absolute_coordinate tag)
  {
    in_subpath_ = true;
  }

  void path_close_subpath()
  {
    if (in_subpath_)
    {
      line_to(subpath_start_x_, subpath_start_y_);

      exit_subpath(true);
    }
  }

  void path_exit() // TODO
  {
    if (in_subpath_)
      exit_subpath(false);
  }

private:
  void line_to(coordinate_type x, coordinate_type y)
  {
    coordinate_type dx = x - last_x_;
    coordinate_type dy = y - last_y_;
    if (dx == 0 && dy == 0)
    {
      on_zero_length_segment();
    }
    else
    {
      directionality_type directionality = MarkerPolicy::directionality_policy::segment_directionality(dx, dy);
      on_valid_start_point_directionality(directionality);
      last_end_point_directionality_ = directionality;
      last_x_ = x;
      last_y_ = y;
    }
  }

  void on_zero_length_segment()
  {
    if (subpath_first_segment_)
      subpath_first_segment_ = false;
    else
      switch (config_.mid())
      {
      case marker_none: break;
      case marker_orient_ignore:
        config_.call_marker_orient_fixed(context_, marker_mid, last_x_, last_y_, next_marker_index_++);
        break;
      case marker_orient_auto:
        ++points_without_directionality_;
        break;
      }
  }

  void exit_subpath(bool by_close_path)
  {
    directionality_type directionality, start_point_directionality;
    
    if (last_end_point_directionality_)
      if (by_close_path)
      {
        BOOST_ASSERT(subpath_start_directionality_);
        start_point_directionality = directionality 
          = MarkerPolicy::directionality_policy::bisector_directionality(
            *last_end_point_directionality_, *subpath_start_directionality_);
      }
      else
      {
        directionality = *last_end_point_directionality_;
        start_point_directionality = *subpath_start_directionality_;
      }
    else
      subpath_start_directionality_ = directionality = MarkerPolicy::directionality_policy::undetermined_directionality();

    if (config_.mid() == marker_orient_auto)
      for(; points_without_directionality_ > 0; --points_without_directionality_)
        LoadMarkerPolicy::marker(context_, marker_mid, last_x_, last_y_, directionality, 
          next_marker_index_++);

    switch (config_.end())
    {
    case marker_none: break;
    case marker_orient_ignore:
      config_.call_marker_orient_fixed(context_, marker_end, last_x_, last_y_, next_marker_index_++);
      break;
    case marker_orient_auto:
      LoadMarkerPolicy::marker(context_, marker_end, last_x_, last_y_, directionality, next_marker_index_++);
      break;
    }

    if (config_.start() == marker_orient_auto)
      LoadMarkerPolicy::marker(context_, marker_start,
        subpath_start_x_, subpath_start_y_, 
        start_point_directionality, 
        last_start_marker_index_);

    in_subpath_ = false;
    subpath_first_segment_ = true;
    subpath_start_directionality_.reset();
    last_end_point_directionality_.reset();
    points_without_directionality_ = 0;
  }

  void on_valid_start_point_directionality(
    directionality_type start_point_directionality)
  {
    if (!subpath_start_directionality_)
      subpath_start_directionality_ = start_point_directionality;

    if (subpath_first_segment_)
      subpath_first_segment_ = false;
    else
    {
      switch (config_.mid())
      {
      case marker_none: break;
      case marker_orient_ignore:
        config_.call_marker_orient_fixed(context_, marker_mid, last_x_, last_y_, next_marker_index_++);
        break;
      case marker_orient_auto:
      {
        directionality_type directionality;
        if (last_end_point_directionality_)
          directionality = MarkerPolicy::directionality_policy::bisector_directionality(
            *last_end_point_directionality_, start_point_directionality);
        else
          directionality = start_point_directionality;

        for(++points_without_directionality_; points_without_directionality_ > 0; --points_without_directionality_)
          LoadMarkerPolicy::marker(context_, marker_mid, last_x_, last_y_, directionality, 
            next_marker_index_++);
        break;
      }
      }
    }
  }

  OutputContext & context_;
  detail::path_markers_adapter_config<LoadMarkerPolicy, MarkerPolicy::always_calculate_auto_orient> config_;

  bool in_path_;
  bool in_subpath_;
  bool subpath_first_segment_;
  coordinate_type subpath_start_x_, subpath_start_y_;
  coordinate_type last_x_, last_y_;
  int points_without_directionality_;
  boost::optional<directionality_type> subpath_start_directionality_;
  boost::optional<directionality_type> last_end_point_directionality_;
  unsigned next_marker_index_;
  unsigned last_start_marker_index_;
};

}