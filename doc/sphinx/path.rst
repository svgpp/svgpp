.. _path_section:

Path
==========

Path parsing is controlled by *Path Policy* и *Path Events Policy*. *Path Policy* задает настройки адаптера, который упрощает для 
приложения работу с paths (например, адаптер может заменять относительные координаты на абсолютные, избавив программиста от
этой работы). *Path Events Policy* определяет, как разобранные данные передаются объекту контекста.

Path Events Policy Concept
--------------------------

::

  struct path_events_concept
  {
    typedef /*...*/ context_type;

    static void path_move_to(context_type & context, number_type x, number_type y, AbsoluteOrRelative absoluteOrRelative);
    static void path_line_to(context_type & context, number_type x, number_type y, AbsoluteOrRelative absoluteOrRelative);
    static void path_line_to_ortho(context_type & context, number_type coord, bool horizontal, AbsoluteOrRelative absoluteOrRelative);
    static void path_cubic_bezier_to(context_type & context, number_type x1, number_type y1, 
                                          number_type x2, number_type y2, 
                                          number_type x, number_type y, 
                                          AbsoluteOrRelative absoluteOrRelative);
    static void path_cubic_bezier_to(context_type & context, 
                                          number_type x2, number_type y2, 
                                          number_type x, number_type y, 
                                          AbsoluteOrRelative absoluteOrRelative);
    static void path_quadratic_bezier_to(context_type & context, 
                                          number_type x1, number_type y1, 
                                          number_type x, number_type y, 
                                          AbsoluteOrRelative absoluteOrRelative);
    static void path_quadratic_bezier_to(context_type & context, 
                                          number_type x, number_type y, 
                                          AbsoluteOrRelative absoluteOrRelative);
    static void path_elliptical_arc_to(context_type & context, 
                                          number_type rx, number_type ry, number_type x_axis_rotation,
                                          bool large_arc_flag, bool sweep_flag, 
                                          number_type x, number_type y,
                                          AbsoluteOrRelative absoluteOrRelative);
    static void path_close_subpath(context_type & context);
    static void path_exit(context_type & context);
  };

В качестве параметра ``absoluteOrRelative`` может быть передан объект типа ``tag::coordinate::absolute`` или типа 
``tag::coordinate::relative`` в зависимости от того абсолютные или относительные координаты переданы в качестве
параметров.

В зависимости от *Path Policy*, не все перечисленные методы могут использоваться и, соответственно, не все может быть необходимо
реализовывать.

*Path Events Policy* по умолчанию (``policy::path_events::forward_to_method``) переадресует вызовы статических методов 
методам объекта ``context``::

  struct forward_to_method
  {
    typedef Context context_type; 

    template<class Coordinate, class AbsoluteOrRelative>
    static void path_move_to(Context & context, Coordinate x, Coordinate y, AbsoluteOrRelative absoluteOrRelative)
    { 
      context.path_move_to(x, y, absoluteOrRelative); 
    }

    /*...*/
  };

:ref:`Named class template parameter <named-params>` for *Path Events Policy* is ``path_events_policy``.

Path Policy Concept
------------------------

::

  struct path_policy_concept
  {
    static const bool absolute_coordinates_only     = /* true or false */;
    static const bool no_ortho_line_to              = /* true or false */;
    static const bool no_quadratic_bezier_shorthand = /* true or false */;
    static const bool no_cubic_bezier_shorthand     = /* true or false */;
    static const bool quadratic_bezier_as_cubic     = /* true or false */;
    static const bool arc_as_cubic_bezier           = /* true or false */; 
  };

Как видно, *Path Policy* - это класс с набором статических member constants типа bool. Если все они имеют значение ``false``
(как в ``policy::path::raw``), то адаптер не используется и парсер передает разобранные значения как есть. Устанавливая опции в 
``true`` можно упростить код приложения:

  ``absolute_coordinates_only = true`` 
    Относительные координаты заменяются на соответствующие им абсолютные. 
    Соответственно, методы *Path Events Policy* с параметром ``tag::coordinate::relative`` не используются.

  ``no_ortho_line_to = true`` 
    Вместо ``path_line_to_ortho`` с одной координатой используется соответствующий 
    вызов ``path_line_to`` с двумя.

  ``no_quadratic_bezier_shorthand = true``
    Вместо ``path_quadratic_bezier_to`` с двумя координатами (shorthand/smooth curve) 
    используется соответствующий вызов с четырьмя.

  ``no_cubic_bezier_shorthand = true`` 
    Вместо ``path_cubic_bezier_to`` с четырьмя координатами (shorthand/smooth curve) 
    используется соответствующий вызов с шестью.

  ``quadratic_bezier_as_cubic = true`` 
    ``path_quadratic_bezier_to`` заменяется на соответствующие вызовы ``path_cubic_bezier_to``.

  ``arc_as_cubic_bezier = true`` 
    Elliptical arc аппроксимируется кубической кривой Bézier. Вызов ``path_elliptical_arc_to`` 
    заменяется серией вызовов ``path_cubic_bezier_to``.

:ref:`Named class template parameter <named-params>` for *Path Policy* is ``path_policy``.

В файле ``svgpp/policy/path.hpp`` определены несколько predefined вариантов *Path Policy*. Используемый по умолчанию
``policy::path::no_shorthands`` максимально сокращает интерфейс *Path Events Policy*, но не использует аппроксимацию.
Для него *Path Events Policy* имеет вид::

  struct path_events_no_shorthands_concept
  {
    typedef /*...*/ context_type;

    static void path_move_to(context_type & context, number_type x, number_type y, tag::coordinate::absolute);
    static void path_line_to(context_type & context, number_type x, number_type y, tag::coordinate::absolute);
    static void path_cubic_bezier_to(context_type & context, number_type x1, number_type y1, 
                                          number_type x2, number_type y2, 
                                          number_type x, number_type y, 
                                          tag::coordinate::absolute);
    static void path_quadratic_bezier_to(context_type & context, 
                                          number_type x1, number_type y1, 
                                          number_type x, number_type y, 
                                          tag::coordinate::absolute);
    static void path_elliptical_arc_to(context_type & context, 
                                          number_type rx, number_type ry, number_type x_axis_rotation,
                                          bool large_arc_flag, bool sweep_flag, 
                                          number_type x, number_type y,
                                          tag::coordinate::absolute);
    static void path_close_subpath(context_type & context);
    static void path_exit(context_type & context);
  };

Собственную реализации *Path Policy* лучше наследовать от какой-нибудь из предоставляемых SVG++ для обеспечения совместимости
в будущем.