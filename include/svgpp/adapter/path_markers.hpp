// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/adapter/path.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/detail/adapt_context.hpp>
#include <svgpp/policy/detail/path_events_splitter.hpp>
#include <svgpp/policy/markers.hpp>
#include <svgpp/policy/marker_events.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/math/constants/constants.hpp>

/* TODO:
When a 'closepath' command is followed by a command other than a 'moveto' command, then the orientation of the marker 
corresponding to the 'closepath' command is calculated assuming that:
  the path segment going into the vertex is the path segment corresponding to the closepath
  the path segment coming out of the vertex is the first path segment of the subsequent subpath
*/

namespace svgpp
{

namespace tag
{
  struct orient_fixed {};
}

namespace detail
{
  template<class MarkerEventsPolicy, bool AlwaysCalculateAutoOrient>
  struct path_markers_adapter_config;

  template<class MarkerEventsPolicy>
  struct path_markers_adapter_config<MarkerEventsPolicy, false>
  {
  public:
    template<class Context>
    void request_config(Context & context)
    {
      MarkerEventsPolicy::marker_get_config(context, config_start_, config_mid_, config_end_);
    }

    template<class Context, class Coordinate>
    static void call_marker_orient_fixed(Context & context, marker_vertex v, 
      Coordinate x, Coordinate y, unsigned marker_index)
    {
      MarkerEventsPolicy::marker(context, v, x, y, tag::orient_fixed(), marker_index);
    }

    marker_config start() const { return config_start_; }
    marker_config mid() const { return config_mid_; }
    marker_config end() const { return config_end_; }

    bool no_markers() const 
    { 
      return config_start_ == marker_none
        && config_mid_ == marker_none
        && config_end_ == marker_none;
    }

  private:
    marker_config config_start_, config_mid_, config_end_;
  };

  template<class MarkerEventsPolicy>
  struct path_markers_adapter_config<MarkerEventsPolicy, true>
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
    BOOST_CONSTEXPR bool no_markers() const { return false; }
  };
}

template<
  class OutputContext, 
  class MarkersPolicy = policy::markers::calculate,
  class Coordinate = typename number_type_by_context<OutputContext>::type, 
  class MarkerEventsPolicy = policy::marker_events::forward_to_method<OutputContext> 
>
class path_markers_adapter: boost::noncopyable
{
public:
  typedef Coordinate coordinate_type;
  typedef typename MarkersPolicy::directionality_policy::directionality_type directionality_type;

  path_markers_adapter(OutputContext & context)
    : context_(context)
    , first_vertex_(true)
    , in_subpath_(false)
    , subpath_first_segment_(true)
    , points_without_directionality_(0)
    , next_marker_index_(0)
    , close_subpath_delayed_(false)
  {}

  void path_move_to(
    coordinate_type x, 
    coordinate_type y, tag::coordinate::absolute)
  {
    bool first_vertex = first_vertex_;
    if (first_vertex_)
    {
      // "Moveto" must be the first command in path - checked by grammar
      config_.request_config(context_);
      first_vertex_ = false;
    }

    if (config_.no_markers())
      return;

    on_next_command();
    
    exit_subpath(false, false);

    last_x_ = x;
    last_y_ = y;
    subpath_start_.x_ = x;
    subpath_start_.y_ = y;

    if (first_vertex)
    {
      switch (config_.start())
      {
      case marker_none: break;
      case marker_orient_fixed: 
        config_.call_marker_orient_fixed(context_, marker_start, x, y, next_marker_index_++);
        break;
      case marker_orient_auto:
        subpath_start_.marker_index_ = next_marker_index_++;
        break;
      }
    }
    subpath_start_.is_first_vertex_ = first_vertex;
    in_subpath_ = true;
  }

  void path_line_to(
    coordinate_type x, 
    coordinate_type y, tag::coordinate::absolute)
  {
    if (config_.no_markers())
      return;

    on_next_command();

    in_subpath_ = true;

    line_to(x, y);
  }

  void path_quadratic_bezier_to(
    coordinate_type x1, 
    coordinate_type y1, 
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::absolute)
  {
    if (config_.no_markers())
      return;

    on_next_command();

    in_subpath_ = true;

    coordinate_type dx1 = x1 - last_x_;
    coordinate_type dy1 = y1 - last_y_;
    coordinate_type dx2 = x - x1;
    coordinate_type dy2 = y - y1;
    if ( ( dx1 == 0 && dy1 == 0 ) || ( dx2 == 0 && dy2 == 0 ) )
    {
      line_to(x, y);
    }
    else
    {
      on_nonzero_length_segment(
        MarkersPolicy::directionality_policy::segment_directionality(dx1, dy1),
        MarkersPolicy::directionality_policy::segment_directionality(dx2, dy2));
      last_x_ = x;
      last_y_ = y;
    }
  }

