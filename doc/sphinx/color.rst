.. _color-section:

Color
========

*Color Factory* defines what type corresponds to `<color> <http://www.w3.org/TR/SVG/types.html#DataTypeColor>`_ 
SVG type and how values of this type are created from SVG color description.

*ICC Color Factory* defines what type corresponds to 
`<icccolor> <http://www.w3.org/TR/SVG/types.html#DataTypeICCColor>`_ SVG type 
and how values of this type are created from SVG ICC color description.

Color Factory Concept
------------------------

::
  
  struct color_factory
  {
    typedef /* ... */ color_type;
    typedef /* floating point number type */ percentage_type;

    static color_type create(unsigned char r, unsigned char g, unsigned char b);
    static color_type create_from_percent(percentage_type r, percentage_type g, percentage_type b);
  };

``create_from_percent`` is called when color components are set as percents (e.g. **rgb(100%,100%,100%)**).
Percent values are passed as is, i.e. **100%** is passed as ``100``.

In other cases ``create`` function is called with integer component values in range ``0`` to ``255``.
`Recognized color keyword names <http://www.w3.org/TR/SVG/types.html#ColorKeywords>`_ 
are converted to corresponding component values by SVG++ library. 

`System colors <http://www.w3.org/TR/2008/REC-CSS2-20080411/ui.html#system-colors>`_ aren't handled yet.

:ref:`Named class template parameter <named-params>` for *Color Factory* is ``color_factory``.

Integer Color Factory
-------------------------

SVG++ by default uses ``factory::color::integer<>`` as *Color Factory*. 
This factory returns color packed in ``int`` value: Red component in 3rd byte, 
Green in 2nd, Blue in 1st (least significant).
Component offsets and number type can be configured.


.. _icc-color-factory-section:

ICC Color Factory Concept
---------------------------

::

  struct icc_color_factory
  {
    typedef /* floating point number type */ component_type;
    typedef /* ... */ icc_color_type;
    typedef /* ... */ builder_type;

    template<class Iterator>
    void set_profile_name(builder_type &, typename boost::iterator_range<Iterator> const &) const;
    void append_component_value(builder_type &, component_type) const;

    icc_color_type create_icc_color(builder_type const &) const;
  };

``icc_color_type`` is a type that will be passed to the user code. 

``builder_type`` is used as a temporary object during building ``icc_color_type`` 
from color profile name and some components values.

Pseudo-code that illustrates usage of *ICC Color Factory* from inside SVG++
to parse **icc-color(profile1 0.75, 0.15, 0.25)** value::

  void parse_icc(icc_color_factory const & factory)
  {
    icc_color_factory::builder_type builder;
    factory.set_profile_name(builder, boost::as_literal("profile1"));
    factory.append_component_value(builder, 0.75);
    factory.append_component_value(builder, 0.15);
    factory.append_component_value(builder, 0.25);
    value_events_policy::set(context, attribute_tag, factory.create_icc_color(builder));
  }


ICC Color Policy Concept
---------------------------

*ICC Color Policy* defines which *ICC Color Factory* instance is used for the context. 
It can be used to configure *ICC Color Factory* in runtime, e.g. taking in account referenced color profiles in SVG document.

::

  struct icc_color_policy
  {
    typedef /* ... */ context_type;
    typedef /* ... */ icc_color_factory_type;

    static icc_color_factory_type & icc_color_factory(Context const &);
  };
  
Default *ICC Color Policy* returns static instance of ``factory::icc_color::stub``.
``factory::icc_color::stub`` is a model of *ICC Color Factory* that skips passed values 
and returns instance of empty ``tag::skip_icc_color`` class as ICC color.

:ref:`Named class template parameter <named-params>` for *ICC Color Policy* is ``icc_color_policy``.