.. _length-section:

Length
===============

*Length Factory* определяет какой тип будет соответствовать SVG types `<length> <http://www.w3.org/TR/SVG/types.html#DataTypeLength>`_ 
and `<coordinate> <http://www.w3.org/TR/SVG/types.html#DataTypeCoordinate>`_ и как он создается из 
текстовых значений, включающих единицы измерения.

*Length Policy* определяет способ получения экземпляра *Length Factory* для конкретного контекста. Это позволяет 
конфигурировать *Length Factory* in runtime.

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

``create_length`` method receives number and length tag and returns corresponding length value of type ``length_type``.

Lengths, заданные в percent units, могут обрабатываться 
`по разному <http://www.w3.org/TR/SVG/coords.html#Units_viewport_percentage>`_, 
в зависимости от того, длине или ширине соответствует значение. Для этого ``create_length`` передается третий
параметр - один из трёх тэгов ``tag::length_dimension::width``, ``tag::length_dimension::height`` 
или ``tag::length_dimension::not_width_nor_height``.

В зависимости от реализации *Length Factory*, length может иметь как численный тип, так и более сложный, например объект, содержащий 
значение и единицы измерения. SVG++ предоставляет ``factory::length::unitless``, реализующий конфигурируемую фабрику, 
возвращающую численные значения.

Unitless Length Factory
--------------------------

*Unitless Length Factory* ``factory::length::unitless`` - это model of *Length Factory*, предоставляемая SVG++ library. 
Unitless в названии означает,
что lengths, создаваемые фабрикой, просто числа, не имеющие размерности. Информация о единицах измерения 
используется *Unitless Length Factory* для выбора коэффициента.

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
  Позволяет задать коэффициент, на который будет умножаться значение, заданное в абсолютных единицах измерения
  (*in*, *cm*, *mm*, *pt* или *pc*), для получения длины.
  Метод достаточно вызвать для любой абсолютной единицы измерения, коэффициенты для остальных абсолютных единиц будут вычисленны
  автоматически. Например::

    svgpp::factory::length::unitless<> factory;
    // Let our length value be a pixel. Set 'in' coefficient to 90 (90 Dots per inch)
    factory.set_absolute_units_coefficient(90, svgpp::tag::length_units::in());
    // Coefficient for 'pc' (pica = 1/6 inch) will be 90/6 = 15
    assert(factory.get_absolute_units_coefficient(svgpp::tag::length_units::pc()) == 15);

``set_user_units_coefficient``
  Позволяет задать коэффициент, на который будет умножаться значение, заданное в user units (заданное без единиц измерения
  или в *px*). По умолчанию коэффициент = 1.

``set_viewport_size``
  Задает ширину и высоту viewport для преобразования percentage values that are defined to be relative to the size of viewport.

``set_em_coefficient`` и ``set_ex_coefficient``
  Позволяют задать коэффициенты для единиц измерения *em* и *ex*, учитывающих размер выбранного фонта.

Length Policy Concept
--------------------------

*Length Policy* определяет способ получения экземпляра *Length Factory* для конкретного контекста::

  struct length_policy
  {
    typedef /* ... */ length_factory_type;

    static length_factory_type & length_factory(context_type & context);
  };

*Length Policy* по умолчанию возвращает константную ссылку на общий экземпляр ``factory::length::default_factory`` независимо от
контекста.

:ref:`Named class template parameter <named-params>` for *Length Policy* is ``length_policy``.

Для того чтобы сконфигурировать *Length Factory* для ``document_traversal``, нужно передать параметр шаблона ``length_policy``. Например,
``policy::length::forward_to_method``::

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