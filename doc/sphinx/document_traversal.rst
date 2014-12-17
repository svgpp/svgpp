.. _Associative Sequence: http://www.boost.org/doc/libs/1_55_0/libs/mpl/doc/refmanual/associative-sequence.html

Document Traversal
======================

.. _document_traversal:

document_traversal Class
-----------------------------

Шаблонный класс ``document_traversal`` - это фасад, предоставляющий доступ к большинству возможностей библиотеки.

``document_traversal`` включает только статические методы.
Все методы ``document_traversal`` принимают *context* и *XML element*.
*Context* - это пользовательский объект, который будет принимать обработанные данные.
*XML element* - это тип, соответствующий :ref:`типу <xml_policy_types>` XML элемента в выбранном XML парсере.

:ref:`Именованные параметры <named-params>` шаблонного класса ``document_traversal`` позволяют настраивать
все аспекты обработки SVG.

::

  template<class Args...>
  struct document_traversal
  {
    template<class XMLElement, class Context>
    static bool load_document(XMLElement const & xml_root_element, Context & context)
      { return load_expected_element(xml_root_element, context, tag::element::svg()); }

    template<class XMLElement, class Context, class ElementTag>
    static bool load_expected_element(
      XMLElement const & xml_element, 
      Context & context, 
      ElementTag expected_element);

    template<class RefArgs...>
    struct load_referenced_element
    {
      template<class XMLElement, class Context>
      static bool load(XMLElement const & xml_element, Context & parent_context);
    };
  };

Методы
""""""""""""

::

  template<class XMLElement, class Context>
  static bool load_document(XMLElement const & xml_root_element, Context & context);

.. _fakeRef3:

  ``load_document`` - это shortcut для ``load_expected_element``, принимающий корневой элемент SVG документа (**svg**)
  в качестве параметра ``xml_root_element``.

::

  template<class XMLElement, class Context, class ElementTag>
  static bool load_expected_element(
    XMLElement const & xml_element, 
    Context & context, 
    ElementTag expected_element);

.. _fakeRef4:

  ``load_expected_element`` загружает ``xml_element`` и его потомков, в соответствии с конфигурацией, передавая
  загруженные данные в ``context``. 

  ``load_expected_element`` ожидает, что имя элемента ``xml_element`` соответствует ``ElementTag``.
  Если нет - выбрасывается исключение ``unexpected_element_error`` (см. :ref:`Error Policy <error-handling>`).

::

  template<class RefArgs...>
  template<class XMLElement, class Context>
  static bool load_referenced_element<RefArgs...>::load(
    XMLElement const & xml_element, 
    Context & parent_context);

.. _fakeRef5:

  ``load_referenced_element`` главным образом используется для загрузки элементов SVG, на которые ссылается другой
  элемент SVG, например, элемент **use** или *gradients*. 
  В отличие от ``load_expected_element``, допустимый тип ``xml_element``, не ограничен одним элементом, а задается
  последовательностью ``expected_elements``. 

  Named class template parameters of ``document_traversal::load_referenced_element``

  ``expected_elements`` *(required)*
    Значение параметра - `Associative Sequence`_.
    Содержит список тэгов ожидаемых типов элемента ``xml_element``.
    Если тип ``xml_element`` не соответствует одному из списка  - выбрасывается исключение 
    ``unexpected_element_error`` (см. :ref:`Error Policy <error-handling>`).

    .. note::

      ``traits::reusable_elements`` содержит список элементов, на которые может ссылаться **use**.

.. _referencing_element:

  ``referencing_element`` *(optional)*
    Значение параметра - тэг элемента, который содержит ссылку на загружаемый элемент.
    Используется только если *Viewport Policy* требует обработки viewport библиотекой 
    (для того, чтобы корректно обрабатывать элементы **svg** и **symbols**, на которые
    ссылаются **image** или **use**).

  ``processed_elements`` или ``ignored_elements`` *(optional)*
    Не более одного из двух может быть задано. 
    :ref:`См. описание <processed_elements>` одноименных параметров ``document_traversal``.
    Позволяет для переданного элемента переопределить значения ``processed_elements``/``ignored_elements``, заданные
    для ``document_traversal``. Дочерние элементы будут обработаны с настройками ``document_traversal``.


