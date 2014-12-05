Error handling
=================

Возвращаемые значения
-----------------------

Многие методы в SVG++ возвращают значение типа ``bool``. В большинстве случаев оно используется как альтернатива исключениям - 
если метод вернул ``false``, то вызывающий его метод тоже немедленно завершит работу, вернув ``false`` и так далее.

Обработка ошибок конфигурируется с помощью *Error Policy*. Используемый по умолчанию ``policy::error::raise_exception`` 
выбрасывает в случае ошибок объекты исключений, унаследованные от ``std::exception`` и ``boost::exception``.

Default Error Handling
----------------------------

Используемый по умолчанию ``policy::error::raise_exception`` uses transporting of arbitrary data to the catch site feature
of Boost.Exception.

``boost::error_info`` с тэгами ``tag::error_info::xml_element`` и ``tag::error_info::xml_attribute`` передают
с исключением информацию о месте возникновения исключения. Тип значения зависит от используемого XML parser и *XML Policy*

Пример обработки исключений с парсером RapidXML NS::

  typedef rapidxml_ns::xml_node<> const * XMLElement;

  try
  {
    /* ... */
    document_traversal</* ... */>::load_document(/* ... */);
  }
  catch(svgpp::exception_base const & e)
  {
    typedef boost::error_info<svgpp::tag::error_info::xml_element, XMLElement> element_error_info;
    std::cerr << "Error reading XML";
    if (XMLElement const * element = boost::get_error_info<element_error_info>(e))
      std::cerr 
        << " in element '" 
        << std::string((*element)->name(), (*element)->name() + (*element)->name_size())
        << "'";
    std::cerr << ": " << e.what() << "\n";
  }


Error Policy Concept
---------------------------

::

  struct error_policy
  {
    typedef /* ... */ context_type;

    template<class XMLElement, class ElementName>
    static bool unknown_element(context_type const &, XMLElement const & element, ElementName const & name);

    template<class XMLAttributesIterator, class AttributeName>
    static bool unknown_attribute(context_type &, 
      XMLAttributesIterator const & attribute, 
      AttributeName const & name,
      BOOST_SCOPED_ENUM(detail::namespace_id) namespace_id,
      tag::source::attribute);

    template<class XMLAttributesIterator, class AttributeName>
    static bool unknown_attribute(context_type &, 
      XMLAttributesIterator const & attribute, 
      AttributeName const & name,
      tag::source::css);

    static bool unexpected_attribute(context_type &, 
      detail::attribute_id id, tag::source::attribute);
  
    template<class AttributeTag>
    static bool required_attribute_not_found(context_type &, AttributeTag);

    template<class AttributeTag, class AttributeValue>
    static bool parse_failed(context_type &, AttributeTag,
      AttributeValue const & value);

    template<class XMLElement>
    static bool unexpected_element(context_type &, XMLElement const & element);

    template<class AttributeTag>
    static bool negative_value(context_type &, AttributeTag);

    typedef boost::exception intercepted_exception_type;

    template<class XMLElement>
    static bool add_element_info(intercepted_exception_type & e, 
      XMLElement const & element);
  };