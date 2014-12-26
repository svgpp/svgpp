.. _Associative Sequence: http://www.boost.org/doc/libs/1_55_0/libs/mpl/doc/refmanual/associative-sequence.html
.. _Metafunction Class: http://www.boost.org/doc/libs/1_57_0/libs/mpl/doc/refmanual/metafunction-class.html

Document Traversal
======================

.. _document_traversal:

document_traversal Class
-----------------------------

``document_traversal`` template class is a facade that provides access to most features of SVG++ library.

``document_traversal`` contains only static methods.
Each ``document_traversal`` method receives *context* and *XML element* parameters.
*Context* is a user defined object that will receive parsed data.
*XML element* has :ref:`type <xml_policy_types>` of XML element in chosen XML parser.

:ref:`Named template parameters <named-params>` of ``document_traversal`` class allows to configure 
almost each aspect of  SVG processing.

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

Methods
""""""""""""

::

  template<class XMLElement, class Context>
  static bool load_document(XMLElement const & xml_root_element, Context & context);

.. _fakeRef3:

  ``load_document`` - is a shortcut for ``load_expected_element`` that receives root element (**svg**) of SVG document 
  as ``xml_root_element`` parameter.

::

  template<class XMLElement, class Context, class ElementTag>
  static bool load_expected_element(
    XMLElement const & xml_element, 
    Context & context, 
    ElementTag expected_element);

.. _fakeRef4:

  ``load_expected_element`` loads ``xml_element`` and its descendands and passes parsed data
  to ``context``. 

  ``load_expected_element`` expects that XML name of ``xml_element`` corresponds to ``ElementTag``.
  Otherwise exception ``unexpected_element_error`` is thrown (see :ref:`Error Policy <error-handling>`).

::

  template<class RefArgs...>
  template<class XMLElement, class Context>
  static bool load_referenced_element<RefArgs...>::load(
    XMLElement const & xml_element, 
    Context & parent_context);

.. _fakeRef5:

  ``load_referenced_element`` mainly used to load SVG elements that are referenced by other SVG element,
  e.g. **use** element or *gradients*. 
  Unlike ``load_expected_element``, allowed XML name of ``xml_element`` isn't limited to one and is passed
  as ``expected_elements`` sequence. 

  Named class template parameters of ``document_traversal::load_referenced_element``

  ``expected_elements`` *(required)*
    Value is `Associative Sequence`_.
    Contains list of tags of expected elements for ``xml_element``.
    If XML name of ``xml_element`` doesn't correspond to any from the list, exception 
    ``unexpected_element_error`` (see :ref:`Error Policy <error-handling>`) is thrown.

    .. note::

      ``traits::reusable_elements`` contains list of elements, that can be referenced by **use** element.

.. _referencing_element:

  ``referencing_element`` *(optional)*
    Value is tag of element that references ``xml_element``.
    It is used only if *Viewport Policy* requires processing of viewport by SVG++ 
    (for correct processing of **svg** and **symbol** elements, referenced by **image** or **use** elements).

  ``processed_elements`` or ``ignored_elements`` *(optional)*
    Only one may be set. 
    See :ref:`description <processed_elements>` of ``document_traversal`` parameters with the same names.
    Allows to override ``document_traversal`` settings ``processed_elements``/``ignored_elements``
    for the passed SVG element. Child elements will be processed with ``document_traversal`` settings.


document_traversal Named Class Template Parameters
--------------------------------------------------------

.. _processed_elements:

  ``ignored_elements`` and ``processed_elements``
    One of them must be set to limit SVG elements processed. 
    It must be model of `Associative Sequence`_ (e.g. ``boost::mpl::set``),
    containing element tags.

    If ``processed_elements`` is set, then only the listed elements are processed,
    otherwise if ``ignored_elements`` is set, then only non-listed elements are processed.

.. _processed_attributes:

  ``ignored_attributes`` and ``processed_attributes``
    One of them must be set to limit SVG attributes processed. 
    It must be model of `Associative Sequence`_,
    containing attribute tags. Also it may contain pairs <element tag, attribute tag>
    like this ``boost::mpl::pair<tag::element::g, tag::attribute::transform>``, such pair
    is matched if both processed element and processed attribute matches tags.

    If ``processed_attributes`` is set, then only listed attributes are processed,
    otherwise if ``ignored_attributes`` is set, then only non-listed attributes are processed.

.. _passthrough_attributes:

  ``passthrough_attributes`` *(optional)*
    Is a `Associative Sequence`_, that contains attribute tags. 
    Values of listed attributes aren't parsed by SVG++, and passed to the user code as :ref:`string <passing-string>`.

  ``context_factories`` *(optional)*
    See :ref:`context_factories`.

  ``attribute_traversal_policy`` *(optional)*
    See :ref:`attribute_traversal_policy`.

