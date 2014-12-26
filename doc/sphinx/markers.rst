.. _markers-section:

Markers
=============

SVG++ provides option to automatically calculate marker symbols orientation on lines
(**path**, **line**, **polyline** or **polygon** element).

*Markers Policy* turns on/off and configures marker position calculations.
*Marker Events Policy* defines how marker positions are passed to the user code.

Markers Policy Concept
------------------------------------

::

  struct makers_policy_concept
  {
    typedef /* ... */ directionality_policy;

    static const bool calculate_markers = /* true or false */;
    static const bool always_calculate_auto_orient = /* true or false */; 
  };

``calculate_markers = true`` 
  Enables marker position calculations. If ``calculate_markers = false``,
  then other class members aren't used.

Marker properties define which vertices contain markers. 
Depending on the value of **orient** attribute of **marker** element, marker orientation may be
fixed or calculated by line geometry (**auto** value).

``always_calculate_auto_orient = true`` 
  In this case marker orientation is calculated for each vertex (orentation required in **orient="auto"** case).

``always_calculate_auto_orient = false``
  In this case, before processing each line element, ``marker_get_config`` method of *Marker Events Policy* is called,
  to request from user code which verteces markers are required (user code should know this from marker properties).
  Marker positions are returned by *Marker Events Policy* only for vertices, for which user code requested marker calculations.

``directionality_policy`` 
  Class that defines how marker orientation is calculated and passed. 
  By default marker orientation is ``double`` value, containing angle in radians.

File ``svgpp/policy/markers.hpp`` contains some predefined *Markers Policies*: 
``policy::markers::calculate_always``, ``policy::markers::calculate`` and ``policy::markers::raw``.
``policy::markers::raw`` used by default disables automatic marker calculation.

:ref:`Named class template parameter <named-params>` for *Markers Policy* is ``markers_policy``.


Marker Events Policy Concept
----------------------------------

::

  namespace svgpp
  {
    enum marker_vertex { marker_start, marker_mid, marker_end };
    enum marker_config { marker_none, marker_orient_fixed, marker_orient_auto };
  }

::

  struct marker_events_policy_concept
  {
    typedef /* ... */ context_type;

    static void marker_get_config(context_type & context, 
      marker_config & start, marker_config & mid, marker_config & end);

    static void marker(context_type & context, marker_vertex v, 
      number_type x, number_type y, directionality_type directionality, unsigned marker_index);
    static void marker(context_type & context, marker_vertex v, 
      number_type x, number_type y, tag::orient_fixed directionality, unsigned marker_index);
  };

``marker_get_config`` method is called if ``makers_policy::always_calculate_auto_orient = false``. 
User code must set for each vertex type (``start``, ``mid`` and ``end``), whether it wants to
calculate orientation and position (``marker_orient_auto``), position only (``marker_orient_fixed``)
or neither of them (``marker_none``).

``marker`` method is called for each vertex calculated with this arguments:

  ``x`` and ``y`` - marker position

  ``v`` - vertex type (``marker_start``, ``marker_mid`` or ``marker_end``)

  ``directionality`` - marker orientation (by default ``double`` value in radians).
    If ``marker_get_config`` returned ``marker_orient_fixed`` for this type of vertex, 
    then tag ``tag::orient_fixed`` is passed instead of orientation value.

  ``marker_index``
    Marker orientations aren't always calculated and passed in the order,
    because orientation of marker on first vertex of *subpath* may be calculated only after *subpath* is finished.
    ``marker_index`` is the 0-based index of the marker in drawing order.

:ref:`Named class template parameter <named-params>` for *Marker Events Policy* is ``marker_events_policy``.

Default *Marker Events Policy* (``policy::marker_events::forward_to_method``) fowards calls to its static methods
to ``context`` object methods.

Example::

  class Context
  {
  public:
    void marker(svgpp::marker_vertex v, double x, double y, double directionality, unsigned marker_index)
    {
      if (markers_.size() <= marker_index)
        markers_.resize(marker_index + 1);
      auto & m = markers_[marker_index];
      m.v = v;
      m.x = x;
      m.y = y;
      m.directionality = directionality;
    }

  private:
    struct MarkerPosition
    {
      svgpp::marker_vertex v;
      double x, y, directionality;
    };

    std::vector<MarkerPosition> markers_;
  };

  void load()
  {
    document_traversal<
      /* ... */
      svgpp::markers_policy<svgpp::policy::markers::calculate_always>
    >::load_document(xml_root_element, context);
  }