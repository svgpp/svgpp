.. _Associative Sequence: http://www.boost.org/doc/libs/1_57_0/libs/mpl/doc/refmanual/associative-sequence.html
.. _Metafunction Class: http://www.boost.org/doc/libs/1_57_0/libs/mpl/doc/refmanual/metafunction-class.html

Tutorial
================

Начнем знакомство с SVG++ с примера, в который будем последовательно добавлять использование возможностей библиотеки.


All SVG++ headers may be included through this one::

#include <svgpp/svgpp.hpp>

All SVG++ code is placed in ``svgpp`` namespace. We'll import entire namespace in our sample.

.. _tutorial:

Handling Shapes Geometry
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Основной способ использования SVG++ - функциям библиотеки передается объект-контекст, библиотека вызывает функции объекта 
и передает им загруженные данные. 

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
    boost::mpl::fold<
      traits::shape_elements,
      boost::mpl::set<
        tag::element::svg,
        tag::element::g
      >::type,
      boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
    >::type processed_elements_t;

  void loadSvg(xml_element_t xml_root_element)
  {
    Context context;
    document_traversal<
      processed_elements<processed_elements_t>,
      processed_attributes<traits::shapes_attributes_by_element>
    >::load_document(xml_root_element, context);
  }

Класс :ref:`document_traversal <document_traversal>` - это фасад, предоставляющий доступ к большинству возможностей библиотеки.

В большинстве случаев обрабатывать нужно только некоторое подмножество элементов SVG, для этого мы передаем 
:ref:`именованный параметр <named-params>` :ref:`processed_elements <processed_elements>` 
шаблонному классу ``document_traversal``. В нашем случае ``processed_elements_t`` 
это ``boost::mpl::set``, объединяющий последовательность ``traits::shape_elements`` (enumerates SVG 
`shapes <http://www.w3.org/TR/SVG11/intro.html#TermShape>`_) и два structural elements **svg** and **g**.

SVG++ references SVG element types by :ref:`tags <tags-section>`.

Аналогично выбранным для обработки элементам, выбираем подмножество атрибутов и передаем его в параметре 
:ref:`processed_attributes <processed_attributes>`.  
``traits::shapes_attributes_by_element`` содержит атрибуты, определяющие геометрию всех shapes 
({**x**, **y**, **width**, **height**, **rx** and **ry**} for **rect**, {**d**} for **path** etc). 

В этом примере один объект-контекст используется для всех элементов SVG, 
``on_enter_element(element_tag)`` вызывается при каждом переходе к дочернему элементу, в качестве аргумента 
передается тэг типа дочернего элемента. ``on_exit_element()`` вызывается при выходе из дочернего объекта:

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

Вызовы ``path_XXXX`` except ``path_exit`` соответствуют командам from SVG 
`path data <http://www.w3.org/TR/SVG11/paths.html#PathData>`_. 
``path_exit`` is called after path data attribute was parsed.

SVG++ по умолчанию использует :ref:`Path Policy <path_policy>`, который:

- Замещает относительные координаты на абсолютные
- Команды рисования ортогональных линий (H, h, V, v) преобразует в вызовы ``path_line_to`` с двумя координатами
- Shorthand/smooth curveto and shorthand/smooth quadratic Bézier curveto replaces with calls with full parameters list

:ref:`Basic Shapes Policy <basic_shapes>` по умолчанию converts 
`basic shapes <http://www.w3.org/TR/SVG11/shapes.html>`_ to path.

Выбор XML парсера
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

We didn't declared ``xml_element_t`` yet. Это тип, соответствующий :ref:`типу <xml_policy_types>` элемента в выбранном XML парсере.
Let's use `RapidXML NS <https://github.com/svgpp/rapidxml_ns>`_ library (it is a clone of 
`RapidXML <http://rapidxml.sourceforge.net/>`_ with namespace handling added) that comes with SVG++ 
in ``third_party/rapidxml_ns/rapidxml_ns.hpp`` file. It's a single header library, so we just need to point to its header::

  #include <rapidxml_ns/rapidxml_ns.hpp>

Then we must include *policy* for XML parser chosen::

  #include <svgpp/policy/xml/rapidxml_ns.hpp>

XML policies headers don't include parser header because their location and names may differ. Programmer must include 
appropriate XML parser header herself before including policy header.

Setting appropriate XML element type for RapidXML NS parser::

  typedef rapidxml_ns::xml_node<> const * xml_element_t;