  void path_cubic_bezier_to(
    coordinate_type x1, 
    coordinate_type y1, 
    coordinate_type x2, 
    coordinate_type y2, 
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::absolute)
  {
    if (config_.no_markers())
      return;

    on_next_command();

    in_subpath_ = true;

    coordinate_type dx1 = x1 - last_x_;
    coordinate_type dy1 = y1 - last_y_;
    coordinate_type dx2 = x - x2;
    coordinate_type dy2 = y - y2;
    if ( ( dx1 == 0 && dy1 == 0 && ( ( dx2 == 0 && dy2 == 0 ) || ( x2 == last_x_ && y2 == last_y_ ) ) )
      || ( dx2 == 0 && dy2 == 0 && x1 == x && y1 == y ) ) 
    {
      line_to(x, y);
    }
    else
    {
      if (dx1 == 0 && dy1 == 0)
      {
        dx1 = x2 - last_x_;
        dy1 = y2 - last_y_;
      }
      else if (dx2 == 0 && dy2 == 0)
      {
        dx2 = x - x1;
        dy2 = y - y1;
      }
      on_nonzero_length_segment(
        MarkersPolicy::directionality_policy::segment_directionality(dx1, dy1),
        MarkersPolicy::directionality_policy::segment_directionality(dx2, dy2));
      last_x_ = x;
      last_y_ = y;
    }
  }

  void path_elliptical_arc_to(
    coordinate_type rx, 
    coordinate_type ry, 
    coordinate_type x_axis_rotation, 
    bool large_arc_flag, bool sweep_flag, 
    coordinate_type x, 
    coordinate_type y, 
    tag::coordinate::absolute)
  {
    if (config_.no_markers())
      return;

    on_next_command();

    in_subpath_ = true;

    x_axis_rotation *= boost::math::constants::degree<coordinate_type>();
    coordinate_type cx, cy, theta1, theta2;
    arc_endpoint_to_center(last_x_, last_y_, x, y,
      rx, ry, x_axis_rotation, large_arc_flag, sweep_flag,
      cx, cy, theta1, theta2);

    coordinate_type sin_theta = std::sin(x_axis_rotation);
    coordinate_type cos_theta = std::cos(x_axis_rotation);
    coordinate_type a_sin_eta1 = rx * std::sin(theta1);
    coordinate_type b_cos_eta1 = ry * std::cos(theta1);
    coordinate_type deriv_x1 = -a_sin_eta1 * cos_theta - b_cos_eta1 * sin_theta;
    coordinate_type deriv_y1 = -a_sin_eta1 * sin_theta + b_cos_eta1 * cos_theta;
    coordinate_type a_sin_eta2 = rx * std::sin(theta2);
    coordinate_type b_cos_eta2 = ry * std::cos(theta2);
    coordinate_type deriv_x2 = -a_sin_eta2 * cos_theta - b_cos_eta2 * sin_theta;
    coordinate_type deriv_y2 = -a_sin_eta2 * sin_theta + b_cos_eta2 * cos_theta;

    coordinate_type sweep_sign = sweep_flag ? 1 : -1;

    on_nonzero_length_segment(
      MarkersPolicy::directionality_policy::segment_directionality(deriv_x1 * sweep_sign, deriv_y1 * sweep_sign),
      MarkersPolicy::directionality_policy::segment_directionality(deriv_x2 * sweep_sign, deriv_y2 * sweep_sign));
    last_x_ = x;
    last_y_ = y;
  }

  void path_close_subpath()
  {
    if (config_.no_markers())
      return;

    on_next_command();

    // We must know whether it is end or middle point of path to choose marker configuration.
    // So we delay processing till next path command or path_exit().
    close_subpath_delayed_ = true;
  }