.. _context_factories:

Context Factories
--------------------------------

When ``document_traversal`` traverses SVG tree, it, for each SVG element met, chooses context 
to be used to pass attribute values and character data by corresponding *event policy*. 
To configure this behavior ``context_factories`` parameter is used.

``context_factories`` parameter accepts `Metafunction Class`_, that receives: 

  - ``ParentContext`` - parent context type (context used for parent SVG element);
  - ``ElementTag`` - element tag (corresponds to the SVG element met),
  and returns *Context Factory*.

::

  typedef 
    typename context_factories::template apply<ParentContext, ElementTag>::type 
      selected_context_factory;

SVG++ provides several *Context Factories*::

  template<class ParentContext, class ElementTag>
  class factory::context::same;

.. _fakeRef1:

  The new context object isn't created, parent context will be used. 
  ``on_enter_element(ElementTag())`` and ``on_exit_element()`` methods of parent context will be called
  at the start and at the end of element processing respectively.

::

  template<class ChildContext>
  class factory::context::on_stack;

.. _fakeRef2:

  The new object of type ``ChildContext`` is created on stack. 
  Constructor of the object is passed with the reference to the parent context. 
  After element processing is finished, method ``ChildContext::on_exit_element()`` is called before destruction.
  Lifetime of context object matches processing of SVG element content (element attributes, child elements and text nodes).

  ``factory::context::on_stack_with_xml_element`` is the same as ``factory::context::on_stack``, 
  but ``ChildContext`` constructor receives second parameter - XML element. 
  Its type depends on XML parser used.

::
  
  template<
    class ElementTag, 
    class ChildContextPtr, 
    class ChildContext = typename boost::pointee<ChildContextPtr>::type 
  >
  class get_ptr_from_parent;

.. _fakeRef7:

  Pointer ``ChildContextPtr`` to context object is requested from parent context by call to the
  method ``get_child_context(ElementTag())``. Pointer can be raw pointer or some smart pointer. 
  ``ChildContext::on_exit_element()`` is called after element processing is finished.


.. _attribute_traversal_policy:

Attribute Traversal Policy
-----------------------------------

*Attribute Traversal Policy* configures order and other aspects of SVG attributes and CSS properties processing.

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
  Contents of **style** attribute is parsed as a sequence of semicolon-separated pairs property-value.

``css_hides_presentation_attribute = true``
  Is checked only if ``parse_style = true``. 
  
  If the same property is set both in **style** attribute and as a *presentation attribute*, 
  then only value in **style** attribute will be parsed (it has higher priority according to SVG).

  If ``css_hides_presentation_attribute = false``, then memory usage is lower, but both values
  (from **style** attribute and a *presentation attribute*) of the same property will be parsed and passed to the user code
  in the arbitrary order.

.. _get_priority_attributes_by_element:

``get_priority_attributes_by_element``, ``get_deferred_attributes_by_element`` and ``get_required_attributes_by_element``
  They are `Metafunction classes <http://www.boost.org/doc/libs/1_57_0/libs/mpl/doc/refmanual/metafunction-class.html>`_,
  receiving element tag and returning 
  `Forward Sequence <http://www.boost.org/doc/libs/1_57_0/libs/mpl/doc/refmanual/forward-sequence.html>`_ of attribute tags.

  Attributes, returned by ``get_priority_attributes_by_element`` metafunction for current element,
  will be processed before all others in the same order as in sequence. 

  Attributes, returned by ``get_deferred_attributes_by_element`` metafunction for current element,
  will be processed after all others in the same order as in sequence. 

.. _notify_in_priority:

  .. note::

    Sequences, returned by ``get_priority_attributes_by_element`` and ``get_deferred_attributes_by_element``, 
    besides attribute tags may contain elements like ``notify_context<EventTag>``. 
    ``EventTag`` is an arbitrary tag, that will be passed to ``notify`` method of the context. 
    ``notify(EventTag())`` will be called right after all previous attributes in the sequence are processed.

  If element lacks any attribute from the sequence, returned by ``get_required_attributes_by_element`` metafunction for
  this element, then ``required_attribute_not_found_error`` exception will be thrown (see :ref:`Error Policy <error-handling>`).
  SVG++ contains definition of ``traits::element_required_attributes`` metafunction
  that returns mandatory attributes for element, according to SVG Specification. It can be used like this::

    struct my_attribute_traversal_policy
    {
      /* ... other members ... */
      typedef boost::mpl::quote1<traits::element_required_attributes> 
        get_required_attributes_by_element;
    };