Полный файл с примером можно увидеть здесь ``src/samples/sample01a.cpp``.

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

Файл с примером ``src/samples/sample01b.cpp``.

Handling Viewports
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Элементы **svg** могут быть использованы внутри документа для establishing new viewport. 
Для того чтобы обработать new viewport coordinate system, a new user coordinate system 
нужно обработать несколько атрибутов (**x**, **y**, **width**, **height**, **preserveAspectRatio**, **viewbox**).
SVG++ может взять это на себя.

Мы задаем :ref:`Viewport Policy <viewport-section>` ``policy::viewport::as_transform``::

  document_traversal<
    processed_elements<processed_elements_t>,
    processed_attributes<processed_attributes_t>,
    viewport_policy<policy::viewport::as_transform>
  >::load_document(xml_root_element, context);

и добавляем viewport attributes к списку обрабатываемых::

  typedef 
    boost::mpl::fold<
      boost::mpl::protect<
        boost::mpl::joint_view<
          svgpp::traits::shapes_attributes_by_element, 
          svgpp::traits::viewport_attributes
        >
      >,
      boost::mpl::set<
        tag::attribute::transform
      >::type,
      boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
    >::type processed_attributes_t;

Теперь SVG++ будет вызывать уже добавленный метод ``transform_matrix`` для установки new user coordinate system.
И нам остается добавить метод ``set_viewport``, через который будет передаваться информация о новом viewport::

  void set_viewport(double viewport_x, double viewport_y, double viewport_width, double viewport_height);

Файл с примером ``src/samples/sample01c.cpp``.

Creating Contexts
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

До сих пор использовался один экземпляр объекта-контекст для загрузки всех элементов SVG.
Удобно создавать на стеке экземпляр объекта-контекста для каждого встреченного элемента SVG. Это поведение контролируется 
фабриками контекстов, передаваемыми в параметре :ref:`context_factories <context_factories>` класса ``document_traversal``.

*Context factories* - это `Metafunction Class`_, принимающий в качестве параметров тип родительского контекста и тэг элемента 
и возвращающий тип фабрики контекста.

В нашем примере обрабатываются structural elements (**svg** и **g**) и shape elements (**path**, **rect**, **circle** etc).
У structural elements обрабатывается только атрибут **transform**, а у shape elements - и **transform** и attributes 
describing shape. Мы можем разделить класс ``Context`` на ``BaseContext`` и ``ShapeContext``:

