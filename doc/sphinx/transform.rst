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

Пример с использованием настроек по умолчанию (``src/samples/sample_transform01.cpp``):
  
.. literalinclude:: ../../src/samples/sample_transform01.cpp 

Пример, в котором умножение последовательных transforms происходит в user code (``src/samples/sample_transform02.cpp``):

.. literalinclude:: ../../src/samples/sample_transform02.cpp 


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