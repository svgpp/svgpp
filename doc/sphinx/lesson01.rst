.. _Associative Sequence: http://www.boost.org/doc/libs/1_57_0/libs/mpl/doc/refmanual/associative-sequence.html
.. _Metafunction Class: http://www.boost.org/doc/libs/1_57_0/libs/mpl/doc/refmanual/metafunction-class.html

Tutorial
================

This tutorial introduces SVG++ features by adding them one-by-one into some sample application.

All SVG++ headers may be included through this one::

#include <svgpp/svgpp.hpp>

All SVG++ code is placed in ``svgpp`` namespace. We'll import the entire namespace in our sample.

.. _tutorial:

Handling Shapes Geometry
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Basic pattern of SVG++ usage: call ``document_traversal::load_document`` method, passing XML element and *context* to it.
Library will call methods of *context*, passing the parsed data.

::

  #include <svgpp/svgpp.hpp>

  using namespace svgpp;

  class Context
  {
  public:
    void path_move_to(double x, double y, tag::coordinate::absolute);
    void path_line_to(double x, double y, tag::coordinate::absolute);
    void path_cubic_bezier_to(
      double x1, double y1, 
      double x2, double y2, 
      double x, double y, 
      tag::coordinate::absolute);
    void path_quadratic_bezier_to(
      double x1, double y1, 
      double x, double y, 
      tag::coordinate::absolute);
    void path_elliptical_arc_to(
      double rx, double ry, double x_axis_rotation,
      bool large_arc_flag, bool sweep_flag, 
      double x, double y,
      tag::coordinate::absolute);
    void path_close_subpath();
    void path_exit();

    void on_enter_element(tag::element::any);
    void on_exit_element();
  };

  typedef 
    boost::mpl::set<
      // SVG Structural Elements
      tag::element::svg,
      tag::element::g,
      // SVG Shape Elements
      tag::element::circle,
      tag::element::ellipse,
      tag::element::line,
      tag::element::path,
      tag::element::polygon,
      tag::element::polyline,
      tag::element::rect
    >::type processed_elements_t;

  void loadSvg(xml_element_t xml_root_element)
  {
    Context context;
    document_traversal<
      processed_elements<processed_elements_t>,
      processed_attributes<traits::shapes_attributes_by_element>
    >::load_document(xml_root_element, context);
  }

:ref:`document_traversal <document_traversal>` is a facade that provides access to most library capabilities.

In most cases only some subset of SVG elements is needed, so we pass
:ref:`named template parameter <named-params>` :ref:`processed_elements <processed_elements>` 
to ``document_traversal`` template class. In our case it is ``processed_elements_t`` -
``boost::mpl::set`` that combines ``traits::shape_elements`` (enumerates SVG 
`shapes <http://www.w3.org/TR/SVG11/intro.html#TermShape>`_) with **svg** and **g** elements.

SVG++ references SVG element types by :ref:`tags <tags-section>`.

We choose SVG attributes subset and pass it as
:ref:`processed_attributes <processed_attributes>` parameter.  
``traits::shapes_attributes_by_element`` contains attributes, that describe geometry of all shapes 
({**x**, **y**, **width**, **height**, **rx** and **ry**} for **rect**, {**d**} for **path** etc.). 

In this sample the same context instance is used for all SVG elements.
``Context::on_enter_element(element_tag)`` is called when moving to child SVG element, type
of child element passed as tag in the only argument (``tag::element::any`` is a base class for all element tags).
``on_exit_element()`` is called when processing of child element is finished:

=====================   =============================================
XML element             Call to context
=====================   =============================================
``<svg>``               ``on_enter_element(tag::element::svg())``
``<rect``               ``on_enter_element(tag::element::rect())``
``x="100" y="200"``
``/>``                  ``on_exit_element()``
``<g>``                 ``on_enter_element(tag::element::g())``
``<rect``               ``on_enter_element(tag::element::rect())``
``x="300" y="100"``
``/>``                  ``on_exit_element()``
``</g>``                ``on_exit_element()``
``</svg>``              ``on_exit_element()``
=====================   =============================================

Calls like ``path_XXXX`` except ``path_exit`` correspond to SVG  
`path data <http://www.w3.org/TR/SVG11/paths.html#PathData>`_ commands. 
``path_exit`` is called after path data attribute was parsed.

SVG++ by default (see :ref:`Path Policy <path_policy>` for details):

- converts relative coordinates to absolute ones;
- commands for horizontal and vertical lines (**H**, **h**, **V**, **v**) converts to calls to ``path_line_to`` with two coordinates;
- shorthand/smooth curveto and shorthand/smooth quadratic Bézier curveto replaces with calls with full parameters list.

