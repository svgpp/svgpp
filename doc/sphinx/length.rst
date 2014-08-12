Length
===============

Length factory определяет какой тип будет соответствовать SVG types `length <http://www.w3.org/TR/SVG/types.html#DataTypeLength>`_ 
and `coordinate <http://www.w3.org/TR/SVG/types.html#DataTypeCoordinate>`_ и как он создается из 
текстовых значений, включающих единицы измерения.

Length Factory Concept
----------------------------

::

  struct length_factory
  {
    typedef /* ... */ length_type;
    typedef /* ... */ number_type;

    length_type create_length(number_type number, tag::length_units::px) const;
    length_type create_length(number_type number, tag::length_units::cm) const;
    /* ... */

    length_type create_length(number_type number, tag::length_units::percent, tag::length_dimension::width) const; 
    length_type create_length(number_type number, tag::length_units::percent, tag::length_dimension::height) const; 
    length_type create_length(number_type number, tag::length_units::percent, tag::length_dimension::not_width_nor_height) const; 
  };

``create_length`` method receives number and length tag and returns corresponding length value of type ``length_type``.

Lengths, заданные в percent units, могут `обрабатываться <http://www.w3.org/TR/SVG/coords.html#Units_viewport_percentage>`_ 
по разному, в зависимости от того длине или ширине соответствует значение. Для этого ``create_length`` передается третий
параметр - один из трёх тэгов ``tag::length_dimension::width``, ``tag::length_dimension::height`` 
или ``tag::length_dimension::not_width_nor_height``.

В зависимости от реализации length factory, length может иметь как численный тип, так и более сложный, например объект, содержащий 
значение и единицы измерения. SVG++ предоставляет ``factory::length::unitless``, реализующий конфигурируемую фабрику, 
возвращающую численные значения.

Unitless Length Factory
--------------------------


Length Policy Concept
--------------------------

Length Policy определяет способ получения экземпляра length factory для конкретного контекста::

  struct length_policy
  {
    typedef /* ... */ context_type;
    typedef /* ... */ length_factory_type;

    static length_factory_type & length_factory(context_type & context);
  };

Length Policy по умолчанию возвращает константную ссылку на общий экземпляр ``factory::length::default_factory`` независимо от
контекста.

Для того чтобы сконфигурировать length factory нужно передать параметр шаблона ``length_policy``. Например,
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