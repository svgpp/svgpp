.. _transform-section:

Transform 
=================

Transformation parsing is controlled by *Transform Policy* и *Transform Events Policy*. *Transform Policy* задает настройки адаптера, 
который упрощает для приложения работу с coordinate system transformations (например, адаптер может 
заменять все simple transformation steps translate/scale/rotate/skew на соответствующие tranformation matrices, более удобные
во многих случаях). *Transform Events Policy* определяет, как разобранные данные передаются объекту контекста.


Transform Events Policy Concept
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

*Transform Events Policy* зависит от compile-time настроек, задаваемых посредством *Transform Policy*. 
Если используются настройки ``policy::transform::raw``, максимально сохраняющие структуру входных данных, 
то *Transform Events Policy* имеет вид::

  struct transform_events_policy
  {
    typedef /*...*/ context_type; 

    static void transform_matrix(context_type & context, const boost::array<number_type, 6> & matrix);
    static void transform_translate(context_type & context, number_type tx, number_type ty);
    static void transform_translate(context_type & context, number_type tx);
    static void transform_scale(context_type & context, number_type sx, number_type sy);
    static void transform_scale(context_type & context, number_type scale);
    static void transform_rotate(context_type & context, number_type angle);
    static void transform_rotate(context_type & context, number_type angle, number_type cx, number_type cy);
    static void transform_skew_x(context_type & context, number_type angle);
    static void transform_skew_y(context_type & context, number_type angle);
  };

При других конфигурациях *Transform Policy* могут использоваться не все методы. 

.. note::
  Transformation matrix передается как массив из шести элементов ``[a b c d e f]``, соответствующих
  такой матрице:

  .. image:: http://www.w3.org/TR/SVG11/images/coords/Matrix.png

:ref:`Named class template parameter <named-params>` for *Transform Events Policy* is ``transform_events_policy``.

*Transform Events Policy* по умолчанию (``policy::transform_events::forward_to_method``) переадресует вызовы статических методов 
методам объекта ``context``::

  struct forward_to_method
  {
    typedef Context context_type; 

    template<class Number>
    static void transform_matrix(context_type & context, const boost::array<Number, 6> & matrix)
    {
      context.transform_matrix(matrix);
    }

    /*...*/
  };

Пример с использованием настроек по умолчанию::
  
  #include <svgpp/svgpp.hpp>

  struct Context
  {
    void transform_matrix(const boost::array<double, 6> & matrix)
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
  #include <boost/math/constants/constants.hpp>
  #include <boost/numeric/ublas/matrix.hpp>
  #include <boost/numeric/ublas/io.hpp>

  namespace ublas = boost::numeric::ublas;

  typedef ublas::scalar_matrix<double> matrix_t;

  struct transform_events_policy
  {
    typedef matrix_t context_type;

    static void transform_matrix(matrix_t & transform, const boost::array<number_type, 6> & matrix)
    {
      matrix_t m(3, 3);
      m(0, 0) = matrix[0]; m(1, 0) = matrix[1]; m(0, 1) = matrix[2]; 
      m(1, 1) = matrix[3]; m(0, 2) = matrix[4]; m(1, 2) = matrix[5]; m(2, 2) = 1.0;
      transform = ublas::prod(transform, matrix);
    }

    static void transform_translate(matrix_t & transform, number_type tx, number_type ty)
    {
      matrix_t m = ublas::identity_matrix(3, 3);
      m(0, 2) = tx; m(1, 2) = ty; 
      transform = ublas::prod(transform, matrix);
    }

    static void transform_scale(matrix_t & transform, number_type sx, number_type sy)
    {
      matrix_t m(3, 3);
      m(0, 0) = sx; m(1, 1) = sy; m(2, 2) = 1; 
      transform = ublas::prod(transform, matrix);
    }

    static void transform_rotate(matrix_t & transform, number_type angle)
    {
      angle *= boost::math::constants::degree<number_type>();
      matrix_t m(3, 3);
      m(0, 0) =  std::cos(angle); m(1, 0) = std::sin(angle); 
      m(0, 1) = -std::sin(angle); m(1, 1) = std::cos(angle); m(2, 2) = 1; 
      transform = ublas::prod(transform, matrix);
    }

    static void transform_skew_x(matrix_t & transform, number_type angle)
    {
      angle *= boost::math::constants::degree<number_type>();
      matrix_t m = ublas::identity_matrix(3, 3);
      m(0, 1) = std::tan(angle);
      transform = ublas::prod(transform, matrix);
    }

    static void transform_skew_y(matrix_t & transform, number_type angle)
    {
      angle *= boost::math::constants::degree<number_type>();
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
      transform_events_policy<transform_events_policy>
    >::parse(tag::attribute::transform(), transform, 
      std::string("translate(-10,-20) scale(2) rotate(45) translate(5,10)"), tag::source::attribute());
    std::cout << context.transform << "\n";
  }


Transform Policy Concept
^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

  struct transform_policy_concept
  {
    static const bool join_transforms = /* true or false */;
    static const bool no_rotate_about_point = /* true or false */;
    static const bool no_shorthands = /* true or false */; 
    static const bool only_matrix_transform = /* true or false */;
  };

Как видно, *Transform Policy* - это класс с набором статических member constants типа bool. Если все они имеют значение ``false``
(как в ``policy::transform::raw``), то адаптер не используется и парсер передает разобранные значения как есть. Устанавливая опции в 
``true`` можно упростить код приложения:

  ``join_transforms = true``
    Все transformations атрибута объединяются в одну transformation matrix. Значения других *Transform Policy* members 
    игнорируются. *Transform Events Policy* в таком случае использует только один метод::

      struct transform_events_policy
      {
        typedef /*...*/ context_type; 

        static void transform_matrix(context_type & context, const boost::array<number_type, 6> & matrix);
      }

  ``no_rotate_about_point = true``
    **rotate(<rotate-angle> <cx> <cy>)** заменяется на **translate(<cx>, <cy>) rotate(<rotate-angle>) translate(-<cx>, -<cy>)**.
    Не используется метод *Transform Events Policy* ``transform_rotate`` с параметрами ``cx`` и ``cy``.

  ``no_shorthands = true``
    Вызовы ``transform_translate`` и ``transform_scale`` с одним числом заменяются на соответствующие
    вызовы с двумя числами.

  ``only_matrix_transform = true``
    Каждый transformation step преобразуется в соответствующих вызов ``transform_matrix``. Соответственно,
    используется только этот метод *Transform Events Policy*.

В файле ``svgpp/policy/transform.hpp`` определены несколько predefined вариантов *Transform Policy*. Используемый по умолчанию
``policy::transform::matrix`` устанавливает ``join_transforms = true``.

:ref:`Named class template parameter <named-params>` for *Transform Policy* is ``transform_policy``.