SVG++ by default converts `basic shapes <http://www.w3.org/TR/SVG11/shapes.html>`_ to path
(see :ref:`Basic Shapes Policy <basic_shapes>` for details).

XML Parser
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

We didn't declared ``xml_element_t`` yet. 
Let's use `RapidXML NS <https://github.com/svgpp/rapidxml_ns>`_ library (it is a clone of 
`RapidXML <http://rapidxml.sourceforge.net/>`_ with namespace handling added) that comes with SVG++ 
in the ``third_party/rapidxml_ns/rapidxml_ns.hpp`` file. It's a single header library, so we just need to point to its header::

  #include <rapidxml_ns/rapidxml_ns.hpp>

Then we must include SVG++ *policy* for chosen XML parser::

  #include <svgpp/policy/xml/rapidxml_ns.hpp>

XML policy headers don't include parser header because their location and names may differ. 
The programmer must include 
appropriate XML parser header herself before including policy header.

Setting appropriate XML element type for RapidXML NS parser::

  typedef rapidxml_ns::xml_node<> const * xml_element_t;

You can find the full cpp file here: `src/samples/sample01a.cpp <https://github.com/svgpp/svgpp/blob/master/src/samples/sample01a.cpp>`_.

Handling Transformations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Just add ``tag::attribute::transform`` to ``processed_attributes`` list and ``transform_matrix`` method to ``Context`` class::

  void transform_matrix(const boost::array<double, 6> & matrix);

  typedef
    boost::mpl::insert<
      traits::shapes_attributes_by_element,
      tag::attribute::transform
    >::type processed_attributes_t;

  /* ... */

  document_traversal<
    processed_elements<processed_elements_t>,
    processed_attributes<processed_attributes_t>
  >::load_document(xml_root_element, context);

Passed ``matrix`` array ``[a b c d e f]`` correspond to this matrix:

.. image:: http://www.w3.org/TR/SVG11/images/coords/Matrix.png

The :ref:`default <transform-section>` SVG++ behavior is to join all transformations in ``transform`` attribute into single affine transformation matrix.

Source file: `src/samples/sample01b.cpp <https://github.com/svgpp/svgpp/blob/master/src/samples/sample01b.cpp>`_.

Handling Viewports
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The **svg** element may be used inside SVG document to establish a new viewport. 
To process new viewport coordinate system and new user coordinate system 
several attributes must be processed (**x**, **y**, **width**, **height**, **preserveAspectRatio**, **viewbox**).
SVG++ will do it itself if we set ``policy::viewport::as_transform`` :ref:`Viewport Policy <viewport-section>` ::

  document_traversal<
    processed_elements<processed_elements_t>,
    processed_attributes<processed_attributes_t>,
    viewport_policy<policy::viewport::as_transform>
  >::load_document(xml_root_element, context);

we also must append viewport attributes to the list of processed attributes::

  typedef 
    boost::mpl::fold<
      boost::mpl::protect<
        boost::mpl::joint_view<
          traits::shapes_attributes_by_element, 
          traits::viewport_attributes
        >
      >,
      boost::mpl::set<
        tag::attribute::transform
      >::type,
      boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
    >::type processed_attributes_t;

Please note that this cryptic code just merges predefined sequences ``traits::shapes_attributes_by_element``
and ``traits::viewport_attributes`` with ``tag::attribute::transform`` attribute into single MPL sequence
equivalent to the following::

  typedef boost::mpl::set<
    // Transform attribute
    tag::attribute::transform,
    // Viewport attributes
    tag::attribute::x, 
    tag::attribute::y, 
    tag::attribute::width, 
    tag::attribute::height, 
    tag::attribute::viewBox, 
    tag::attribute::preserveAspectRatio,
    // Shape attributes for each shape element
    boost::mpl::pair<tag::element::path,      tag::attribute::d>,
    boost::mpl::pair<tag::element::rect,      tag::attribute::x>,
    boost::mpl::pair<tag::element::rect,      tag::attribute::y>,
    boost::mpl::pair<tag::element::rect,      tag::attribute::width>, 
    boost::mpl::pair<tag::element::rect,      tag::attribute::height>,
    boost::mpl::pair<tag::element::rect,      tag::attribute::rx>,
    boost::mpl::pair<tag::element::rect,      tag::attribute::ry>,
    boost::mpl::pair<tag::element::circle,    tag::attribute::cx>,
    boost::mpl::pair<tag::element::circle,    tag::attribute::cy>,
    boost::mpl::pair<tag::element::circle,    tag::attribute::r>,
    boost::mpl::pair<tag::element::ellipse,   tag::attribute::cx>,
    boost::mpl::pair<tag::element::ellipse,   tag::attribute::cy>,
    boost::mpl::pair<tag::element::ellipse,   tag::attribute::rx>,
    boost::mpl::pair<tag::element::ellipse,   tag::attribute::ry>,
    boost::mpl::pair<tag::element::line,      tag::attribute::x1>,
    boost::mpl::pair<tag::element::line,      tag::attribute::y1>,
    boost::mpl::pair<tag::element::line,      tag::attribute::x2>,
    boost::mpl::pair<tag::element::line,      tag::attribute::y2>,
    boost::mpl::pair<tag::element::polyline,  tag::attribute::points>,
    boost::mpl::pair<tag::element::polygon,   tag::attribute::points>
  >::type processed_attributes_t;