document_traversal Named Class Template Parameters
--------------------------------------------------------

.. _processed_elements:

  ``ignored_elements`` and ``processed_elements``
    Один из этих параметров должен быть задан, чтобы определить какие элементы SVG обрабатываются. 
    Значение параметра - `Associative Sequence`_ (например ``boost::mpl::set``),
    содержащий тэги элементов.

    Если задан ``processed_elements``, то обработка ограничивается только перечисленными элементами,
    если ``ignored_elements``, то обрабатываются все элементы, кроме перечисленных.

.. _processed_attributes:

  ``ignored_attributes`` and ``processed_attributes``
    Один из этих параметров должен быть задан, чтобы определить какие атрибуты SVG обрабатываются. 
    Значение параметра - `Associative Sequence`_ (например ``boost::mpl::set``),
    содержащий тэги атрибутов. Кроме тэгов атрибутов он может содержать пары <тэг элемента, тэг атрибута>
    в таком виде ``boost::mpl::pair<tag::element::g, tag::attribute::transform>``, в этом случае
    проверка проходит если и атрибут и элемент соответствуют обрабатывемому. 

    Если задан ``processed_attributes``, то обработка ограничивается только перечисленными атрибутами,
    если ``ignored_attributes``, то обрабатываются все атрибуты, кроме перечисленных.  

.. _passthrough_attributes:

  ``passthrough_attributes`` *(optional)*
    Значение параметра - `Associative Sequence`_ (например ``boost::mpl::set``),
    содержащий тэги атрибутов. Значения перечисленных атрибутов не обрабатываются SVG++,
    а передаются в пользовательский код в виде :ref:`строки <passing-string>`.

  ``context_factories`` *(optional)*
    См. :ref:`context_factories`.

  ``attribute_traversal_policy`` *(optional)*
    См. :ref:`attribute_traversal_policy`.

.. _context_factories:

Context Factories
--------------------------------

При обходе дерева SVG, для каждого встреченного элемента выбирается контекст, который будет использоваться
для передачи значений атрибутов и character data посредством соответствующего *event policy*. 
Для конфигурации этого поведения используется параметр ``context_factories``.

Параметр ``context_factories`` содержит metafunction class, принимающий два параметра: 

  - ``ParentContext`` - тип родительского контекста (контекст, использованный для parent SVG element);
  - ``ElementTag`` - тэг элемента (соответствует типу встреченного элемента),
    и возвращающий тип *Context Factory*.

::

  typedef 
    typename context_factories::template apply<ParentContext, ElementTag>::type 
      selected_context_factory;

SVG++ предоставляет несколько готовых *Context Factory*::

  template<class ParentContext, class ElementTag>
  class factory::context::same;

.. _fakeRef1:

  Новый контекст для элемента не создается, будет использован parent context. У parent context будут вызваны 
  методы ``on_enter_element(ElementTag())`` и ``on_exit_element()`` в начале и в конце обработки элемента соответственно.

::

  template<class ChildContext>
  class factory::context::on_stack;

.. _fakeRef2:

  На стеке создается объект контекста типа ``ChildContext``. В конструктор передается один параметр - reference 
  to parent context. После окончания обработки объекта перед вызовом деструктора
  вызывается метод ``ChildContext::on_exit_element()`` этого объекта.
  Время жизни объекта ограничено временем обработки атрибутов элемента, всех дочерних элементов и текста.

  ``factory::context::on_stack_with_xml_element`` то же самое что ``factory::context::on_stack``, но в конструктор 
  ``ChildContext`` вторым параметром передается объект, соответствующий элементу XML. Его тип зависит от используемого
  XML парсера.

