.. _color-section:

Color
========

*Color Factory* определяет какой тип будет соответствовать SVG type `<color> <http://www.w3.org/TR/SVG/types.html#DataTypeColor>`_ 
и как он создается из текстового описания цвета.

*ICC Color Factory* определяет какой тип будет соответствовать SVG type 
`<icccolor> <http://www.w3.org/TR/SVG/types.html#DataTypeICCColor>`_ и как он создается из текстового описания цвета.

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

``create_from_percent`` вызывается в случае, когда компоненты цвета заданы процентами (например, **rgb(100%,100%,100%)**).
Значения процентов передаются без коэффициента, **100%** передается как ``100``.

В остальных случаях вызывается функция ``create``, значения компонентов передаются как целые числа от ``0`` до ``255``.
`Recognized color keyword names <http://www.w3.org/TR/SVG/types.html#ColorKeywords>`_ преобразуются в соответствующие значения
компонентов SVG++ library. 

`System colors <http://www.w3.org/TR/2008/REC-CSS2-20080411/ui.html#system-colors>`_ библиотекой пока не обрабатываются.

Integer Color Factory
-------------------------


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

``icc_color_type`` - это тип, который будет передан в user code. 

``builder_type`` используется как временный объект для создания ``icc_color_type`` из имени цветового профиля 
и некоторого количества значений компонентов.

Иллюстрация последовательности вызовов методов *ICC Color Factory* на примере разбора 
значения **icc-color(profile1 0.75, 0.15, 0.25)**::

  void parse_icc(icc_color_factory const & factory)
  {
    icc_color_factory::builder_type builder;
    factory.set_profile_name(builder, boost::as_literal("profile1"));
    factory.append_component_value(builder, 0.75);
    factory.append_component_value(builder, 0.15);
    factory.append_component_value(builder, 0.25);
    set_color(factory.create_icc_color(builder));
  }


ICC Color Policy Concept
---------------------------

*ICC Color Policy* определяет способ получения экземпляра *ICC Color Factory* для конкретного контекста. Это позволяет 
учитывать состояние контекста (например, referenced color profiles) при создании цвета.

::

  struct icc_color_policy
  {
    typedef /* ... */ context_type;
    typedef /* ... */ icc_color_factory_type;

    static icc_color_factory_type & icc_color_factory(Context const &);
  };
  
*ICC Color Policy* по умолчанию возвращает статический экземпляр ``factory::icc_color::stub``.
``factory::icc_color::stub`` - model of *ICC Color Factory*, игнорирующая передаваемые фабрике значения и 
возвращающая в качестве экземпляра ICC color пустой класс ``tag::skip_icc_color``.