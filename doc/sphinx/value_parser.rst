Value Parsers
==================

Чтобы выбрать ``value_parser`` для атрибута, надо воспользоваться метафункцией ``traits::attribute_type``::

  template<class Element, class Attribute> 
  struct attribute_type
  {
    typedef /*unspecified*/ type;
  };

Возвращаемый тип может быть:

* Одним из тэгов типов. Например **width**, как и многие другие атрибуты имеет тип *length*::

    BOOST_MPL_ASSERT(( boost::is_same< 
      traits::attribute_type<tag::element::rect, tag::attribute::width>::type, 
      tag::type::length> ));

* Тэгом атрибута. Например, атрибут **viewBox** имеет собственный синтаксис::

    BOOST_MPL_ASSERT(( boost::is_same< 
      traits::attribute_type<tag::element::svg, tag::attribute::viewBox>::type, 
      tag::attribute::viewBox> ));
  
* Или парой *<тэг элемента, тэг атрибута>*. Например, атрибут **type** может принимать разные значения в элементах **animateTransform**, **feColorMatrix**, **feTurbulence** и т. д.::

    BOOST_MPL_ASSERT(( boost::is_same< 
      traits::attribute_type<tag::element::animateTransform, tag::attribute::type>::type, 
      boost::mpl::pair<tag::element::animateTransform, tag::attribute::type> > ));


Интерфейс attribute value parsers::

  template<class ValueType, class Args..>
  struct value_parser
  {
    template<class AttributeTag, class Context, class AttributeValue, class PropertySource>
    static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                      PropertySource source);
  };

``AttributeTag tag`` - тэг атрибута, используется для идентификации значения, при передаче его контексту, но не влияет на парсинг.
``Context & context`` - ссылка на контекст, которому будет передано значение.
``AttributeValue const & attribute_value`` - строковое значение атрибута, должно быть моделью *Forward Range* concept (Boost.Range).
``PropertySource source`` - может иметь один из двух типов: ``tag::source::attribute`` или ``tag::source::css``, в зависимости от того, значение соответствует атрибуту XML или CSS property.

