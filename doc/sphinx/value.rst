How parsed values are passed to user code
=============================================

Для того, чтобы определить, как значение данного атрибута будет передано в user code, нужно использовать
следующий алгоритм:

#. Если атрибут включен в список passthrough_attributes_, то его значение будет передано через `Value Events Policy`_ 
   в виде :ref:`строки <passing-string>`.
#. Если тип ``traits::attribute_type<ElementTag, AttributeTag>::type`` совпадает с ``tag::type::string``, то значение 
   тоже будет передано в виде :ref:`строки <passing-string>`.
#. Иначе надо найти описание этого атрибута в `SVG Specification <http://www.w3.org/TR/SVG/attindex.html>`_
   и узнать его тип.
#. Attribute values of type `<path data> <http://www.w3.org/TR/SVG/paths.html#PathData>`_ 
   (e.g. **d** attribute of **path** element) are described in :ref:`path_section` section.
#. Attribute values of type `<transform list> <http://www.w3.org/TR/SVG/coords.html#TransformAttribute>`_ 
   (e.g. **transform** attribute) are described in :ref:`transform-section` section.
#. All other are passed through `Value Events Policy`_.


Value Events Policy
-----------------------

Value Events Policy Concept
^^^^^^^^^^^^^^^^^^^^^^^^^^^

*Value Events Policy* - это класс, у которого определены статические методы ``set``, принимающие ссылку на экземпляр 
контекста в качестве первого параметра и тэг атрибута в качестве второго. Число и типы остальных параметров 
зависят от атрибута.

::

  struct value_events_policy
  {
    static void set(Context & context, AttributeTag tag, ValueType1 const & value1);
    static void set(Context & context, AttributeTag tag, ValueType1 const & value1, ValueType2 const & value2);
    /*...*/
  };

По умолчанию используется ``policy::value_events::forward_to_method``, который переадресует вызовы 
методов ``set`` объекту ``context`` с параметрами  ``AttributeTag tag, ValueType1 const & value1...``::

  template<class Context>
  struct forward_to_method
  {
    template<class AttributeTag, class ...Args>
    static void set(Context & context, AttributeTag tag, Args... args)
    {
      context.set(tag, args...);
    }
  };

.. note::

  *Value Events Policy* по умолчанию не передает значения ``tag::value::inherit`` для тех properties and 
  presentation attributes, которые не наследуются (см. ``policy::value_events::skip_inherit``). Значение 
  **inherit** для этих атрибутов эквивалентно их отсутствию.


Пример использования *Value Events Policy* по умолчанию::

  #include <svgpp/svgpp.hpp>

  using namespace svgpp;

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