  void path_exit()
  {
    if (config_.no_markers())
      return;

    exit_subpath(close_subpath_delayed_, true);
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
      directionality_type directionality = MarkersPolicy::directionality_policy::segment_directionality(dx, dy);
      on_nonzero_length_segment(directionality, directionality);
      last_x_ = x;
      last_y_ = y;
    }
  }

  void exit_subpath(bool by_close_path, bool path_end)
  {
    if (!in_subpath_)
      return;

    if (by_close_path)
      line_to(subpath_start_.x_, subpath_start_.y_);

    directionality_type subpath_end_directionality, subpath_start_marker_directionality;
    
    if (last_end_point_directionality_)
    {
      BOOST_ASSERT(subpath_start_.directionality_);
      if (by_close_path)
      {
        subpath_start_marker_directionality 
          = MarkersPolicy::directionality_policy::bisector_directionality(
            *last_end_point_directionality_, *subpath_start_.directionality_);
      }
      else
      {
        subpath_start_marker_directionality = *subpath_start_.directionality_; 
      }
      subpath_end_directionality = *last_end_point_directionality_;
    }
    else
      subpath_start_marker_directionality = subpath_end_directionality 
        = MarkersPolicy::directionality_policy::undetermined_directionality();

    if (config_.mid() == marker_orient_auto)
      for(; points_without_directionality_ > 0; --points_without_directionality_)
        MarkerEventsPolicy::marker(context_, marker_mid, last_x_, last_y_, subpath_end_directionality, 
          next_marker_index_++);

    if (!(subpath_first_segment_ && subpath_start_.is_first_vertex_))
      switch (path_end ? config_.end() : config_.mid())
      {
      case marker_none: break;
      case marker_orient_fixed:
        config_.call_marker_orient_fixed(context_, path_end ? marker_end : marker_mid, last_x_, last_y_, next_marker_index_++);
        break;
      case marker_orient_auto:
        if (subpath_first_segment_)
          subpath_start_.marker_index_ = next_marker_index_++;
        else
          MarkerEventsPolicy::marker(context_, path_end ? marker_end : marker_mid, last_x_, last_y_, 
            by_close_path ? subpath_start_marker_directionality : subpath_end_directionality, 
            next_marker_index_++);
        break;
      }

    {
      marker_vertex subpath_start_vertex;
      marker_config subpath_start_config;
      if (subpath_start_.is_first_vertex_)
      {
        subpath_start_vertex = marker_start;
        subpath_start_config = config_.start();
      }
      else 
      {
        subpath_start_vertex = (subpath_first_segment_ && path_end) ? marker_end : marker_mid;
        subpath_start_config = (subpath_first_segment_ && path_end) ? config_.end() : config_.mid();
      }
      if (subpath_start_config == marker_orient_auto)
        MarkerEventsPolicy::marker(context_, 
          subpath_start_vertex,
          subpath_start_.x_, subpath_start_.y_, 
          subpath_start_marker_directionality, 
          subpath_start_.marker_index_);
    }

    in_subpath_ = false;
    subpath_first_segment_ = true;
    subpath_start_.directionality_.reset();
    last_end_point_directionality_.reset();
    points_without_directionality_ = 0;
  }

  void on_zero_length_segment()
  {
    if (!(subpath_first_segment_ && subpath_start_.is_first_vertex_))
      switch (config_.mid())
      {
      case marker_none: break;
      case marker_orient_fixed:
        config_.call_marker_orient_fixed(context_, marker_mid, last_x_, last_y_, next_marker_index_++);
        break;
      case marker_orient_auto:
        if (subpath_first_segment_)
          subpath_start_.marker_index_ = next_marker_index_++;
        else
          ++points_without_directionality_;
        break;
      }
    subpath_first_segment_ = false;
  }

  void on_nonzero_length_segment(
    directionality_type subpath_start_directionality, directionality_type end_point_directionality)
  {
    if (!subpath_start_.directionality_)
      subpath_start_.directionality_ = subpath_start_directionality;

    if (!(subpath_first_segment_ && subpath_start_.is_first_vertex_))
    {
      switch (config_.mid())
      {
      case marker_none: break;
      case marker_orient_fixed:
        config_.call_marker_orient_fixed(context_, marker_mid, last_x_, last_y_, next_marker_index_++);
        break;
      case marker_orient_auto:
        if (subpath_first_segment_)
          subpath_start_.marker_index_ = next_marker_index_++;
        else
        {
          directionality_type directionality;
          if (last_end_point_directionality_)
            directionality = MarkersPolicy::directionality_policy::bisector_directionality(
              *last_end_point_directionality_, subpath_start_directionality);
          else
            directionality = subpath_start_directionality;

          for(++points_without_directionality_; points_without_directionality_ > 0; --points_without_directionality_)
            MarkerEventsPolicy::marker(context_, marker_mid, last_x_, last_y_, directionality, 
              next_marker_index_++);
        }
        break;
      }
    }

    subpath_first_segment_ = false;
    last_end_point_directionality_ = end_point_directionality;
  }

  void on_next_command()
  {
    if (close_subpath_delayed_)
    {
      close_subpath_delayed_ = false;

      exit_subpath(true, false);
    }
  }

  OutputContext & context_;
  detail::path_markers_adapter_config<MarkerEventsPolicy, MarkersPolicy::always_calculate_auto_orient> config_;

  bool first_vertex_;
  bool in_subpath_;
  bool subpath_first_segment_;
  bool close_subpath_delayed_;
  struct subpath_start_t
  {
    coordinate_type x_, y_;
    unsigned marker_index_;
    bool is_first_vertex_; // I.e. starting move-to of the path
    boost::optional<directionality_type> directionality_;
  } subpath_start_;
  coordinate_type last_x_, last_y_;
  int points_without_directionality_;
  boost::optional<directionality_type> last_end_point_directionality_;
  unsigned next_marker_index_;
};

