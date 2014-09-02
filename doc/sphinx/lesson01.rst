.. _Associative Sequence: http://www.boost.org/doc/libs/1_56_0/libs/mpl/doc/refmanual/associative-sequence.html
.. _Metafunction Class: http://www.boost.org/doc/libs/1_56_0/libs/mpl/doc/refmanual/metafunction-class.html

Getting started
================

Handling Shapes Geometry
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

SVG++ headers are included like this::

#include <svgpp/svgpp.hpp>

All SVG++ code is placed in ``svgpp`` namespace

Let's start learning SVG++ with simple SVG to `WKT <http://en.wikipedia.org/wiki/Well-known_text>`_ converter::

  #include <svgpp/svgpp.hpp>

  using namespace svgpp;

  class Context
  {
  public:
    void path_move_to(double x, double y, tag::coordinate::absolute)
    { 
    }

    void path_line_to(double x, double y, tag::coordinate::absolute)
    { 
    }

    void path_cubic_bezier_to(
      double x1, double y1, 
      double x2, double y2, 
      double x, double y, 
      tag::coordinate::absolute)
    { 
    }

    void path_quadratic_bezier_to(
      double x1, double y1, 
      double x, double y, 
      tag::coordinate::absolute)
    { 
    }

    void path_elliptical_arc_to(
      double rx, double ry, double x_axis_rotation,
      bool large_arc_flag, bool sweep_flag, 
      double x, double y,
      tag::coordinate::absolute)
    { 
    }

    void path_close_subpath()
    { 
    }

    void path_exit()
    { 
    }

    void on_enter_element(tag::element::any)
    {}

    void on_exit_element()
    {}
  };

  void loadSvg(xml_element_t xml_root_element)
  {
    typedef 
      boost::mpl::insert<
        boost::mpl::insert<
          traits::shape_elements,
          tag::element::svg
        >::type,
        tag::element::g
      >::type processed_elements_t;

    Context context;
    document_traversal<
      processed_elements<processed_elements_t>,
      processed_attributes<traits::shapes_attributes_by_element>
    >::load_document(xml_root_element, context);
  }

Основной способ использования SVG++ - функциям библиотеки передается объект-контекст, библиотека вызывает функции объекта 
и передает им загруженные данные.

Класс ``document_traversal`` содержит только статические функции и выполняет обход XML-SVG дерева, передавая обработку атрибутов 
классам ``attribute_traversal`` and ``attribute_dispatcher``.

В большинстве случаев обрабатывать нужно только некоторое подмножество элементов SVG, для этого мы передаем именованный 
параметр ``processed_elements`` шаблонному классу ``document_traversal``. ``processed_elements`` должен быть моделью 
`Associative Sequence`_ , например ``boost::mpl::set``. 
Мы добавляем к последовательности ``traits::shape_elements`` (enumerates SVG 
`shapes <http://www.w3.org/TR/SVG11/intro.html#TermShape>`_) два structural elements **svg** and **g**.

SVG++ references SVG element types by :ref:`tags <tags-section>`.

Аналогично выбранным для обработки элементам, выбираем подмножество атрибутов и передаем его в параметре 
``processed_attributes``. Именованный параметр ``processed_attributes`` тоже должен быть моделью `Associative Sequence`_, 
но может содержать как тэги атрибутов, так и пары *<element tag, attribute tag>* ``boost::mpl::pair<ElementTag, AttributeTag>``. 
``traits::shapes_attributes_by_element`` содержит атрибуты, определяющие геометрию всех shapes 
({**x**, **y**, **width**, **height**, **rx** and **ry**} for **rect**, {**d**} for **path** etc). 

В этом примере один объект-контекст используется для всех элементов SVG, ``on_enter_element(element_tag)`` вызывается при каждом переходе к дочернему элементу, в качестве аргумента передается тэг типа дочернего элемента. ``on_exit_element`` вызывается при выходе из дочернего объекта.