Пример собственной реализации *Value Events Policy*. Создаем специализацию шаблона 
``policy::value_events::default_policy`` для нашего типа контекста (для примера ``boost::optional<double>``)::

  namespace svgpp { namespace policy { namespace value_events 
  {
    template<>
    struct default_policy<boost::optional<double> >
    {
      template<class AttributeTag>
      static void set(boost::optional<double> & context, AttributeTag tag, double value) 
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

*Literal Values*
  Если значением атрибута может быть литерал, то такому значению атрибута соответствует вызов с тэгом из
  пространства имен ``tag::value``. Пример атрибутов, которые могут принимать литеральные значения::

      gradientUnits = "userSpaceOnUse | objectBoundingBox"
      clip-path = "<funciri> | none | inherit"

  **gradientUnits** ограничен одним из двух возможных литеральных значений, а **clip-path**, кроме
  литеральных значений **none** и **inherit**, может принимать значения другого типа - *<FuncIRI>*.

  Пример реализации контекста, принимающего значения атрибута **gradientUnits**::

    class GradientContext
    {
    public:
      GradientContext()
        : m_gradientUnitsUserSpace(false)
      {}

      void set(tag::attribute::gradientUnits, tag::value::userSpaceOnUse)
      {
        m_gradientUnitsUserSpace = true;
      }

      void set(tag::attribute::gradientUnits, tag::value::objectBoundingBox)
      {
        m_gradientUnitsUserSpace = false;
      }

    private:
      bool m_gradientUnitsUserSpace;
    };

*<length>* or *<coordinate>*
  Передается одним параметром, тип которого определяется :ref:`Length Factory <length-section>` 

*<IRI>* or *<FuncIRI>*
  См. :ref:`iri-section`.
    
*<integer>*
  Передается одно значение типа ``int``.

*<number>* or *<opacity-value>*
  Передается одим параметром типа number_type_ (по умолчанию ``double``).

*<percentage>*

*<color>* 
  Передается одним параметром, тип которого определяется :ref:`Color Factory <color-section>` 

*<color> [<icccolor>]*
  Если *<icccolor>* не задан, то передается одним параметром, тип которого определяется :ref:`Color Factory <color-section>`.
  Если задан, то добавляется второй параметр, тип которого определяется :ref:`ICC Color Factory <icc-color-factory-section>`.
  Пример::

    struct Context
    {
      void set(tag::attribute::flood_color, int rgb);
      void set(tag::attribute::flood_color, int rgb, tag::skip_icc_color);
      void set(tag::attribute::flood_color, tag::value::currentColor);
      void set(tag::attribute::flood_color, tag::value::inherit);
    };

*<angle>*
  Передается одним параметром, тип и значение которого определяются :ref:`Angle Factory <angle-section>` (по умолчанию 
  это значение типа ``double`` в градусах).

*<number-optional-number>*
  Передается одим или двумя параметрами типа number_type_ (по умолчанию ``double``).

*<list-of-numbers>*, *<list-of-lengths>* or *<list-of-points>* 
  Передается одним параметром of unspecified type, который является моделью 
  `Boost Single Pass Range <http://www.boost.org/doc/libs/1_57_0/libs/range/doc/html/range/concepts/single_pass_range.html>`_.
  
  Элементы *range* имеют тип number_type_ (по умолчанию ``double``) для *<list-of-numbers>*. 
  Или определяется :ref:`Length Factory <length-section>` в случае *<list-of-lengths>*.
  Или имеют тип ``std::pair<number_type, number_type>`` (по умолчанию ``std::pair<double, double>``) в случае *<list-of-points>*.

  Пример::

    struct Context
    {
      template<class Range>
      void set(tag::attribute::kernelMatrix, Range const & range)
      {
        for(typename boost::range_iterator<Range>::type it = boost::begin(range), end = boost::end(range); 
          it != end; ++it)
          std::cout << *it;
      }
    };

  .. note::
    Если решение с template function не может быть использовано (например, требуется виртуальная функция),
    в качестве типа параметра можно использовать Boost 
    `any_range <http://www.boost.org/doc/libs/1_57_0/libs/range/doc/html/range/reference/ranges/any_range.html>`_::

      typedef boost::any_range<double, boost::single_pass_traversal_tag, double const &, std::ptrdiff_t> Range;


*<shape>*
  Передается пятью параметрами - первый *tag* ``tag::value::rect``, остальные типа number_type_ (по умолчанию ``double``): 
  ``(tag::value::rect(), top, right, bottom, left)``.

**viewBox** attribute
  Передается четырьмя параметрами типа number_type_ (по умолчанию ``double``): ``(x, y, width, height)``.

**bbox** attribute
  Передается четырьмя параметрами типа number_type_ (по умолчанию ``double``): ``(lo_x, lo_y, hi_x, hi_y)``.

**preserveAspectRatio** attribute

**text-decoration** property
  Значения **none** and **inherit** передаются как *Literal Values* (см. выше).
  Остальные варианты передаются восемью параметрами, из них четыре типа ``bool``, каждому предшествует *tag*, 
  определяющий назначение параметра. Boolean parameters принимают значение ``true``, если соответствующий 
  text decoration указан в property::

    struct Context
    {
      void set(tag::attribute::text_decoration, tag::value::none);
      void set(tag::attribute::text_decoration, tag::value::inherit);
      void set(tag::attribute::text_decoration, 
        tag::value::underline,    bool underline,
        tag::value::overline,     bool overline,
        tag::value::line_through, bool line_through,
        tag::value::blink,        bool blink);
    };

**enable-background** property
  Значения **accumulate**, **new** and **inherit** передаются как *Literal Values* (см. выше).
  Значения вида **new <x> <y> <width> <height>** передаются пятью параметрами, первое - *tag*, 
  остальные имеют тип number_type_ (по умолчанию ``double``): 
  ``(tag::value::new_(), x, y, width, height)``.

*<paint>*
  Возможные комбинации параметров:

    * (``tag::value::inherit``)
    * (``tag::value::none``)
    * (``tag::value::currentColor``)
    * (*<color>*)
    * (*<color>*, *<icccolor>*)
    * (*<iri>*, ``tag::value::none``)
    * (*<iri>*, ``tag::value::currentColor``)
    * (*<iri>*, *<color>*)
    * (*<iri>*, *<color>*, *<icccolor>*)

  Как определяется тип *<color>* и *<icccolor>* описано выше.

  Если в качестве :ref:`IRI Policy <iri-section>` используется ``policy::iri::distinguish_local``,
  то число методов с *<iri>* удваивается:

    * (``tag::value::inherit``)
    * (``tag::value::none``)
    * (``tag::value::currentColor``)
    * (*<color>*)
    * (*<color>*, *<icccolor>*)
    * (*<iri>*, ``tag::value::none``)
    * (``tag::iri_fragment``, *<iri fragment>*, ``tag::value::none``)
    * (*<iri>*, ``tag::value::currentColor``)
    * (``tag::iri_fragment``, *<iri fragment>*, ``tag::value::currentColor``)
    * (*<iri>*, *<color>*)
    * (``tag::iri_fragment``, *<iri fragment>*, *<color>*)
    * (*<iri>*, *<color>*, *<icccolor>*)
    * (``tag::iri_fragment``, *<iri fragment>*, *<color>*, *<icccolor>*)

  Пример::

    typedef boost::variant<tag::value::none, tag::value::currentColor, int/* rgba */> SolidPaint;

    struct IRIPaint
    {
      IRIPaint(
        std::string const & fragment, 
        boost::optional<SolidPaint> const & fallback = boost::optional<SolidPaint>());
    };

    typedef boost::variant<SolidPaint, IRIPaint> Paint;

    template<class AttributeTag>
    class PaintContext
    {
    public:
      void set(AttributeTag, tag::value::none)
      { 
        m_paint = tag::value::none(); 
      }

      void set(AttributeTag, tag::value::currentColor)
      { 
        m_paint = tag::value::currentColor(); 
      }

      void set(AttributeTag, int color, tag::skip_icc_color = tag::skip_icc_color())
      { 
        m_paint = color; 
      }

      template<class IRI>
      void set(AttributeTag tag, IRI const & iri)
      { 
        throw std::runtime_error("Non-local references aren't supported");
      }

      template<class IRI>
      void set(AttributeTag tag, tag::iri_fragment, IRI const & fragment)
      { 
        m_paint = IRIPaint(std::string(boost::begin(fragment), boost::end(fragment))); 
      }

      template<class IRI>
      void set(AttributeTag tag, IRI const &, tag::value::none val)
      { 
        // Ignore non-local IRI, fallback to second option
        set(tag, val); 
      }

      template<class IRI>
      void set(AttributeTag tag, tag::iri_fragment, IRI const & fragment, tag::value::none val)
      { 
        m_paint = IRIPaint(std::string(boost::begin(fragment), boost::end(fragment)), boost::optional<SolidPaint>(val)); 
      }

      template<class IRI>
      void set(AttributeTag tag, IRI const &, tag::value::currentColor val)
      { 
        // Ignore non-local IRI, fallback to second option
        set(tag, val); 
      }

      template<class IRI>
      void set(AttributeTag tag, tag::iri_fragment, IRI const & fragment, tag::value::currentColor val)
      { 
        m_paint = IRIPaint(std::string(boost::begin(fragment), boost::end(fragment)), boost::optional<SolidPaint>(val)); 
      }

      template<class IRI>
      void set(AttributeTag tag, IRI const &, agg::rgba8 val, tag::skip_icc_color = tag::skip_icc_color())
      { 
        // Ignore non-local IRI, fallback to second option
        set(tag, val); 
      }

      template<class IRI>
      void set(AttributeTag tag, tag::iri_fragment, IRI const & fragment, int val, tag::skip_icc_color = tag::skip_icc_color())
      { 
        m_paint = IRIPaint(std::string(boost::begin(fragment), boost::end(fragment)), boost::optional<SolidPaint>(val)); 
      }
  
    private:
      Paint m_paint;
    };


