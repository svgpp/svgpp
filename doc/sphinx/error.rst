.. _error-handling:

Error handling
=================

Returned values
-----------------------

In most cases when SVG++ methods returns ``bool`` values, they can be used as an alternative to exceptions
- if method returns ``false``, then calling method immediately returns with ``false`` result and so on.

Error reporting is controlled by *Error Policy*. ``policy::error::raise_exception`` used by default
throws exception objects, derived from ``std::exception`` and ``boost::exception``. 
In this case only ``true`` may be returned as a result code.

Default Error Handling
----------------------------

Default ``policy::error::raise_exception`` uses Boost.Exception for transporting arbitrary 
data to the catch site.

``boost::error_info`` uses tags ``tag::error_info::xml_element`` and ``tag::error_info::xml_attribute`` 
to pass information about place in SVG document where error occured alongside with the exception object.
Value type depends on XML parser and *XML Policy* used.

Example of SVG++ exception handling when RapidXML NS parser is used::

  typedef rapidxml_ns::xml_node<> const * XMLElement;

  try
  {
    /* ... */
    document_traversal</* ... */>::load_document(/* ... */);
  }
  catch(svgpp::exception_base const & e)
  {
    typedef boost::error_info<svgpp::tag::error_info::xml_element, XMLElement> element_error_info;
    std::cerr << "Error reading SVG";
    if (XMLElement const * element = boost::get_error_info<element_error_info>(e))
      std::cerr 
        << " in element '" 
        << std::string((*element)->name(), (*element)->name() + (*element)->name_size())
        << "'";
    std::cerr << ": " << e.what() << "\n";
  }

.. _error_policy:

Error Policy Concept
---------------------------

::

  struct error_policy
  {
    typedef /* ... */ context_type;

    template<class XMLElement, class ElementName>
    static bool unknown_element(
      context_type const &, 
      XMLElement const & element, 
      ElementName const & name);

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

    typedef /* ... */ intercepted_exception_type;

    template<class XMLElement>
    static bool add_element_info(intercepted_exception_type & e, 
      XMLElement const & element);
  };

If *Error Policy* method returns ``true``, then SVG++ continues SVG processing skipping the part with the error. 
In some cases it may lead to problems in further processing. 

If method returns ``false``, then processing immediately stops.