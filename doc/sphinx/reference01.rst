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

По умолчанию используется ``policy::load_value::forward_to_method``, который переадресует вызывы методов ``set`` объекту ``context`` с параметрами  ``AttributeTag tag, ValueType1 const & value1...``::

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

  load_value policy по умолчанию не передает значения ``tag::value::inherit`` для тех properties and presentation attributes, которые не наследуются (см. ``policy::load_value::skip_inherit``). Значение **inherit** для этих атрибутов эквивалентно их отсутствию.


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


load_transform Policy
-------------------------

load_transform Policy Concept
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

load_transform policy зависит от compile-time настроек, задаваемых посредством transform policy. Если используются настройки ``policy::transform::raw``, максимально сохраняющие структуру входных данных, то load_transform policy имеет вид::

  struct load_transform_policy
  {
    typedef /*...*/ context_type; 

    static void append_transform_matrix(context_type & context, const boost::array<number_type, 6> & matrix);
    static void append_transform_translate(context_type & context, number_type tx, number_type ty);
    static void append_transform_translate(context_type & context, number_type tx);
    static void append_transform_scale(context_type & context, number_type sx, number_type sy);
    static void append_transform_scale(context_type & context, number_type scale);
    static void append_transform_rotate(context_type & context, number_type angle);
    static void append_transform_rotate(context_type & context, number_type angle, number_type cx, number_type cy);
    static void append_transform_skew_x(context_type & context, number_type angle);
    static void append_transform_skew_y(context_type & context, number_type angle);
  };

Другие конфигурации transform policy могут уменьшать количество методов. 

Настройки ``policy::transform::matrix``, используемые по умолчанию, объединяют all transforms in list in single matrix transform и load_transform policy имеет вид::

  struct load_transform_policy
  {
    typedef /*...*/ context_type; 

    static void set_transform_matrix(context_type & context, const boost::array<number_type, 6> & matrix);
  };

load_transform policy по умолчанию (policy::load_transform::forward_to_method) переадресует вызовы статических методов на вызовы методов объекта context с остальными параметрами::

  struct forward_to_method
  {
    typedef Context context_type; 

    template<class Number>
    static void append_transform_matrix(context_type & context, const boost::array<Number, 6> & matrix)
    {
      context.append_transform_matrix(matrix);
    }

    /*...*/
  };

Пример с использованием настроек по умолчанию::
  
  #include <svgpp/svgpp.hpp>

  struct Context
  {
    void set_transform_matrix(const boost::array<double, 6> & matrix)
    {
      for(auto n: matrix)
        std::cout << n << " ";
      std::cout << "\n";
    }
  };

  void func()
  {
    Context context;
    value_parser<tag::type::transform_list>::parse(tag::attribute::transform(), context, 
      std::string("translate(-10,-20) scale(2) rotate(45) translate(5,10)"), tag::source::attribute());
  }

Пример, в котором умножение последовательных transforms происходит в user code::

  #include <svgpp/svgpp.hpp>
  #include <boost/numeric/ublas/matrix.hpp>
  #include <boost/numeric/ublas/io.hpp>

  namespace ublas = boost::numeric::ublas;

  typedef ublas::scalar_matrix<double> matrix_t;

  struct load_transform_policy
  {
    typedef matrix_t context_type;

    static void append_transform_matrix(matrix_t & transform, const boost::array<number_type, 6> & matrix)
    {
      matrix_t m(3, 3);
      m(0, 0) = matrix[0]; m(1, 0) = matrix[1]; m(0, 1) = matrix[2]; 
      m(1, 1) = matrix[3]; m(0, 2) = matrix[4]; m(1, 2) = matrix[5]; m(2, 2) = 1.0;
      transform = ublas::prod(transform, matrix);
    }

    static void append_transform_translate(matrix_t & transform, number_type tx, number_type ty)
    {
      matrix_t m = ublas::identity_matrix(3, 3);
      m(0, 2) = tx; m(1, 2) = ty; 
      transform = ublas::prod(transform, matrix);
    }

    static void append_transform_scale(matrix_t & transform, number_type sx, number_type sy)
    {
      matrix_t m(3, 3);
      m(0, 0) = sx; m(1, 1) = sy; m(2, 2) = 1; 
      transform = ublas::prod(transform, matrix);
    }

    static void append_transform_rotate(matrix_t & transform, number_type angle)
    {
      matrix_t m(3, 3);
      m(0, 0) =  std::cos(angle); m(1, 0) = std::sin(angle); 
      m(0, 1) = -std::sin(angle); m(1, 1) = std::cos(angle); m(2, 2) = 1; 
      transform = ublas::prod(transform, matrix);
    }

    static void append_transform_skew_x(matrix_t & transform, number_type angle)
    {
      matrix_t m = ublas::identity_matrix(3, 3);
      m(0, 1) = std::tan(angle);
      transform = ublas::prod(transform, matrix);
    }

    static void append_transform_skew_y(matrix_t & transform, number_type angle)
    {
      matrix_t m = ublas::identity_matrix(3, 3);
      m(1, 0) = std::tan(angle);
      transform = ublas::prod(transform, matrix);
    }
  };

  void func()
  {
    matrix_t transform(ublas::identity_matrix(3, 3));
    value_parser<
      tag::type::transform_list,
      transform_policy<policy::transform::minimal<double> >,
      load_transform_policy<load_transform_policy>
    >::parse(tag::attribute::transform(), transform, 
      std::string("translate(-10,-20) scale(2) rotate(45) translate(5,10)"), tag::source::attribute());
    std::cout << context.transform << "\n";
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