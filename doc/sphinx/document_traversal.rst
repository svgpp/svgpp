Document Traversal
======================

document_traversal Class
-----------------------------

::

  template<class Args...>
  struct document_traversal
  {
    template<class XMLElement, class Context>
    static bool load_document(XMLElement const & xml_root_element, Context & context)
      { return load_expected_element(xml_root_element, context, tag::element::svg()); }

    template<class XMLElement, class Context, class ElementTag>
    static bool load_expected_element(XMLElement const & xml_element, Context & context, ElementTag expected_element);

    template<class RefArgs...>
    struct load_referenced_element
    {
      template<class XMLElement, class Context>
      static bool load(XMLElement const & xml_element, Context & parent_context);
    };
  };

Named class template parameters

  ``ignored_elements`` and ``processed_elements``

    Один из этих параметров должен быть задан, чтобы определить какие элементы SVG обрабатываются. 
    Значение параметра - model of `Associative Sequence`_, например ``boost::mpl::set``,
    содержащий тэги элементов.

    Если задан ``processed_elements``, то обработка ограничивается только перечисленными элементами,
    если ``ignored_elements``, то обрабатываются все элементы, кроме перечисленных.

.. _processed_attributes:

  ``ignored_attributes``, ``processed_attributes``
    Один из этих параметров должен быть задан, чтобы определить какие атрибуты SVG обрабатываются. 
    Значение параметра - model of `Associative Sequence`_, например ``boost::mpl::set``,
    содержащий тэги атрибутов. Кроме тэгов атрибутов он может содержать пары <тэг элемента, тэг атрибута>
    в таком виде ``boost::mpl::pair<tag::element::g, tag::attribute::transform>``, в этом случае
    проверка проходит если и атрибут и элемент соответствуют обрабатывемому. 

    Если задан ``processed_attributes``, то обработка ограничивается только перечисленными атрибутами,
    если ``ignored_attributes``, то обрабатываются все атрибуты, кроме перечисленных.  

.. _passthrough_attributes:

  ``passthrough_attributes``
    Значение параметра - model of `Associative Sequence`_, например ``boost::mpl::set``,
    содержащий тэги атрибутов. Значения перечисленных атрибутов не обрабатываются SVG++,
    а передаются в пользовательский код в виде :ref:`строки <passing-string>`.

  ``context_factories``
    См. :ref:`context_factories`.

  ``attribute_traversal_policy``
    См. :ref:`attribute_traversal_policy`.

  ``referencing_element``


.. _context_factories:

Context Factories
--------------------------------

При обходе дерева SVG, для каждого встреченного элемента выбирается контекст, который будет использоваться
для передачи значений атрибутов и character data посредством соответствующего *event policy*. 
Для конфигурации этого поведения используется параметр ``context_factories``.

Параметр ``context_factories`` содержит metafunction class, принимающий два параметра: 
  ``ParentContext`` - тип родительского контекста (контекст, использованный для parent SVG element);
  
  ``ElementTag`` - тэг элемента (соответствует типу встреченного элемента),
и возвращающий тип *Context Factory*.

::

  typedef typename context_factories::template apply<ParentContext, ElementTag>::type selected_context_factory;


SVG++ предоставляет несколько готовых *Context Factory*::

  template<class ParentContext, class ElementTag>
  class factory::context::same;

Новый контекст для элемента не создается, будет использован parent context. У parent context будут вызваны 
методы ``on_enter_element(ElementTag())`` и ``on_exit_element()`` в начале и в конце обработки элемента соответственно.

::

  template<class ChildContext>
  class factory::context::on_stack;

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

``parse_style = true``
  Содержимое атрибута **style** обрабатывается как последовательность semicolon-separated пар свойство-значение.

``css_hides_presentation_attribute = true``
  Проверяется только если ``parse_style = true``. 
  
  Если одно свойство задано как в **style**, так и соответствующим
  *presentation attribute*, то будет обработано только значение свойства в атрибуте **style** (оно имеет больший
  приоритет в соответствии со спецификацией SVG).

  Если ``css_hides_presentation_attribute = false``, то используется меньше памяти, но могут приходить 
  значения одного свойства и из **style** и из *presentation attribute* в произвольном порядке.

``get_priority_attributes_by_element``

``get_deferred_attributes_by_element``

``get_required_attributes_by_element``
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

    typedef boost::mpl::quote1<traits::element_required_attributes> get_required_attributes_by_element;





