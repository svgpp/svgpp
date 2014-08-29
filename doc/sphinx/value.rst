How parsed values are passed to user code
=============================================

Для того, чтобы определить, как значение данного атрибута будет передано в user code, нужно использовать
следующий алгоритм:

#. Если атрибут включен в список passthrough_attributes_, то его значение будет передано через `Load Value Policy`_ в виде строки.
#. Если тип ``traits::attribute_type<ElementTag, AttributeTag>::type`` совпадает с ``tag::type::string``, то значение 
   тоже будет передано в виде строки.
#. Иначе надо найти описание этого атрибута в `SVG Specification <http://www.w3.org/TR/SVG/attindex.html>`_
   и узнать его тип.
#. Attribute values of type `<path data> <http://www.w3.org/TR/SVG/paths.html#PathData>`_ 
   (e.g. **d** attribute of **path** element) are described in :ref:`path_section` section.
#. Attribute values of type `<transform list> <http://www.w3.org/TR/SVG/coords.html#TransformAttribute>`_ 
   (e.g. **transform** attribute) are described in :ref:`transform-section` section.
#. All other are passed through `Load Value Policy`_:
  Literal Values
    Если в описании атрибута используется литерал, то такому значению атрибута соответствует вызов с тэгом из
    пространства имен ``tag::value``. Пример атрибутов, которые могут принимать литеральные значения::

        gradientUnits = "userSpaceOnUse | objectBoundingBox"
        clip-path = "<funciri> | none | inherit"

    **gradientUnits** ограничен одним из двух возможных литеральных значений, а **clip-path**, кроме
    литеральных значений **none** и **inherit**, может принимать значения другого типа - *<FuncIRI>*.

  *<length>* or *<coordinate>*
    Передается одним значением, тип которого определяется :ref:`Length Factory <length-section>` 

  *<IRI>* or *<FuncIRI>*
    См. :ref:`iri-section`.
    
  *<integer>*
    Передается значение типа ``int``.

  *<number>* or *<opacity-value>*
    См. number_type_.

  *<percentage>*

  *<color>* or *<color> [<icccolor>]*

  *<angle>*

  *<paint>*

  *<number-optional-number>*

  *<list-of-points>*

  *<list-of-numbers>*

  *<shape>*

  **viewBox** attribute

  **bbox** attribute

  **preserveAspectRatio** attribute


Load Value Policy
-----------------------

Load Value Policy Concept
^^^^^^^^^^^^^^^^^^^^^^^^^^^

*Load Value Policy* - это класс, у которого определены статические методы ``set``, принимающие ссылку на экземпляр 
контекста в качестве первого параметра и тэг атрибута в качестве второго. Число и типы остальных параметров 
зависят от атрибута.

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

  *Load Value Policy* по умолчанию не передает значения ``tag::value::inherit`` для тех properties and 
  presentation attributes, которые не наследуются (см. ``policy::load_value::skip_inherit``). Значение 
  **inherit** для этих атрибутов эквивалентно их отсутствию.


Пример использования *Load Value Policy* по умолчанию::

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

Пример собственной реализации *Load Value Policy*. Создаем специализацию шаблона 
``policy::load_value::default_policy`` для нашего типа контекста (для примера ``boost::optional<double>``)::

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


List of attributes, passed as ``passthrough_attributes`` parameter, aren't parsed and passed by *Load Value Policy* as string.