|XML element|Call to context|
|-----|-----|
|``<svg>``|``on_enter_element(tag::element::svg())``|
|``<rect``|``on_enter_element(tag::element::rect())``|
|``x="100" y="200"``||
|``/>``|``on_exit_element()``|
|``<g>``|``on_enter_element(tag::element::g())``|
|``<rect``|``on_enter_element(tag::element::rect())``|
|``x="300" y="100"``||
|``/>``|``on_exit_element()``|
|``</g>``|``on_exit_element()``|
|``</svg>``|``on_exit_element()``|

Вызовы ``path_XXXX`` except ``path_exit`` соответствуют командам from SVG `path data <http://www.w3.org/TR/SVG11/paths.html#PathData>`_. ``path_exit`` is called after path data attribute was parsed.

|SVG path data command|SVG parameters|SVG++ call to context|
|---------|---------|--------|
|**M** (absolute) or **m** (relative)|(x y)|``path_move_to(x, y, absolute_or_relative_tag())``|

SVG++ по умолчанию использует path-*адаптер*, который:

- Замещает относительные координаты на абсолютные
- Команды рисования ортогональных линий (H, h, V, v) преобразует в вызовы ``path_line_to`` с двумя координатами
- Shorthand/smooth curveto and shorthand/smooth quadratic Bézier curveto replaces with calls with full parameters list

Another adapter in SVG++ converts `Basic shapes <http://www.w3.org/TR/SVG11/shapes.html>`_ to path.

XML Parser
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

We didn't declare ``xml_element_t`` yet. It can be any.
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

Handling Transformation 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Just add ``tag::attribute::transform`` to ``processed_attributes`` list and ``set_transform_matrix`` to ``Context`` class::

  void set_transform_matrix(const boost::array<double, 6> & matrix);

Passed ``matrix`` array ``[a b c d e f]`` correspond to this matrix:

.. image:: http://www.w3.org/TR/SVG11/images/coords/Matrix.png

The default SVG++ behavior is to join all transformations in ``transform`` attribute into single affine transformations matrix.

Creating Contexts
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

До сих пор использовался один экземпляр объекта-контекст для загрузки всех элементов SVG.
Удобно создавать на стеке экземпляр объекта-контекста для каждого встреченного элемента SVG. Это поведение контролируется 
фабриками контекстов, передаваемыми в параметре ``context_factories`` класса ``document_traversal``.

Context factories - это `Metafunction Class`_, принимающий в качестве параметров тип родительского контекста и тэг элемента 
и возвращающий тип фабрики контекста::

  struct ChildContextFactories
  {
    template<class ParentContext, class ElementTag>
    struct apply;
  };

  document_traversal<
    /* ... */
    context_factories<ChildContextFactories>,
    /* ... */
  >::load(/*...*/);

В нашем примере обрабатываются structural elements (**svg** и **g**) и shape elements (**path**, **rect**, **circle** etc).
У structural elements обрабатывается только атрибут **transform**, а у shape elements - и **transform** и attributes 
describing shape::

  class Transformable
  {
  public:
    Transformable(Transformable const & parent);
    void on_exit_element() {}

    void set_transform_matrix(const boost::array<double, 6> & matrix);
  };

  class Shape: public Transformable
  {
  public:
    Shape(Transformable const & parent);

    void path_move_to(double x, double y, tag::coordinate::absolute); 
    /* ... */
  };

  struct ChildContextFactories
  {
    template<class ParentContext, class ElementTag, class Enable = void>
    struct apply
    {
      // Default definition handles "svg" and "g" elements
      typedef factory::context::on_stack<Transformable, Transformable> type;
    };
  };

  // This specialization handles all shape elements (elements from traits::shape_elements sequence)
  template<class ElementTag>
  struct ChildContextFactories::apply<Transformable, ElementTag, 
    typename boost::enable_if<boost::mpl::has_key<traits::shape_elements, ElementTag> >::type>
  {
    typedef factory::context::on_stack<Transformable, Shape> type;
  };

Factory ``factory::context::on_stack<ParentContext, ChildContext>`` создаёт объект контекста для дочернего элемента
типа ``ChildContext``, передавая в конструктор ссылку на родительский контекст. Время жизни контекста - до завершения обработки
element content (child elements and text nodes). ``on_exit_element()`` вызывается перед уничтожением объекта контекста.

