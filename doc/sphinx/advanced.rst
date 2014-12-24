Advanced Topics
=====================

Adding new XML parser
-------------------------------------------------

Implementing support for the new XML parser requires creating new specializations for
``svgpp::policy::xml::attribute_iterator`` and ``svgpp::policy::xml::element_iterator`` 
class templates using one of existing implementations in folder ``include/svgpp/policy/xml`` as a sample.

Understanding following types may cause some difficulties::

  template<>
  struct attribute_iterator<CustomXMLAttribute>
  {
    typedef /* ... */ string_type;
    typedef /* ... */ attribute_name_type;
    typedef /* ... */ attribute_value_type;
    typedef /* ... */ saved_value_type;

    /* ... */
  };

``string_type`` 
  A model of 
  `Forward Range <http://www.boost.org/doc/libs/1_57_0/libs/range/doc/html/range/concepts/forward_range.html>`_, 
  with items of some character type (``char``, ``wchar_t``, ``char16_t``, ``char32_t``).
  Easy solution is to use ``boost::iterator_range<CharT const *>``.

``attribute_name_type`` 
  Copy constructible type, for which exists method
  ``string_type attribute_iterator::get_string_range(attribute_name_type const &)``. 
  Value of ``attribute_name_type`` is used only before ``attribute_iterator`` 
  gets incremented and may become invalid after that.

``attribute_value_type`` 
  Copy constructible type, for which exists method
  ``string_type attribute_iterator::get_string_range(attribute_value_type const &)``. 
  Value of ``attribute_value_type`` must be accessible independently of ``attribute_iterator`` state changes.

``saved_value_type`` 
  Copy constructible type, for which exists method
  ``attribute_value_type attribute_iterator::get_value(saved_value_type const &)``. 
  Object of this type must as efficiently as possible save attribute value that may be not requested.
  For example XML parser provides access to XML attribute like this::

    class XmlAttribute
    {
    public:
      std::string getValue() const;
    };

  In this case ``saved_value_type`` may be defined as ``XmlAttribute const *``, instead of ``std::string``, 
  to avoid expences of creating and coping string that may not be requested later.

.. include::  value_parser.rst