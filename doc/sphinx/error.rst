Error handling
=================

Возвращаемые значения
-----------------------

Многие методы в SVG++ возвращают значение типа ``bool``. В большинстве случаев оно используется как альтернатива исключениям - 
если метод вернул ``false``, то вызывающий его метод тоже немедленно завершит работу, вернув ``false`` и так далее.

Обработка ошибок конфигурируется с помощью *Error Policy*. Используемый по умолчанию ``policy::error::raise_exception`` 
выбрасывает в случае ошибок объекты исключений, унаследованные от ``std::exception`` и ``boost::exception``.

Error Policy Concept
---------------------------

::

  struct error_policy
  {
    typedef /* ... */ context_type;

    template<class XMLElement, class ElementName>
    static bool unknown_element(Context const &, XMLElement const & element, ElementName const & name);

    template<class XMLAttributesIterator, class AttributeName>
    static bool unknown_attribute(Context const &, 
      XMLAttributesIterator const & attribute, 
      AttributeName const & name,
      BOOST_SCOPED_ENUM(detail::namespace_id) namespace_id,
      tag::source::attribute);

    template<class XMLAttributesIterator, class AttributeName>
    static bool unknown_attribute(Context const &, 
      XMLAttributesIterator const & attribute, 
      AttributeName const & name,
      tag::source::css);

    static bool unexpected_attribute(Context const &, 
      detail::attribute_id id, tag::source::attribute);
  
    template<class AttributeTag>
    static bool required_attribute_not_found(Context const &, 
      AttributeTag);

    template<class AttributeTag, class AttributeValue>
    static bool parse_failed(Context const &, AttributeTag,
      AttributeValue const & value);

    template<class XMLElement>
    static bool unexpected_element(Context const &, XMLElement const & element);

    template<class AttributeTag>
    static bool negative_value(Context const &, AttributeTag);

    typedef boost::exception intercepted_exception_type;

    template<class XMLElement>
    static bool add_element_info(intercepted_exception_type & e, 
      XMLElement const & element);
  };