::
  
  template<
    class ElementTag, 
    class ChildContextPtr, 
    class ChildContext = typename boost::pointee<ChildContextPtr>::type 
  >
  class get_ptr_from_parent;

.. _fakeRef7:

  Для получения контекста вызывается метод ``get_child_context(ElementTag())`` родительского контекста, который
  должен вернуть указатель типа ``ChildContextPtr`` на контекст дочернего объекта. Указатель может быть обычным или
  smart указателем. После окончания обработки дочернего объекта вызывается метод ``ChildContext::on_exit_element()`` этого объекта.


.. _attribute_traversal_policy:

Attribute Traversal Policy
-----------------------------------

*Attribute Traversal Policy* настраивает порядок обработки SVG attributes, а также CSS properties in **style** attribute.

::

  struct attribute_traversal_policy_concept
  {
    static const bool parse_style = /* true or false */;
    static const bool css_hides_presentation_attribute = /* true or false */;

    typedef /* Metafunction class */ get_priority_attributes_by_element;
    typedef /* Metafunction class */ get_deferred_attributes_by_element;
    typedef /* Metafunction class */ get_required_attributes_by_element;
  };

.. _parse_style:

``parse_style = true``
  Содержимое атрибута **style** обрабатывается как последовательность semicolon-separated пар свойство-значение.

``css_hides_presentation_attribute = true``
  Проверяется только если ``parse_style = true``. 
  
  Если одно свойство задано как в **style**, так и соответствующим
  *presentation attribute*, то будет обработано только значение свойства в атрибуте **style** (оно имеет больший
  приоритет в соответствии со спецификацией SVG).

  Если ``css_hides_presentation_attribute = false``, то используется меньше памяти, но могут приходить 
  значения одного свойства и из **style** и из *presentation attribute* в произвольном порядке.

.. _get_priority_attributes_by_element:

``get_priority_attributes_by_element``, ``get_deferred_attributes_by_element`` and ``get_required_attributes_by_element``
  `Metafunction class <http://www.boost.org/doc/libs/1_57_0/libs/mpl/doc/refmanual/metafunction-class.html>`_,
  принимающий тэг элемента и
  возвращающий `Forward Sequence <http://www.boost.org/doc/libs/1_57_0/libs/mpl/doc/refmanual/forward-sequence.html>`_.
  Элементы возвращаемой последовательности - тэги атрибутов.

  Атрибуты, возвращаемые метафункцией ``get_priority_attributes_by_element`` для данного элемента, будут обработаны раньше других 
  атрибутов именно в том порядке, в каком они перечислены в последовательности.

  Атрибуты, возвращаемые метафункцией ``get_deferred_attributes_by_element`` для данного элемента, будут обработаны 
  после всех других атрибутов именно в том порядке, в каком они перечислены в последовательности.

  .. note::

    Последовательности, возвращаемые  ``get_priority_attributes_by_element`` и ``get_deferred_attributes_by_element``, 
    кроме тэгов атрибутов могут содержать элементы вида ``notify_context<EventTag>``. ``EventTag`` - это 
    произвольный тип-тэг, который будет передан методу ``notify`` контекста. 
    ``notify(EventTag())`` будет вызван в тот момент, когда все атрибуты, идущие в последовательности
    перед ним, уже обработаны.

  Отсутствие какого-либо атрибута из последовательности, возвращаемой метафункцией ``get_required_attributes_by_element`` для данного элемента, 
  считается ошибкой и будет обработано в соотвествии с заданным *Error Policy*.
  В SVG++ определена метафункция ``traits::element_required_attributes``, возвращающая обязательные атрибуты, в соответствии со
  спецификацией SVG. Ее можно использовать так::

    typedef boost::mpl::quote1<traits::element_required_attributes> 
      get_required_attributes_by_element;
