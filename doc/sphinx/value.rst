.. _passing-values:

How parsed values are passed to context
=============================================

To find out how value of the SVG attribute will be passed to context, following algorithm should be applied:

#. If attribute tag is included in :ref:`passthrough_attributes <passthrough_attributes>` sequence, 
   then its value will be passed by `Value Events Policy`_ as a :ref:`string <passing-string>`.
#. If for this element and attribute ``traits::attribute_type<ElementTag, AttributeTag>::type`` is ``tag::type::string``, 
   then value will also be passed by `Value Events Policy`_ as a :ref:`string <passing-string>`.
#. Otherwise type of the attribute should be found in its description in `SVG Specification <http://www.w3.org/TR/SVG/attindex.html>`_.
#. Attribute values of type `<path data> <http://www.w3.org/TR/SVG/paths.html#PathData>`_ 
   (e.g. **d** attribute of **path** element) are described in :ref:`path_section` section.
#. Attribute values of type `<transform list> <http://www.w3.org/TR/SVG/coords.html#TransformAttribute>`_ 
   (e.g. **transform** attribute) are described in :ref:`transform-section` section.
#. All others are passed through `Value Events Policy`_.


Value Events Policy
-----------------------

Value Events Policy Concept
^^^^^^^^^^^^^^^^^^^^^^^^^^^

*Value Events Policy* is a class, containing static ``set`` methods, that receives reference to
context object as a first argument, attribute tag as a second and a source tag as a third. 
Source tag is one of two types: ``tag::source::css`` and ``tag::source::attribute`` (they have common base ``tag::source::any``).
Source tag shows where value comes from -- CSS value in **style** attribute or from separate SVG/XML attribute.
Number and types of other parameters depends on an attribute type.

::

  struct value_events_policy
  {
    static void set(Context & context, AttributeTag tag, SourceTag source, ValueType1 const & value1);
    static void set(Context & context, AttributeTag tag, SourceTag source, ValueType1 const & value1, ValueType2 const & value2);
    /*...*/
  };

``policy::value_events::forward_to_method`` used by default forward calls to ``set`` methods of ``context`` object::

  template<class Context>
  struct forward_to_method
  {
    template<class AttributeTag, class ...Args>
    static void set(Context & context, AttributeTag tag, tag::source::any const &, Args... args)
    {
      context.set(tag, args...);
    }
  };

.. note::

  Source tag is dropped by default *Value Events Policy*, because default 
  :ref:`Attribute Traversal Policy <attribute_traversal_policy>` processes only one value of the same property,
  even if both are provided (``css_hides_presentation_attribute = true``).

.. note::

  Default *Value Events Policy* doesn't pass ``tag::value::inherit`` values of properties and presentation attributes, 
  that are inherited (see ``policy::value_events::skip_inherit``). 
  **inherit** value for such attributes is equivalent to its absence.


Example of default *Value Events Policy* usage::

  #include <svgpp/svgpp.hpp>

  using namespace svgpp;

  struct Context
  {
    void set(tag::attribute::amplitude, double value)
    {
      std::cout << value << "\n";
    }
  };

  void func()
  {
    Context context;
    value_parser<tag::type::number>::parse(tag::attribute::amplitude(), context, std::string("3.14"), tag::source::attribute());
  }

Example of own *Value Events Policy* implementation. 
Created ``policy::value_events::default_policy`` template class specialization 
for our context type (let it be ``boost::optional<double>`` in our example)::

  namespace svgpp { namespace policy { namespace value_events 
  {
    template<>
    struct default_policy<boost::optional<double> >
    {
      template<class AttributeTag>
      static void set(boost::optional<double> & context, AttributeTag tag, tag::source::any, double value) 
      {
        context = value;
      }
    };
  }}}

  void func()
  {
    boost::optional<double> context;
    value_parser<tag::type::number>::parse(tag::attribute::amplitude(), context, std::string("3.14"), tag::source::attribute());
    if (context)
      std::cout << *context << "\n";
  }

.. _Literal Values:

*Literal Values*
  If literal is one of attribute possible values, then this value will cause call with
  tag from ``tag::value`` namespace. Example of attributes that may have literal values::

      gradientUnits = "userSpaceOnUse | objectBoundingBox"
      clip-path = "<funciri> | none | inherit"

  **gradientUnits** is limited to two literal values. **clip-path**, besides
  **none** and **inherit** literal values may get values of *<FuncIRI>* type.

  Example of context implementation, that receives values of **gradientUnits** attributes::

    class GradientContext
    {
    public:
      GradientContext()
        : m_gradientUnitsUserSpace(false)
      {}

      void set(tag::attribute::gradientUnits, tag::value::userSpaceOnUse)
      {
        m_gradientUnitsUserSpace = true;
      }

      void set(tag::attribute::gradientUnits, tag::value::objectBoundingBox)
      {
        m_gradientUnitsUserSpace = false;
      }

    private:
      bool m_gradientUnitsUserSpace;
    };