Now SVG++ will call the existing method ``transform_matrix`` to set new user coordinate system.
And we must add some methods that will be passed with information about new viewport::

  void set_viewport(double viewport_x, double viewport_y, double viewport_width, double viewport_height);
  void set_viewbox_size(double viewbox_width, double viewbox_height);
  void disable_rendering();

The full cpp file for this step can be found here: `src/samples/sample01c.cpp <https://github.com/svgpp/svgpp/blob/master/src/samples/sample01c.cpp>`_.

Creating Contexts
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Until now only one instance of context object was used for entire SVG document tree.
It is convenient to create context instance on stack for each SVG element processed. 
This behavior is controlled by *context factories*, passed by :ref:`context_factories <context_factories>` 
parameter of ``document_traversal`` template class.

*Context factories* is a `Metafunction Class`_ that receives parent context type and child element tag as parameters
and returns *context factory* type.

This sample application processes structural elements (**svg** and **g**) and shape elements (**path**, **rect**, **circle** etc).
For the structural elements only **transform** attribute is processed, and for the shape elements - **transform** and attributes 
describing shape. So we can divide ``Context`` context class for ``BaseContext`` and ``ShapeContext`` subclass:

::

  class BaseContext
  {
  public:
    void on_exit_element();
    void transform_matrix(const boost::array<double, 6> & matrix);
    void set_viewport(double viewport_x, double viewport_y, double viewport_width, double viewport_height);
    void set_viewbox_size(double viewbox_width, double viewbox_height);
    void disable_rendering();
  };

  class ShapeContext: public BaseContext
  {
  public:
    ShapeContext(BaseContext const & parent);
    void path_move_to(double x, double y, tag::coordinate::absolute);
    /* ... other path methods ... */
  };

  struct ChildContextFactories
  {
    template<class ParentContext, class ElementTag, class Enable = void>
    struct apply
    {
      // Default definition handles "svg" and "g" elements
      typedef factory::context::on_stack<BaseContext> type;
    };
  };

  // This specialization handles all shape elements (elements from traits::shape_elements sequence)
  template<class ElementTag>
  struct ChildContextFactories::apply<BaseContext, ElementTag,
    typename boost::enable_if<boost::mpl::has_key<traits::shape_elements, ElementTag> >::type>
  {
    typedef factory::context::on_stack<ShapeContext> type;
  };

``factory::context::on_stack<ChildContext>`` factory creates context object ``ChildContext``, passing reference 
to parent context in ``ChildContext`` constructor. 
Lifetime of context object - until processing of element content (child elements and text nodes) is finished. 
``on_exit_element()`` is called right before object destruction.

``ChildContextFactories`` is passed to ``document_traversal``::

  document_traversal<
    /* ... */
    context_factories<ChildContextFactories>
  >::load_document(xml_root_element, context);

Source file: `src/samples/sample01d.cpp <https://github.com/svgpp/svgpp/blob/master/src/samples/sample01d.cpp>`_.


The **use** Element Support
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The **use** element is used to include/draw other SVG element. If **use** references **svg** or
**symbol**, then new viewport and user coordinate system are established.

To add support for **use** in our sample we:

  * Add ``tag::element::use_`` to the list of processed elements, and ``tag::attribute::xlink::href`` to 
    the list of processed attributes  (**x**, **y**, **width** and **height** already included through ``traits::viewport_attributes``).
  * Create context class ``UseContext`` to be used for **use** element, that will
    collect **x**, **y**, **width**, **height** and **xlink:href** attributes values.
  * After processing all **use** element attributes (in method ``UseContext::on_exit_element()``),
    look inside document for element with given **id** and load it with call to 
    ``document_traversal_t::load_referenced_element<...>::load()``.
  * Implement :ref:`Viewport Policy <viewport-section>` requirement - **svg** and **symbol** context
    must have method::

      void get_reference_viewport_size(double & width, double & height);

    that returns size of the viewport set in referenced **use** element. 
    One of possible variant is creation of new context ``ReferencedSymbolOrSvgContext``.

