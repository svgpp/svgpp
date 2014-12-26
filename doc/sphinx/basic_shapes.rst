.. _Associative Sequence: http://www.boost.org/doc/libs/1_56_0/libs/mpl/doc/refmanual/associative-sequence.html

.. _basic_shapes:

Basic Shapes
====================

`Basic shapes <http://www.w3.org/TR/SVG/shapes.html>`_ (**rect**, **circle**, **ellipse**, **line**, **polyline**
and **polygon**) can be automatically converted to *path* commands.

Basic Shapes Policy Concept
--------------------------------

::

  struct basic_shapes_policy
  {
    typedef /* Associative Sequence */ convert_to_path;
    typedef /* Associative Sequence */ collect_attributes;

    static const bool convert_only_rounded_rect_to_path = /* true or false */;
  };

``convert_to_path``
  `Associative Sequence`_ (e.g. ``boost::mpl::set``), containing SVG *basic shapes* elements tags,
  which will be converted to **path**.  Generated **path** uses *Path Policy* and *Path Events Policy* settings.

``collect_attributes``
  `Associative Sequence`_, containing any of **rect**, **circle**, **ellipse** or **line** element tags.
  Geometry of elements included in the sequence will be passed with single call instead of separate attribute
  handling (see *Basic Shapes Events Policy*).

``convert_only_rounded_rect_to_path``
  If static member constant ``convert_only_rounded_rect_to_path`` equals to ``true`` and 
  ``tag::element::rect`` is included in ``convert_to_path`` sequence, 
  then only rounded rectangles will be converted to **path**,
  and regular rectangles will be handle like if ``tag::element::rect`` is included in ``collect_attributes``.

In ``document_traversal`` processing of attributes describing *basic shapes* geometry (**x**, **y**,
**r** etc) must be :ref:`enabled <processed_attributes>`, 
i. e. they must be included in ``processed_attributes`` or excluded from ``ignored_attributes``.
`Associative Sequence`_ ``traits::shapes_attributes_by_element`` contains tags of all such attributes 
for *basic shapes*.

:ref:`Named class template parameter <named-params>` for *Basic Shapes Policy* is ``basic_shapes_policy``.


Basic Shapes Events Policy Concept
-------------------------------------

::

  struct basic_shapes_events_policy
  {
    static void set_rect(Context & context, Coordinate x, Coordinate y, Coordinate width, Coordinate height,
      Coordinate rx, Coordinate ry);
    static void set_rect(Context & context, Coordinate x, Coordinate y, Coordinate width, Coordinate height);
    static void set_line(Context & context, Coordinate x1, Coordinate y1, Coordinate x2, Coordinate y2);
    static void set_circle(Context & context, Coordinate cx, Coordinate cy, Coordinate r);
    static void set_ellipse(Context & context, Coordinate cx, Coordinate cy, Coordinate rx, Coordinate ry);
  };

*Basic Shapes Events Policy* is used for *basic shapes* (except **polyline** and **polygon**) elements, 
that are listed in ``collect_attributes`` field of *Basic Shapes Policy*.

Adapters that implement these conversions, use ``length_to_user_coordinate`` method of *Length Factory*
to get *user coordinates* value by *length*. These adapters passes default values if attributes are missing
and check correctness of attributes. If value disables rendering of the element
according to SVG specification, then *Basic Shapes Events Policy* methods aren't called, 
and if an attribute has negative value that is not permitted by specification, 
then ``negative_value`` function of :ref:`Error Policy <error_policy>` is called.

Default *Basic Shapes Events Policy* (``policy::basic_shapes_events::forward_to_method``) 
forwards calls to its static methods to ``context`` object methods::

  struct forward_to_method
  {
    template<class Coordinate>
    static void set_rect(Context & context, Coordinate x, Coordinate y, Coordinate width, Coordinate height,
      Coordinate rx, Coordinate ry)
    {
      context.set_rect(x, y, width, height, rx, ry);
    }

    /*...*/
  };

:ref:`Named class template parameter <named-params>` for *Basic Shapes Events Policy* is ``basic_shapes_events_policy``.