*<length>* or *<coordinate>*
  Is passed as single argument, whose type is set by :ref:`Length Factory <length-section>` (by default ``double``).

*<IRI>* or *<FuncIRI>*
  See :ref:`iri-section`.
    
*<integer>*
  Single argument of ``int`` type is used.

*<number>* or *<opacity-value>*
  Is passed as single argument of number_type_ (by default ``double``).

*<percentage>*

*<color>* 
  Is passed as single argument, whose type is set by :ref:`Color Factory <color-section>` 
  (by default 8 bit per channel RGB packed in ``int``).

*<color> [<icccolor>]*
  If *<icccolor>* isn't set, then it is passed as single argument,
  whose type is set by :ref:`Color Factory <color-section>`.
  Otherwise, second argument is added, whose type is set by :ref:`ICC Color Factory <icc-color-factory-section>`.
  Example::

    struct Context
    {
      void set(tag::attribute::flood_color, int rgb);
      void set(tag::attribute::flood_color, int rgb, tag::skip_icc_color);
      void set(tag::attribute::flood_color, tag::value::currentColor);
      void set(tag::attribute::flood_color, tag::value::inherit);
    };

*<angle>*
  Is passed as single argument, whose type and value are set by *Angle Factory* 
  (by default ``double`` value in degrees).

*<number-optional-number>*
  Is passed as one or two arguments of number_type_ type (by default ``double``).

*<list-of-numbers>*, *<list-of-lengths>* or *<list-of-points>* 
  Is passed as single argument of unspecified type, which is model of  
  `Boost Single Pass Range <http://www.boost.org/doc/libs/1_57_0/libs/range/doc/html/range/concepts/single_pass_range.html>`_.
  
  *range* items have type:
    * number_type_ (by default ``double``) in case of *<list-of-numbers>*;
    * that is set by :ref:`Length Factory <length-section>` in case of *<list-of-lengths>*;
    * ``std::pair<number_type, number_type>`` (by default ``std::pair<double, double>``) in case of *<list-of-points>*.

  Example::

    struct Context
    {
      template<class Range>
      void set(tag::attribute::kernelMatrix, Range const & range)
      {
        for(typename boost::range_iterator<Range>::type it = boost::begin(range), end = boost::end(range); 
          it != end; ++it)
          std::cout << *it;
      }
    };

  .. note::
    If template function can't be used (e.g. it is virtual function),
    then Boost 
    `any_range <http://www.boost.org/doc/libs/1_57_0/libs/range/doc/html/range/reference/ranges/any_range.html>`_
    can be used as range type instead::

      typedef boost::any_range<double, boost::single_pass_traversal_tag, double const &, std::ptrdiff_t> Range;


*<shape>*
  Is passed as 5 arguments - first is *tag* ``tag::value::rect``, others are of number_type_ type (by default ``double``): 
  ``(tag::value::rect(), top, right, bottom, left)``.

**viewBox** attribute
  Is passed as 4 arguments of number_type_ type (by default ``double``): ``(x, y, width, height)``.

**bbox** attribute
  Is passed as 4 arguments of number_type_ type (by default ``double``): ``(lo_x, lo_y, hi_x, hi_y)``.

**preserveAspectRatio** attribute
  Depending on value is passed as:
    * ``(bool defer, tag::value::none)``
    * ``(bool defer, AlignT align, MeetOrSliceT meetOrSlice)``

      Type ``AlignT`` is one of ``tag::value::xMinYMin``, ``tag::value::xMidYMin``, ``tag::value::xMaxYMin``, 
      ``tag::value::xMinYMid``, ``tag::value::xMidYMid``, ``tag::value::xMaxYMid``, 
      ``tag::value::xMinYMax``, ``tag::value::xMidYMax``, ``tag::value::xMaxYMax``.
      Type ``MeetOrSliceT`` is ``tag::value::meet`` or ``tag::value::slice``.

**text-decoration** property
  **none** and **inherit** values are passed as `Literal Values`_.
  Other values are passed as 8 arguments, 4 of which is of type ``bool``, each of them
  preceded with *tag*, describing argument meaning. Boolean parameters takes ``true`` value
  if corresponding text decoration is set in property::

    struct Context
    {
      void set(tag::attribute::text_decoration, tag::value::none);
      void set(tag::attribute::text_decoration, tag::value::inherit);
      void set(tag::attribute::text_decoration, 
        tag::value::underline,    bool underline,
        tag::value::overline,     bool overline,
        tag::value::line_through, bool line_through,
        tag::value::blink,        bool blink);
    };

