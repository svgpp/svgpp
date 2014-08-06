How parsed values are passed to user code
=============================================

* Attribute values of type *path data* (e.g. **d** attribute of **path** element) are passed to user code as series of calls by `load_path Policy`_.
* Attribute values of type *transform list* (e.g. **transform** attribute) are passed by `load_transform Policy`_.
* All other are passed through `load_value Policy`_.

Из этих правил есть исключения:

* Если атрибут включен в список passthrough_attributes_, то он будет передан через `load_value Policy`_ в виде строки.
* Адаптеры могут использовать любые policy.

load_value Policy
-----------------------

load_value Policy Concept
^^^^^^^^^^^^^^^^^^^^^^^^^^^

load_value policy - это класс, у которого определены статические методы ``set``, принимающие ссылку на экземпляр контекста в качестве первого параметра и тэг атрибута в качестве второго. Число и типы остальных параметров зависят от атрибута.

::

  struct load_value_policy
  {
    static void set(Context & context, AttributeTag tag, ValueType1 const & value1);
    static void set(Context & context, AttributeTag tag, ValueType1 const & value1, ValueType2 const & value2);
    /*...*/
  };

По умолчанию используется ``policy::load_value::forward_to_method``, который переадресует вызывает методов ``set`` объекту ``context`` с параметрами  ``AttributeTag tag, ValueType1 const & value1...``::

  template<class Context>
  struct forward_to_method
  {
    template<class AttributeTag, class T1>
    static void set(Context & context, AttributeTag tag, T1 const & value)
    {
      context.set(tag, value);
    }
    /*...*/
  };

.. note::

  load_value policy по умолчанию не передает значения ``tag::value::inherit`` для тех properties and presentation attributes, которые не наследуются (см. ``policy::load_value::skip_inherit``). 


Пример использования load_value policy по умолчанию::

  #include <svgpp/svgpp.hpp>

  struct Context
  {
    void set(tag::attribute::amplitude, double value)
    {
      std::cout << value << "\n";
    }
  };

  void func()
  {
    Context context;
    value_parser<tag::type::number>::parse(tag::attribute::amplitude(), context, std::string("3.14"), tag::source::attribute());
  }

Пример с заменой load_value policy. Создаем специализацию шаблона ``policy::load_value::default_policy`` для нашего типа контекста (для примера ``boost::optional<double>``)::

  namespace svgpp { namespace policy { namespace load_value 
  {
    template<>
    struct default_policy<boost::optional<double> >
    {
      template<class AttributeTag>
      void set(boost::optional<double> & context, AttributeTag tag, double value) 
      {
        context = value;
      }
    };
  }}}

  void func()
  {
    boost::optional<double> context;
    value_parser<tag::type::number>::parse(tag::attribute::amplitude(), context, std::string("3.14"), tag::source::attribute());
    if (context)
      std::cout << *context << "\n";
  }

+------------------------------------------------------------------------+---------------------------------+----------------+
| Attribute type                                                         | Parameters                      | Default        |
+========================================================================+=================================+================+
| `tag::type::number                                                     | (number_type value)             | (double value) |
| <http://www.w3.org/TR/SVG11/types.html#DataTypeNumber>`_               |                                 |                |
+------------------------------------------------------------------------+---------------------------------+----------------+
| `tag::type::integer                                                    | (int value)                                      |
| <http://www.w3.org/TR/SVG11/types.html#Integer>`_                      |                                                  |
+------------------------------------------------------------------------+---------------------------------+----------------+
| `tag::type::length                                                     | (double value)                                   |
| <http://www.w3.org/TR/SVG11/types.html#DataTypeLength>`_               |                                                  |
+------------------------------------------------------------------------+---------------------------------+----------------+


List of attributes, passed as ``passthrough_attributes`` parameter, aren't parsed and passed by *load_value* policy as string.


Чтобы выбрать ``value_parser`` для атрибута, надо воспользоваться метафункцией ``traits::attribute_type``::

  template<class Element, class Attribute> 
  struct attribute_type
  {
    typedef /*unspecified*/ type;
  };

Возвращаемый тип может быть:

* Одним из тэгов типов. Например **width**, как и многие другие атрибуты имеет тип *length*::

    BOOST_MPL_ASSERT(( boost::is_same< traits::attribute_type<tag::element::rect, tag::attribute::width>::type, tag::type::length> ));

* Тэгом атрибута. Например, атрибут **viewBox** имеет собственный синтаксис::

    BOOST_MPL_ASSERT(( boost::is_same< traits::attribute_type<tag::element::svg, tag::attribute::viewBox>::type, tag::attribute::viewBox> ));
  
* Или парой *<тэг элемента, тэг атрибута>*. Например, атрибут **type** может принимать разные значения в элементах **animateTransform**, **feColorMatrix**, **feTurbulence** и т. д.::

    BOOST_MPL_ASSERT(( boost::is_same< traits::attribute_type<tag::element::animateTransform, tag::attribute::type>::type, 
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