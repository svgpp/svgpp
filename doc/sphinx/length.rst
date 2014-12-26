.. _length-section:

Length
===============

*Length Factory* defines what type corresponds to SVG types `<length> <http://www.w3.org/TR/SVG/types.html#DataTypeLength>`_ 
and `<coordinate> <http://www.w3.org/TR/SVG/types.html#DataTypeCoordinate>`_ 
and how instances of that types are created from text values that include units.

*Length Policy* specifies how *Length Factory* for the context is accessed. 
It permits configuration of *Length Factory* in runtime (e.g. when viewport size or font size is changed).

Length Factory Concept
----------------------------

::

  struct length_factory
  {
    typedef /* ... */ length_type;
    typedef /* ... */ number_type;

    length_type create_length(number_type number, tag::length_units::em) const;
    length_type create_length(number_type number, tag::length_units::ex) const;
    length_type create_length(number_type number, tag::length_units::px) const;
    length_type create_length(number_type number, tag::length_units::in) const;
    length_type create_length(number_type number, tag::length_units::cm) const;
    length_type create_length(number_type number, tag::length_units::mm) const;
    length_type create_length(number_type number, tag::length_units::pt) const;
    length_type create_length(number_type number, tag::length_units::pc) const;
    length_type create_length(number_type number, tag::length_units::none) const;

    length_type create_length(number_type number, tag::length_units::percent, tag::length_dimension::width) const; 
    length_type create_length(number_type number, tag::length_units::percent, tag::length_dimension::height) const; 
    length_type create_length(number_type number, tag::length_units::percent, tag::length_dimension::not_width_nor_height) const; 
  };

``create_length`` method receives number and length tag and returns corresponding length value of the type ``length_type``.

Lengths that are set with percent units may be treated
`differently <http://www.w3.org/TR/SVG/coords.html#Units_viewport_percentage>`_, 
depending on whether value corresponds to width or height. That is why ``create_length`` receives the third
parameter - one of the three tag types ``tag::length_dimension::width``, ``tag::length_dimension::height`` 
or ``tag::length_dimension::not_width_nor_height``.

Depending on the implementation of *Length Factory*, length may be of integer type 
or something more complex like object containing value and units used. 
SVG++ provides ``factory::length::unitless``, that implements configurable *Length Factory* 
returning numeric values.

Unitless Length Factory
--------------------------

*Unitless Length Factory* ``factory::length::unitless`` - is a model of *Length Factory*, provided by SVG++ library. 
Name "unitless" means that lengths created by factory are numbers without information about units. 
*Unitless Length Factory* uses units information to apply the corresponding coefficient to the value.

::

  template<
    class LengthType = double, 
    class NumberType = double, 
    class ReferenceAbsoluteUnits = tag::length_units::mm
  >
  class unitless
  {
  public:
    /* Skipped Length Factory methods */

    template<class AbsoluteUnits>
    void set_absolute_units_coefficient(NumberType coeff, AbsoluteUnits);
    template<class AbsoluteUnits>
    NumberType get_absolute_units_coefficient(AbsoluteUnits) const;

    void set_user_units_coefficient(NumberType coeff);
    NumberType get_user_units_coefficient() const;

    void set_viewport_size(LengthType width, LengthType height);

    void set_em_coefficient(NumberType coeff);
    void set_ex_coefficient(NumberType coeff);
    template<class UnitsTag>
    void set_em_coefficient(NumberType coeff, UnitsTag unitsTag);
    template<class UnitsTag>
    void set_ex_coefficient(NumberType coeff, UnitsTag unitsTag);
  };

``set_absolute_units_coefficient``
  Sets coefficient that will be used for lengths in absolute units (*in*, *cm*, *mm*, *pt* or *pc*).
  The method should be called for any one of absolute units, coefficients for others will be calculated
  automatically. Example::

    svgpp::factory::length::unitless<> factory;
    // Let our length value be a pixel. Set 'in' coefficient to 90 (90 Dots per inch)
    factory.set_absolute_units_coefficient(90, svgpp::tag::length_units::in());
    // Coefficient for 'pc' (pica = 1/6 inch) will be 90/6 = 15
    assert(factory.get_absolute_units_coefficient(svgpp::tag::length_units::pc()) == 15);

``set_user_units_coefficient``
  Sets coefficient that will be used for lengths in user units (that are specified without units or in *px*). 
  By default coefficient = 1.

``set_viewport_size``
  Sets width and height of the viewport to be used for percentage values that are defined to be relative to the size of viewport.

``set_em_coefficient`` and ``set_ex_coefficient``
  Sets coefficients for font-related *em* and *ex* units.

Length Policy Concept
--------------------------

*Length Policy* defines which *Length Factory* will be used for the context::

  struct length_policy
  {
    typedef /* ... */ length_factory_type;

    static length_factory_type & length_factory(context_type & context);
  };

Default *Length Policy* returns constant reference to static instance of
``factory::length::default_factory``.

:ref:`Named class template parameter <named-params>` for *Length Policy* is ``length_policy``.

To configure *Length Factory* named template parameter ``length_policy`` must be passed to ``document_traversal``. 
For example, using library provided ``policy::length::forward_to_method``::

  typedef factory::length::unitless<> LengthFactory;

  class Context
  {
  public:
    LengthFactory const & length_factory() { return m_LengthFactory; }
    
  private:
    LengthFactory m_LengthFactory;
  };

  document_traversal<
    length_policy<policy::length::forward_to_method<Context, LengthFactory const> >,
    /* ... */
  >::/* ... */