**enable-background** property
  **accumulate**, **new** and **inherit** values are passed as `Literal Values`_.
  Values as **new <x> <y> <width> <height>** are passed as 5 arguments, first of them is *tag*, 
  other have type number_type_ (by default ``double``): 
  ``(tag::value::new_(), x, y, width, height)``.

*<paint>*
  Possible combinations of argument types:

    * (``tag::value::inherit``)
    * (``tag::value::none``)
    * (``tag::value::currentColor``)
    * (*<color>*)
    * (*<color>*, *<icccolor>*)
    * (*<iri>*, ``tag::value::none``)
    * (*<iri>*, ``tag::value::currentColor``)
    * (*<iri>*, *<color>*)
    * (*<iri>*, *<color>*, *<icccolor>*)

  Which types corresponds to *<color>* and *<icccolor>* is described above.

  If :ref:`IRI Policy <iri-section>` ``policy::iri::distinguish_local`` is used,
  then number of methods with *<iri>* is doubled:

    * (``tag::value::inherit``)
    * (``tag::value::none``)
    * (``tag::value::currentColor``)
    * (*<color>*)
    * (*<color>*, *<icccolor>*)
    * (*<iri>*, ``tag::value::none``)
    * (``tag::iri_fragment``, *<iri fragment>*, ``tag::value::none``)
    * (*<iri>*, ``tag::value::currentColor``)
    * (``tag::iri_fragment``, *<iri fragment>*, ``tag::value::currentColor``)
    * (*<iri>*, *<color>*)
    * (``tag::iri_fragment``, *<iri fragment>*, *<color>*)
    * (*<iri>*, *<color>*, *<icccolor>*)
    * (``tag::iri_fragment``, *<iri fragment>*, *<color>*, *<icccolor>*)

  Example::

    typedef boost::variant<tag::value::none, tag::value::currentColor, int/* rgba */> SolidPaint;

    struct IRIPaint
    {
      IRIPaint(
        std::string const & fragment, 
        boost::optional<SolidPaint> const & fallback = boost::optional<SolidPaint>());
    };

    typedef boost::variant<SolidPaint, IRIPaint> Paint;

    template<class AttributeTag>
    class PaintContext
    {
    public:
      void set(AttributeTag, tag::value::none)
      { 
        m_paint = tag::value::none(); 
      }

      void set(AttributeTag, tag::value::currentColor)
      { 
        m_paint = tag::value::currentColor(); 
      }

      void set(AttributeTag, int color, tag::skip_icc_color = tag::skip_icc_color())
      { 
        m_paint = color; 
      }

      template<class IRI>
      void set(AttributeTag tag, IRI const & iri)
      { 
        throw std::runtime_error("Non-local references aren't supported");
      }

      template<class IRI>
      void set(AttributeTag tag, tag::iri_fragment, IRI const & fragment)
      { 
        m_paint = IRIPaint(std::string(boost::begin(fragment), boost::end(fragment))); 
      }

      template<class IRI>
      void set(AttributeTag tag, IRI const &, tag::value::none val)
      { 
        // Ignore non-local IRI, fallback to second option
        set(tag, val); 
      }

      template<class IRI>
      void set(AttributeTag tag, tag::iri_fragment, IRI const & fragment, tag::value::none val)
      { 
        m_paint = IRIPaint(std::string(boost::begin(fragment), boost::end(fragment)), boost::optional<SolidPaint>(val)); 
      }

      template<class IRI>
      void set(AttributeTag tag, IRI const &, tag::value::currentColor val)
      { 
        // Ignore non-local IRI, fallback to second option
        set(tag, val); 
      }

      template<class IRI>
      void set(AttributeTag tag, tag::iri_fragment, IRI const & fragment, tag::value::currentColor val)
      { 
        m_paint = IRIPaint(std::string(boost::begin(fragment), boost::end(fragment)), boost::optional<SolidPaint>(val)); 
      }

      template<class IRI>
      void set(AttributeTag tag, IRI const &, agg::rgba8 val, tag::skip_icc_color = tag::skip_icc_color())
      { 
        // Ignore non-local IRI, fallback to second option
        set(tag, val); 
      }

      template<class IRI>
      void set(AttributeTag tag, tag::iri_fragment, IRI const & fragment, int val, tag::skip_icc_color = tag::skip_icc_color())
      { 
        m_paint = IRIPaint(std::string(boost::begin(fragment), boost::end(fragment)), boost::optional<SolidPaint>(val)); 
      }
  
    private:
      Paint m_paint;
    };


