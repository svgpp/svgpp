.. _Associative Sequence: http://www.boost.org/doc/libs/1_55_0/libs/mpl/doc/refmanual/associative-sequence.html

Организация библиотеки
=============================

.. _tags-section:

Tags
-------

В библиотеке SVG++ для обозначения многих сущностей SVG используется концепция тэгов - пустых структур, которые используются in compile time with overload resolution and metaprogramming techniques. 

::

  namespace tag 
  { 
    namespace element
    {
      struct any {};    // Common base for all element tags. Made for convenience
      struct a: any {};
      struct altGlyph: any {};
      // ...
      struct vkern: any {};
    }

    namespace attribute
    {
      struct accent_height {};
      struct accumulate {};
      // ...
      struct zoomAndPan {};

      namespace xlink 
      {
        struct actuate {};
        struct arcrole {};
        // ...
        struct type {};
      }
    }
  }

Каждому элементу SVG соответствует тэг из пространства имен tag::element, а каждому атрибуту SVG - тэг из пространства имен tag::attribute. Атрибутам из XML namespace **xlink** соответствуют тэги in C++ namespace tag::attribute::xlink, а атрибутам из XML namespace **xml** - тэги in C++ namespace tag::attribute::xml. 
Есть и иные тэги, которые описаны в других местах документации.

Traits
-------

Metafunction child_element_types
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Synopsis
"""""""""""

::

  namespace traits
  {
    template<class ElementTag>
    struct child_element_types
    {
      typedef boost::mpl::set<...> type;
    };
  }

Parameter
"""""""""""

``ElementTag`` - element tag

Return type
"""""""""""

`Associative Sequence`_, containing tags of elements that can be childs of element corresponding to ``ElementTag``.


Metafunction element_required_attributes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Synopsis
"""""""""""

::

  namespace traits
  {
    template<class ElementTag>
    struct element_required_attributes
    {
      typedef unspecified type;
    };
  }

Parameter
"""""""""""

``ElementTag`` - element tag

Return type
"""""""""""

Forward Sequence (Boost.MPL), containing tags of attributes that are required for an element corresponding to ``ElementTag``.


В основе разбора SVG лежат функции attribute value parsers, которые преобразуют строковые значения атрибутов SVG в удобные для обработки значения.
Например, значение атрибута x="16mm" может быть преобразовано в соответствующее ``double`` значение, учитывающее единицы измерения, 
а d="M10 10 L15 100" может стать последовательностью вызовов ``path_move_to(10,10); path_line_to(15, 100);``

Адаптеры позволяют преобразовывать значения, полученные от value parsers, в более удобное для программиста представление. 
Например, value parser для *path-data* поддерживает все возможности SVG, включая относительные координаты и shortcuts для горизонтальных и вертикальных линий, адаптер позволяет сократить количество методов, которые должен реализовать пользователь библиотеки, преобразовав такие shortcuts к более общим вызовам.

Экземпляр ``attribute_dispatcher`` создается для обрабатываемого элемента SVG. Он выбирает attribute value parser, соответствующий атрибуту, и вызывает его, кроме того ``attribute_dispatcher`` организует работу *адаптеров*. Например, ``attribute_dispatcher`` для элемента *rect* может собрать значения атрибутов x, y, width, height, rx, ry и преобразовать их в вызовы, соответствующие элементу path.

``attribute_traversal`` перебирает атрибуты SVG элемента, определяет по имени атрибута его внутренний числовой идентификатор, проверяет на наличие обязательных атрибутов.