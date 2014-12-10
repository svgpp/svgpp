.. _Associative Sequence: http://www.boost.org/doc/libs/1_56_0/libs/mpl/doc/refmanual/associative-sequence.html

.. _basic_shapes:

Basic Shapes
====================

`Basic shapes <http://www.w3.org/TR/SVG/shapes.html>`_ (**rect**, **circle**, **ellipse**, **line**, **polyline**
и **polygon**) могут быть автоматически преобразованы в команды path.

Basic Shapes Policy Concept
--------------------------------

::

  struct basic_shapes_policy
  {
    typedef /* Associative Sequence */ convert_to_path;
    typedef /* Associative Sequence */ collect_attributes;

    static const bool convert_only_rounded_rect_to_path = /* true or false */;
  };

``convert_to_path``
  `Associative Sequence`_ (например, ``boost::mpl::set``), содержащая тэги *basic shapes* элементов
  SVG, которые будут преобразованы в **path**.  Генерируемые **path** используют настройки *Path Policy* и *Path Events Policy*.

``collect_attributes``
  `Associative Sequence`_, содержащая тэги элементов 
  **rect**, **circle**, **ellipse** и **line**, геометрия которых будет передана одним вызовом, вместо того, чтобы
  передавать значения атрибутов по отдельности.

``convert_only_rounded_rect_to_path``
  Если static member constant ``convert_only_rounded_rect_to_path`` имеет значение ``true`` и 
  ``tag::element::rect`` входит в ``convert_to_path``, то преобразованы в **path** будут только rounded rectangles,
  а обычные прямоугольники будут обработаны как будто ``tag::element::rect`` входит в ``collect_attributes``.

В ``document_traversal`` должна быть :ref:`разрешена <processed_attributes>` 
обработка атрибутов, описывающих геометрию *basic shapes* (**x**, **y**,
**r** и т. д.), т. е. они должны быть включены в ``processed_attributes`` или не включены в ``ignored_attributes``.
`Associative Sequence`_ ``traits::shapes_attributes_by_element`` содержит список таких атрибутов для всех *basic shapes*.

:ref:`Named class template parameter <named-params>` for *Basic Shapes Policy* is ``basic_shapes_policy``.


Basic Shapes Events Policy Concept
-------------------------------------

::

  struct basic_shapes_events_policy
  {
    static void set_rect(Context & context, Coordinate x, Coordinate y, Coordinate width, Coordinate height,
      Coordinate rx, Coordinate ry);
    static void set_rect(Context & context, Coordinate x, Coordinate y, Coordinate width, Coordinate height);
    static void set_line(Context & context, Coordinate x1, Coordinate y1, Coordinate x2, Coordinate y2);
    static void set_circle(Context & context, Coordinate cx, Coordinate cy, Coordinate r);
    static void set_ellipse(Context & context, Coordinate cx, Coordinate cy, Coordinate rx, Coordinate ry);
  };

*Basic Shapes Events Policy* используется для тех элементов *basic shapes* (кроме **polyline** и **polygon**), 
тэги которых перечислены в ``collect_attributes`` *Basic Shapes Policy*.

Адаптеры, реализующие эти преобразования, используют метод ``length_to_user_coordinate`` *Length Factory*,
чтобы получить численные *user coordinates* из *length*. Эти адаптеры передают значения по умолчанию при отсутствии
атрибута и проверяют корректность значений атрибутов. Если значение disables rendering of the element
в соответствии со спецификацией SVG, то функция *Basic Shapes Events Policy* не вызывается, а если 
отрицательное значение недопустимо для этого атрибута в соответствии со стандартом, то вызывается 
функция ``negative_value`` :ref:`Error Policy <error_policy>`.

*Basic Shapes Events Policy* по умолчанию (``policy::basic_shapes_events::forward_to_method``) переадресует вызовы 
статических методов методам объекта ``context``::

  struct forward_to_method
  {
    template<class Coordinate>
    static void set_rect(Context & context, Coordinate x, Coordinate y, Coordinate width, Coordinate height,
      Coordinate rx, Coordinate ry)
    {
      context.set_rect(x, y, width, height, rx, ry);
    }

    /*...*/
  };

:ref:`Named class template parameter <named-params>` for *Basic Shapes Events Policy* is ``basic_shapes_events_policy``.
