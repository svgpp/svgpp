.. _viewport-section:

Viewport
================================

When establishing new viewport by elements **svg**, **symbol** (instanced by **use**) or **image** 
several attributes must be processed (**x**, **y**, **width**, **height**, **preserveAspectRatio**, **viewbox**)
to get the new user coordinate system, viewport and clipping path. 
SVG++ may do it itself and present result in convenient way. 
This behavior is configured by *Viewport Policy*. 
*Viewport Events Policy* defines how viewport information is passed to the user code.

**marker** element establish new viewport similar way, when it is instances in path vertices, 
so **marker** elements may also be handled by *Viewport Policy* and *Viewport Events Policy*.

Viewport Policy Concept
--------------------------------

::

  struct viewport_policy_concept
  {
    static const bool calculate_viewport          = /* true or false */;
    static const bool calculate_marker_viewport   = /* true or false */;
    static const bool calculate_pattern_viewport  = /* true or false */;
    static const bool viewport_as_transform       = /* true or false */;
  };

``calculate_viewport = true``
  Enables handling of viewport attributes for **svg** and **symbol** elements. 
  Values will be passed by calls to ``set_viewport`` or 
  ``set_viewbox_transform`` of *Viewport Events Policy*.

``calculate_marker_viewport = true``
  The same for **marker** elements.

``calculate_pattern_viewport = true``
  The same for **pattern** elements.

``viewport_as_transform = true``
  Is checked only if ``calculate_viewport = true``.
  ``set_viewbox_transform`` method of *Viewport Events Policy* isn't used, 
  new coordinate system is set by :ref:`Transform Events Policy <transform-section>` instead.

:ref:`Named class template parameter <named-params>` for *Viewport Policy* is ``viewport_policy``.

File ``svgpp/policy/viewport.hpp`` contains some predefined *Viewport Policies*. 
``policy::viewport::raw``, used by default, turns off viewport processing in SVG++. 
``policy::viewport::as_transform`` sets all boolean members to ``true``, 
thus enabling viewport attributes handling by SVG++ and 
passing coordinate system changes by *Transform Events Policy*.


Viewport Events Policy Concept
--------------------------------

::

  struct viewport_events_policy_concept
  {
    static void set_viewport(context_type & context, number_type viewport_x, number_type viewport_y, 
      number_type viewport_width, number_type viewport_height);

    static void set_viewbox_transform(context_type & context, number_type translate_x, number_type translate_y, 
      number_type scale_x, number_type scale_y, bool defer);

    static void set_viewbox_size(context_type & context, number_type viewbox_width, number_type viewbox_height);

    static void get_reference_viewport_size(context_type & context, 
      number_type & viewport_width, number_type & viewport_height);

    static void disable_rendering(context_type & context);
  };

``set_viewport`` 
  Sets new viewport position.

``set_viewbox_transform``
  Passes offset and scale that are set by combination of **viewbox** and **preserveAspectRatio**. 
  Not used if ``viewport_as_transform = true``.

``set_viewbox_size``
  Passes size of viewbox, set by **viewbox** attribute. Called only if **viewbox** attribute is present.
  It can be used to set viewport size in *Length Factory*.

``get_reference_viewport_size``
  See :ref:`viewport_referenced_element`.

``disable_rendering``
  SVG Specification says that "[**width** or **height** attribute] value of zero disables rendering of this element".
  ``disable_rendering`` will be called in such cases, so that application can behave in proper way.
  *Document Traversal Control Policy* may be used to skip processing of element content.

:ref:`Named class template parameter <named-params>` for *Viewport Events Policy* is ``viewport_events_policy``.

.. _viewport_referenced_element:

Referenced element
------------------------

When processing **svg** element referenced by **use** or **image**, **symbol** referenced by **use** 
values of width and height of referencing element are required. 

To pass to SVG++ information that element is referenced by **use** or **image**,
tag of referenced element must be passed as :ref:`referencing_element <referencing_element>` parameter
of ``document_traversal`` class.
In this case ``get_reference_viewport_size`` method of *Viewport Events Policy* will be called.
Implementation of ``get_reference_viewport_size`` must set ``viewport_width`` to
value of **width** attribute of referenced element, if this attribute is set. 
And set ``viewport_height`` value of **height** attribute of referenced element, if this attribute is set. 


Processed attributes
--------------------------------

If ``calculate_viewport = true`` or ``calculate_pattern_viewport = true`` in *Viewport Policy*, 
then SVG++ intercepts and processes attributes, 
listed in ``traits::viewport_attributes``::

  namespace traits 
  {
    typedef boost::mpl::set6<
      tag::attribute::x, 
      tag::attribute::y, 
      tag::attribute::width, 
      tag::attribute::height, 
      tag::attribute::viewBox, 
      tag::attribute::preserveAspectRatio
    > viewport_attributes;
  }

If ``calculate_marker_viewport = true`` in *Viewport Policy*, then SVG++ 
intercepts and processes marker attributes listed in ``traits::marker_viewport_attributes``::

  namespace traits 
  {
    typedef boost::mpl::set6<
      tag::attribute::refX, 
      tag::attribute::refY, 
      tag::attribute::markerWidth, 
      tag::attribute::markerHeight, 
      tag::attribute::viewBox, 
      tag::attribute::preserveAspectRatio
    > marker_viewport_attributes;
  }

Processing of this attributes must be :ref:`enabled <processed_attributes>` by the programmer.

Order of viewport processing
-------------------------------------

Viewport attributes will be processed and result will be passed by *Viewport Events Policy* 
after all SVG element attributes are processed or when :ref:`notification <notify_in_priority>`
with tag ``tag::event::after_viewport_attributes`` arrives.