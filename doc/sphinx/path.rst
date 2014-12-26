.. _path_section:

Path
==========

Path parsing is controlled by *Path Policy* and *Path Events Policy*. 
*Path Policy* configures the adapter that simplifies path handling for application
(e.g. adapter may substitute relative coordinates with absolute ones relieving programmer from this duty).
*Path Events Policy* defines how parsed path data is passed to the user code.

Path Events Policy Concept
--------------------------

::

  struct path_events_concept
  {
    typedef /*...*/ context_type;

    static void path_move_to(context_type & context, number_type x, number_type y, AbsoluteOrRelative absoluteOrRelative);
    static void path_line_to(context_type & context, number_type x, number_type y, AbsoluteOrRelative absoluteOrRelative);
    static void path_line_to_ortho(context_type & context, number_type coord, bool horizontal, AbsoluteOrRelative absoluteOrRelative);
    static void path_cubic_bezier_to(context_type & context, number_type x1, number_type y1, 
                                          number_type x2, number_type y2, 
                                          number_type x, number_type y, 
                                          AbsoluteOrRelative absoluteOrRelative);
    static void path_cubic_bezier_to(context_type & context, 
                                          number_type x2, number_type y2, 
                                          number_type x, number_type y, 
                                          AbsoluteOrRelative absoluteOrRelative);
    static void path_quadratic_bezier_to(context_type & context, 
                                          number_type x1, number_type y1, 
                                          number_type x, number_type y, 
                                          AbsoluteOrRelative absoluteOrRelative);
    static void path_quadratic_bezier_to(context_type & context, 
                                          number_type x, number_type y, 
                                          AbsoluteOrRelative absoluteOrRelative);
    static void path_elliptical_arc_to(context_type & context, 
                                          number_type rx, number_type ry, number_type x_axis_rotation,
                                          bool large_arc_flag, bool sweep_flag, 
                                          number_type x, number_type y,
                                          AbsoluteOrRelative absoluteOrRelative);
    static void path_close_subpath(context_type & context);
    static void path_exit(context_type & context);
  };

``absoluteOrRelative`` parameter may have type ``tag::coordinate::absolute`` or
``tag::coordinate::relative`` depending on whether absolute or relative coordinates are passed in other parameters.

Depending on *Path Policy*, some of the methods aren't called by SVG++ and therefore shouldn't be implemented.

Default *Path Events Policy* (``policy::path_events::forward_to_method``) forwards calls to its static methods
to ``context`` object methods::

  struct forward_to_method
  {
    typedef Context context_type; 

    template<class Coordinate, class AbsoluteOrRelative>
    static void path_move_to(Context & context, Coordinate x, Coordinate y, AbsoluteOrRelative absoluteOrRelative)
    { 
      context.path_move_to(x, y, absoluteOrRelative); 
    }

    /*...*/
  };

:ref:`Named class template parameter <named-params>` for *Path Events Policy* is ``path_events_policy``.

.. _path_policy:

Path Policy Concept
------------------------

::

  struct path_policy_concept
  {
    static const bool absolute_coordinates_only     = /* true or false */;
    static const bool no_ortho_line_to              = /* true or false */;
    static const bool no_quadratic_bezier_shorthand = /* true or false */;
    static const bool no_cubic_bezier_shorthand     = /* true or false */;
    static const bool quadratic_bezier_as_cubic     = /* true or false */;
    static const bool arc_as_cubic_bezier           = /* true or false */; 
  };

*Path Policy* is a class with ``bool`` static member constants. 
If they all are ``false`` (as in ``policy::path::raw``), then adapter isn't used and parser passed parsed values to the user code as is.
By setting some members to ``true`` programmer may simplify his work:

  ``absolute_coordinates_only = true`` 
    Relative coordinates are replaced with corresponding absolute. 
    Therefore *Path Events Policy* methods with ``tag::coordinate::relative`` parameter aren't used.

  ``no_ortho_line_to = true`` 
    Instead of call to ``path_line_to_ortho`` with one coordinate call is made to ``path_line_to`` with two coordinates.

  ``no_quadratic_bezier_shorthand = true``
    Instead of ``path_quadratic_bezier_to`` with two coordinates (shorthand/smooth curve) 
    overload of same method with four coordinates is used.

  ``no_cubic_bezier_shorthand = true`` 
    Instead of ``path_cubic_bezier_to`` with four coordinates (shorthand/smooth curve) 
    overload of same method with six coordinates is used.

  ``quadratic_bezier_as_cubic = true`` 
    Call to ``path_quadratic_bezier_to`` is replaced with call to ``path_cubic_bezier_to``.

  ``arc_as_cubic_bezier = true`` 
    Elliptical arc is approximated with cubic Bézier curve. Call to ``path_elliptical_arc_to`` 
    substituted with series of calls to ``path_cubic_bezier_to``.

:ref:`Named class template parameter <named-params>` for *Path Policy* is ``path_policy``.

File ``svgpp/policy/path.hpp`` contains some predefined *Path Policies*. 
``policy::path::no_shorthands`` used by default limits *Path Events Policy* interface as much as possible
not using approximation.
In this case *Path Events Policy* becomes::

  struct path_events_no_shorthands_concept
  {
    typedef /*...*/ context_type;

    static void path_move_to(context_type & context, number_type x, number_type y, tag::coordinate::absolute);
    static void path_line_to(context_type & context, number_type x, number_type y, tag::coordinate::absolute);
    static void path_cubic_bezier_to(context_type & context, number_type x1, number_type y1, 
                                          number_type x2, number_type y2, 
                                          number_type x, number_type y, 
                                          tag::coordinate::absolute);
    static void path_quadratic_bezier_to(context_type & context, 
                                          number_type x1, number_type y1, 
                                          number_type x, number_type y, 
                                          tag::coordinate::absolute);
    static void path_elliptical_arc_to(context_type & context, 
                                          number_type rx, number_type ry, number_type x_axis_rotation,
                                          bool large_arc_flag, bool sweep_flag, 
                                          number_type x, number_type y,
                                          tag::coordinate::absolute);
    static void path_close_subpath(context_type & context);
    static void path_exit(context_type & context);
  };

It is better to inherit own *Path Policy* from some provided by SVG++ to easy upgrade to future versions of SVG++.