Document Traversal
======================

::

  template<class Args...>
  struct document_traversal
  {
    template<class XMLElement, class Context>
    static bool load_document(XMLElement const & xml_element_svg, Context & context);

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

  ``context_factories``

    