Full implementation is in file: `src/samples/sample01e.cpp <https://github.com/svgpp/svgpp/blob/master/src/samples/sample01e.cpp>`_.

Calculating Marker Positions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

SVG++ may solve complex task of calculating orientations of markers with attribute `orient="auto"`.
Let's set :ref:`Markers Policy <markers-section>` that enables this option::

  document_traversal<
    /* ... */
    markers_policy<policy::markers::calculate_always>
  > /* ... */

Then add *Marker Events* method to ``ShapeContext``::

  void marker(marker_vertex v, double x, double y, double directionality, unsigned marker_index);

The sample (`src/samples/sample01f.cpp <https://github.com/svgpp/svgpp/blob/master/src/samples/sample01f.cpp>`_) just shows how to get marker positions. 
To implement full marker support we also need to process 
**marker**, **marker-start**, **marker-mid** and **marker-end** properties
and process **marker** element (similar to processing of **use** element). 
Demo application may give some idea about this.

Processing of **stroke** and **stroke-width** Properties
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Adding **stroke-width** property processing is trivial - just add
``tag::attribute::stroke_width`` to the list of processed attributes, and add method, 
that receives value, to the context class::

  void set(tag::attribute::stroke_width, double val);

Property **stroke** has complex type *<paint>*::

  <paint>:      none |
                currentColor |
                <color> [<icccolor>] |
                <funciri> [ none | currentColor | <color> [<icccolor>] ] |
                inherit

that is why so many methods are required to receive all possible values of the property::

  void set(tag::attribute::stroke_width, double val);
  void set(tag::attribute::stroke, tag::value::none);
  void set(tag::attribute::stroke, tag::value::currentColor);
  void set(tag::attribute::stroke, color_t color, tag::skip_icc_color = tag::skip_icc_color());
  template<class IRI>
  void set(tag::attribute::stroke tag, IRI const & iri);
  template<class IRI>
  void set(tag::attribute::stroke tag, tag::iri_fragment, IRI const & fragment);
  template<class IRI>
  void set(tag::attribute::stroke tag, IRI const &, tag::value::none val);
  template<class IRI>
  void set(tag::attribute::stroke tag, tag::iri_fragment, IRI const & fragment, tag::value::none val);
  template<class IRI>
  void set(tag::attribute::stroke tag, IRI const &, tag::value::currentColor val);
  template<class IRI>
  void set(tag::attribute::stroke tag, tag::iri_fragment, IRI const & fragment, tag::value::currentColor val);
  template<class IRI>
  void set(tag::attribute::stroke tag, IRI const &, color_t val, tag::skip_icc_color = tag::skip_icc_color());
  template<class IRI>
  void set(tag::attribute::stroke tag, tag::iri_fragment, IRI const & fragment, color_t val, tag::skip_icc_color = tag::skip_icc_color());

Default :ref:`IRI Policy <iri-section>` is used that distinguishes absolute IRIs and local IRI references 
to fragments in same SVG document.

Source code: `src/samples/sample01g.cpp <https://github.com/svgpp/svgpp/blob/master/src/samples/sample01g.cpp>`_.

.. note::
  ``svgpp_parser_impl.cpp`` file was added to the project and a couple of macros was added at the start of the ``sample01g.cpp`` 
  to get around Visual C++ 2015 "compiler out of memory" problem. 
  See :ref:`description <external-parser_section>` of this solution.

Custom Color Factory
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Suppose that default SVG++ color presentation as 8 bit per channel RGB value packed in ``int`` doesn't suit our needs.
We prefer to use some custom type, e.g. ``boost::tuple`` (same as C++11 ``std::tuple``)::

  typedef boost::tuple<unsigned char, unsigned char, unsigned char> color_t;
 
In this case we need our own :ref:`Color Factory <color-section>`, that creates our custom color 
from components values, that was read from SVG::

  struct ColorFactoryBase
  {
    typedef color_t color_type;

    static color_type create(unsigned char r, unsigned char g, unsigned char b)
    {
      return color_t(r, g, b);
    }
  };

  typedef factory::color::percentage_adapter<ColorFactoryBase> ColorFactory;

  document_traversal<
    /* ... */
    color_factory<ColorFactory>
  > /* ... */

