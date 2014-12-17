Advanced Topics
=====================

Добавление XML парсера
-------------------------------------

Добавление поддержки нового XML парсера сводится к созданию новых специализаций
``svgpp::policy::xml::attribute_iterator`` и ``svgpp::policy::xml::element_iterator`` по одному из образцов 
в папке ``include/svgpp/policy/xml``.

Некоторую сложность может представлять набор типов::

  template<>
  struct attribute_iterator<CustomXMLAttribute>
  {
    typedef /* ... */ string_type;
    typedef /* ... */ attribute_name_type;
    typedef /* ... */ attribute_value_type;
    typedef /* ... */ saved_value_type;

    /* ... */
  };

``string_type`` должен быть model of 
`Forward Range <http://www.boost.org/doc/libs/1_57_0/libs/range/doc/html/range/concepts/forward_range.html>`_, 
элементы этого range - characters одного из стандартных типов (``char``, ``wchar_t``, ``char16_t``, ``char32_t``).
Простым решением может быть использование ``boost::iterator_range<CharT const *>``.

``attribute_name_type`` 
  Copy constructible тип, для которого есть метод 
  ``string_type attribute_iterator::get_string_range(attribute_name_type const &)``. 
  Значение ``attribute_name_type`` может использоваться только до инкрементации 
  ``attribute_iterator`` и может становиться невалидным при переходе к другому атрибуту.

``attribute_value_type`` 
  Copy constructible тип, для которого есть метод 
  ``string_type attribute_iterator::get_string_range(attribute_value_type const &)``. 
  Значение ``attribute_value_type`` должно сохраняться независимо от состояния
  ``attribute_iterator``.

``saved_value_type`` 
  Copy constructible тип, для которого есть метод
  ``attribute_value_type attribute_iterator::get_value(saved_value_type const &)``. Этот тип должен максимально эффективно
  сохранять значение атрибута, которое может и не понадобиться.
  Если, например, в XML парсере используется такой доступ к XML атрибуту::

    class XmlAttribute
    {
    public:
      std::string getValue() const;
    };

  то ``saved_value_type`` может быть ``XmlAttribute const *``, а не ``std::string``, чтобы
  избежать расходов на создание и копирование строки, которая может и не пригодиться.

.. include::  value_parser.rst