#pragma once

#include <svgpp/adapter/path.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/detail/adapt_context.hpp>
#include <svgpp/policy/detail/load_path_splitter.hpp>
#include <svgpp/policy/markers.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/math/constants/constants.hpp>

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

namespace detail
{
  template<class LoadMarkersPolicy, bool AlwaysCalculateAutoOrient>
  struct path_markers_adapter_config;

  template<class LoadMarkersPolicy>
  struct path_markers_adapter_config<LoadMarkersPolicy, false>
  {
  public:
    template<class Context>
    void request_config(Context & context)
    {
      LoadMarkersPolicy::marker_get_config(context, config_start_, config_mid_, config_end_);
    }

    template<class Context, class Coordinate>
    static void call_marker_orient_fixed(Context & context, marker_vertex v, 
      Coordinate x, Coordinate y, unsigned marker_index)
    {
      LoadMarkersPolicy::marker(context, v, x, y, tag::orient_fixed(), marker_index);
    }

    marker_config start() const { return config_start_; }
    marker_config mid() const { return config_mid_; }
    marker_config end() const { return config_end_; }

  private:
    marker_config config_start_, config_mid_, config_end_;
  };

  template<class LoadMarkersPolicy>
  struct path_markers_adapter_config<LoadMarkersPolicy, true>
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
  class MarkersPolicy = policy::markers::calculate,
  class Coordinate = typename number_type_by_context<OutputContext>::type, 
  class LoadMarkersPolicy = policy::load_markers::forward_to_method<OutputContext> 
>
struct path_markers_adapter: boost::noncopyable
{
public:
  typedef Coordinate coordinate_type;
  typedef typename MarkersPolicy::directionality_policy::directionality_type directionality_type;

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
      directionality_type directionality = MarkersPolicy::directionality_policy::segment_directionality(dx, dy);
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
          = MarkersPolicy::directionality_policy::bisector_directionality(
            *last_end_point_directionality_, *subpath_start_directionality_);
      }
      else
      {
        directionality = *last_end_point_directionality_;
        start_point_directionality = *subpath_start_directionality_;
      }
    else
      subpath_start_directionality_ = directionality = MarkersPolicy::directionality_policy::undetermined_directionality();

    if (config_.mid() == marker_orient_auto)
      for(; points_without_directionality_ > 0; --points_without_directionality_)
        LoadMarkersPolicy::marker(context_, marker_mid, last_x_, last_y_, directionality, 
          next_marker_index_++);

    switch (config_.end())
    {
    case marker_none: break;
    case marker_orient_ignore:
      config_.call_marker_orient_fixed(context_, marker_end, last_x_, last_y_, next_marker_index_++);
      break;
    case marker_orient_auto:
      LoadMarkersPolicy::marker(context_, marker_end, last_x_, last_y_, directionality, next_marker_index_++);
      break;
    }

    if (config_.start() == marker_orient_auto)
      LoadMarkersPolicy::marker(context_, marker_start,
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
          directionality = MarkersPolicy::directionality_policy::bisector_directionality(
            *last_end_point_directionality_, start_point_directionality);
        else
          directionality = start_point_directionality;

        for(++points_without_directionality_; points_without_directionality_ > 0; --points_without_directionality_)
          LoadMarkersPolicy::marker(context_, marker_mid, last_x_, last_y_, directionality, 
            next_marker_index_++);
        break;
      }
      }
    }
  }

  OutputContext & context_;
  detail::path_markers_adapter_config<LoadMarkersPolicy, MarkersPolicy::always_calculate_auto_orient> config_;

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

namespace detail
{

template<class OriginalContext>
class path_bypass_and_markers_adapter
{
  typedef typename detail::unwrap_context<OriginalContext, tag::markers_policy>::policy markers_policy;
  typedef typename detail::unwrap_context<OriginalContext, tag::load_markers_policy>::policy load_markers_policy;
  typedef typename detail::unwrap_context<OriginalContext, tag::number_type>::policy number_type;
  typedef typename detail::unwrap_context<OriginalContext, tag::load_path_policy>::policy original_load_path_policy;

  typedef path_markers_adapter<
    typename detail::unwrap_context<OriginalContext, tag::load_markers_policy>::type, 
    markers_policy, 
    number_type, 
    load_markers_policy
  > markers_adapter_t;

  typedef path_adapter<
    markers_adapter_t, 
    policy::path::no_shorthands, 
    number_type, 
    policy::load_path::forward_to_method<markers_adapter_t>
  > path_adapter_t;

  typedef path_adapter_load_path_policy<path_adapter_t, policy::path::no_shorthands, number_type> path_adapter_load_path_policy_t;

  typedef std::pair<path_adapter_t &, typename detail::unwrap_context<OriginalContext, tag::load_path_policy>::type &> splitter_context_t;

  typedef load_path_splitter<splitter_context_t, path_adapter_load_path_policy_t, original_load_path_policy> splitter_policy_t;

public:
  path_bypass_and_markers_adapter(OriginalContext & original_context)
    : markers_adapter_(detail::unwrap_context<OriginalContext, tag::load_markers_policy>::get(original_context))
    , path_adapter_(markers_adapter_)
    , splitter_context_(path_adapter_, detail::unwrap_context<OriginalContext, tag::load_path_policy>::get(original_context))
  {}

  typedef splitter_policy_t load_path_policy;
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
  typedef adapted_context_wrapper<
    OriginalContext, 
    typename type::context_type, 
    tag::load_path_policy, 
    typename type::load_path_policy
  > adapted_context;
  typedef adapted_context adapted_context_holder;

  static adapted_context adapt_context(OriginalContext & context, type & adapter)
  {
    return adapted_context(context, adapter.context());
  }
};

} // namespace detail

}