::

  class BaseContext
  {
  public:
    void on_exit_element();
    void transform_matrix(const boost::array<double, 6> & matrix);
    void set_viewport(double viewport_x, double viewport_y, double viewport_width, double viewport_height);
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

Factory ``factory::context::on_stack<ChildContext>`` создаёт объект контекста для дочернего элемента
типа ``ChildContext``, передавая в конструктор ссылку на родительский контекст. Время жизни контекста - до завершения обработки
element content (child elements and text nodes). ``on_exit_element()`` вызывается перед уничтожением объекта контекста.

И передаем ``ChildContextFactories`` параметром ``document_traversal``::

  document_traversal<
    /* ... */
    context_factories<ChildContextFactories>
  >::load_document(xml_root_element, context);

Файл с примером ``src/samples/sample01d.cpp``.


Поддержка элемента **use**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Элемент **use** позволяет ссылаться на другие элементы внутри документа SVG. Если **use** ссылается на **svg** или
**symbol** устанавливается новый viewport и новая система координат.

Для добавления поддержки **use** в наш пример мы:

  * Добавим ``tag::element::use_`` в список обрабатываемых элементов, а ``tag::attribute::xlink::href`` в 
    список обрабатываемых атрибутов (**x**, **y**, **width** и **height** уже входят в ``traits::viewport_attributes``).
  * Создадим класс контекста ``UseContext`` для элемента **use**, который будет собирать значения атрибутов 
    **x**, **y**, **width**, **height** и **xlink:href**.
  * После загрузки всех атрибутов элемента **use** (в методе ``UseContext::on_exit_element()``)
    найдем в документе элемент с заданным **id** и загрузим его вызовом
    ``document_traversal_t::load_referenced_element<...>::load()``.
  * Для корректной работы :ref:`Viewport Policy <viewport-section>` мы должны реализовать в контекстах **svg** и **symbol**
    метод::

      void get_reference_viewport_size(double & width, double & height);

    возвращающий размер viewport, заданный ссылающимся элементом **use**. 
    Одним из вариантов будет создание для этого нового контекста ``ReferencedSymbolOrSvgContext``.

Реализация в файле ``src/samples/sample01e.cpp``.

Определение позиций маркеров
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

SVG++ может решать нетривиальную задачу определения направления маркеров, заданных с атрибутом `orient="auto"`.

Задаем :ref:`Markers Policy <markers-section>`, включающий автоматический расчет позиций маркеров::

  document_traversal<
    /* ... */
    markers_policy<policy::markers::calculate_always>
  > /* ... */

И добавляем обработчик *Marker Events* в ``ShapeContext``::

  void marker(marker_vertex v, double x, double y, double directionality, unsigned marker_index);

В нашем примере (``src/samples/sample01f.cpp``) мы ограничились получением списка маркеров с координатами и углами.
Для полной поддержки маркеров надо добавить обработку **marker**, **marker-start**, **marker-mid** и **marker-end** properties
и обработку элементов **marker** (во многом аналогично обработке элементов **use**).

Обработка **stroke** и **stroke-width** properties
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Обработка **stroke-width** property реализуется тривиально - 
``tag::attribute::stroke_width`` добавляется в список обрабатываемых атрибутов, а в класс контекста добавляется метод,
принимающий значение::

  void set(tag::attribute::stroke_width, double val);

Property **stroke** имеет сложный тип *<paint>*::

  <paint>:      none |
                currentColor |
                <color> [<icccolor>] |
                <funciri> [ none | currentColor | <color> [<icccolor>] ] |
                inherit

поэтому и число методов, принимающих возможные значения этого property велико::

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

Здесь используется :ref:`IRI Policy <iri-section>` по умолчанию, 
которое различает абсолютные IRI и локальные IRI ссылки на фрагменты внутри документа.

Файл с примером ``src/samples/sample01g.cpp``.

Custom Color Factory
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Предположим, что нас не устраивает представление цвета в виде упакованных в ``int`` байтовых значений компонентов,
предоставляемое по умолчанию SVG++, а мы хотим хранить компоненты цвета в ``boost::tuple``::

  typedef boost::tuple<unsigned char, unsigned char, unsigned char> color_t;
 
В этом случае нам надо задать собственную :ref:`Color Factory <color-section>`, создающую выбранный нами тип цвета из значений
компонентов, прочитанных из SVG::

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

Использование ``factory::color::percentage_adapter`` избавляет нас от необходимости реализовывать метод
``create_from_percent`` в нашей *Color Factory*.

Файл с примером ``src/samples/sample01h.cpp``.

Correct Length Handling
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

На следующем шаге развития нашего примера (``src/samples/sample01h.cpp``) мы добавим корректную обработку *length*,
учитывающую разрешение (dpi) устройства и изменение размера viewport элементами **svg** и **symbol**, которое
влияет на значения длины, заданные в процентах. Для этого мы:

  * Добавим ``BaseContext`` конструктор, в который будем передавать разрешение устройства в dpi.
  * Добавим ``length_factory_`` field and access function. Настройки ``length_factory_`` (resolution, viewport size)
    будут передаваться контекстам дочерних элементов в конструкторе копирования.
  * В реализации ``set_viewport`` добавим передачу размера *viewport* в *Length Factory*.
  * Зададим :ref:`Length Policy <length-section>`, запрашивающий *Length Factory* у контекста::

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

    // Length Policy interface
    typedef factory::length::unitless<> length_factory_type;

    length_factory_type const & length_factory() const
    { return length_factory_; }

  private:
    length_factory_type length_factory_;
  };

В соответствии со спецификацией SVG, размеры нового viewport должны учитываться в атрибутах элемента,
который establish new viewport (кроме атрибутов **x**, **y**, **width** и **height**).
Так как в выбранной нами стратегии *Length Factory* преобразует проценты в число немедленно,
нам нужно передать размер нового viewport *Length Factory* до обработки остальных атрибутов. 
Для этого воспользуемся параметром :ref:`get_priority_attributes_by_element <get_priority_attributes_by_element>`
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
        // It will result in call to BaseContext::set_viewport
        notify_context<tag::event::after_viewport_attributes>
      >::type,
      boost::mpl::empty_sequence
    > get_priority_attributes_by_element;
  };

  document_traversal<
    /* ... */
    attribute_traversal_policy<AttributeTraversal>
  > /* ... */;

Теперь мы уверены, что ``BaseContext::set_viewport`` будет вызван до обработки остальных атрибутов.