Usage of ``factory::color::percentage_adapter`` frees us from implementing 
``create_from_percent`` method in our *Color Factory*.

Source file: `src/samples/sample01h.cpp <https://github.com/svgpp/svgpp/blob/master/src/samples/sample01h.cpp>`_.

Correct Length Handling
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

On next step (`src/samples/sample01i.cpp <https://github.com/svgpp/svgpp/blob/master/src/samples/sample01i.cpp>`_) of sample evolution we will add correct handling of *length*,
that takes in account device resolution (dpi) and changes of viewport size by **svg** and **symbol** elements, 
that affects lengths, which are set in percent. So we:

  * Add to ``BaseContext`` class constructor that receives device resolution in dpi (this constructor
    is only called by ourselves from ``loadSvg`` function).
  * Add ``length_factory_`` field and access function. ``length_factory_`` settings (resolution, viewport size)
    will be passed to child contexts in copy constructor.
  * In ``BaseContext::set_viewport`` and ``BaseContext::set_viewbox_size`` methods pass *viewport* size to the *Length Factory*.
  * Set :ref:`Length Policy <length-section>`, that will ask ``BaseContext`` class for *Length Factory* instance::

      document_traversal<
        /* ... */
        length_policy<policy::length::forward_to_method<BaseContext> >
      > /* ... */;

::

  class BaseContext: public StylableContext
  {
  public:
    BaseContext(double resolutionDPI)
    {
      length_factory_.set_absolute_units_coefficient(resolutionDPI, tag::length_units::in());
    }

    /* ... */

    // Viewport Events Policy
    void set_viewport(double viewport_x, double viewport_y, double viewport_width, double viewport_height)
    {
      length_factory_.set_viewport_size(viewport_width, viewport_height);
    }

    void set_viewbox_size(double viewbox_width, double viewbox_height)
    {
      length_factory_.set_viewport_size(viewbox_width, viewbox_height);
    }

    // Length Policy interface
    typedef factory::length::unitless<> length_factory_type;

    length_factory_type const & length_factory() const
    { return length_factory_; }

  private:
    length_factory_type length_factory_;
  };

According to SVG Specification, the size of the new viewport affects attributes of element 
that establish new viewport (except **x**, **y**, **width** and **height** attributes).
As our *Length Factory* converts lengths to numbers immediately,
we need to pass new viewport size to *Length Factory* before processing other attributes. 
To do so we will use :ref:`get_priority_attributes_by_element <get_priority_attributes_by_element>` parameter of
*Attribute Traversal Policy*::

  struct AttributeTraversal: policy::attribute_traversal::default_policy
  {
    typedef boost::mpl::if_<
      // If element is 'svg' or 'symbol'...
      boost::mpl::has_key<
        boost::mpl::set<
          tag::element::svg,
          tag::element::symbol
        >,
        boost::mpl::_1
      >,
      boost::mpl::vector<
        // ... load viewport-related attributes first ...
        tag::attribute::x, 
        tag::attribute::y, 
        tag::attribute::width, 
        tag::attribute::height, 
        tag::attribute::viewBox, 
        tag::attribute::preserveAspectRatio,
        // ... notify library, that all viewport attributes that are present was loaded.
        // It will result in call to BaseContext::set_viewport and BaseContext::set_viewbox_size
        notify_context<tag::event::after_viewport_attributes>
      >::type,
      boost::mpl::empty_sequence
    > get_priority_attributes_by_element;
  };

  document_traversal<
    /* ... */
    attribute_traversal_policy<AttributeTraversal>
  > /* ... */;

Now we are sure that ``BaseContext::set_viewport`` (and ``BaseContext::set_viewbox_size``) 
will be called before other attributes are processed.


Text Handling
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

On the next step (`src/samples/sample01j.cpp <https://github.com/svgpp/svgpp/blob/master/src/samples/sample01j.cpp>`_) 
we will implement basic handling of **text** elements:

  * Create new ``TextContext`` child of ``BaseContext`` that will receive **text** element data.
    ``set_text`` :ref:`method <text_section>` will be called by SVG++ to pass character data content of element::

      template<class Range>
      void set_text(Range const & text)
      {
        text_content_.append(boost::begin(text), boost::end(text));
      }

  * Add specialization of **ChildContextFactories** class that will create ``TextContext`` class for
    **text** elements (``tag::element::text``).
  * Add ``tag::element::text`` to the list of processed elements ``processed_elements_t``.