namespace detail
{

template<class OriginalContext>
class path_bypass_and_markers_adapter
{
  typedef typename detail::unwrap_context<OriginalContext, tag::markers_policy>::policy markers_policy;
  typedef typename detail::unwrap_context<OriginalContext, tag::marker_events_policy>::policy marker_events_policy;
  typedef typename detail::unwrap_context<OriginalContext, tag::number_type>::policy number_type;
  typedef typename detail::unwrap_context<OriginalContext, tag::path_events_policy>::policy original_path_events_policy;

  typedef path_markers_adapter<
    typename detail::unwrap_context<OriginalContext, tag::marker_events_policy>::type, 
    markers_policy, 
    number_type, 
    marker_events_policy
  > markers_adapter_t;

  typedef path_adapter<
    markers_adapter_t, 
    policy::path::no_shorthands, 
    number_type, 
    policy::path_events::forward_to_method<markers_adapter_t>
  > path_adapter_t;

  typedef path_adapter_path_events_policy<path_adapter_t, policy::path::no_shorthands, number_type> path_adapter_path_events_policy_t;

  typedef std::pair<path_adapter_t &, typename detail::unwrap_context<OriginalContext, tag::path_events_policy>::type &> splitter_context_t;

  typedef path_events_splitter<splitter_context_t, path_adapter_path_events_policy_t, original_path_events_policy> splitter_policy_t;

public:
  path_bypass_and_markers_adapter(OriginalContext & original_context)
    : markers_adapter_(detail::unwrap_context<OriginalContext, tag::marker_events_policy>::get(original_context))
    , path_adapter_(markers_adapter_)
    , splitter_context_(path_adapter_, detail::unwrap_context<OriginalContext, tag::path_events_policy>::get(original_context))
  {}

  typedef splitter_policy_t path_events_policy;
  typedef splitter_context_t context_type;
  context_type & context() { return splitter_context_; }

private:
  markers_adapter_t markers_adapter_;
  path_adapter_t path_adapter_;
  splitter_context_t splitter_context_;
};

template<class OriginalContext, class Enabled = void>
struct path_markers_adapter_if_needed
{
private:
  struct adapter_stub
  {
    template<class Context> adapter_stub(Context const &) {}
  };

public:
  typedef adapter_stub type;
  typedef OriginalContext adapted_context;
  typedef OriginalContext & adapted_context_holder;

  static OriginalContext & adapt_context(OriginalContext & context, adapter_stub &)
  {
    return context;
  }
};

template<class OriginalContext>
struct path_markers_adapter_if_needed<OriginalContext, 
  typename boost::enable_if_c<
    detail::unwrap_context<OriginalContext, tag::markers_policy>::policy::calculate_markers>::type>
{
  typedef path_bypass_and_markers_adapter<OriginalContext> type;
  typedef const 
    adapted_context_wrapper<
      const adapted_policy_context_wrapper<
        OriginalContext, 
        tag::path_policy, 
        policy::path::raw
      >,
      typename type::context_type, 
      tag::path_events_policy, 
      typename type::path_events_policy
    > adapted_context;
  typedef adapted_context adapted_context_holder;

  static adapted_context adapt_context(OriginalContext & context, type & adapter)
  {
    return adapted_context(
      adapt_context_policy<tag::path_policy, policy::path::raw>(context), 
      adapter.context());
  }
};

} // namespace detail

}
