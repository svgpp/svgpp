.. _Associative Sequence: http://www.boost.org/doc/libs/1_56_0/libs/mpl/doc/refmanual/associative-sequence.html

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

Тип ``convert_to_path`` - это `Associative Sequence`_ (например, ``boost::mpl::set``), содержащая тэги элементов
SVG, которые будут преобразованы в **path**.  Генерируемые **path** используют настройки *Path Policy* и *Load Path Policy*.

Тип ``collect_attributes`` - это `Associative Sequence`_, содержащая тэги элементов 
**rect**, **circle**, **ellipse** и **line**, геометрия которых будет передана одним вызовом, вместо того, чтобы
передавать значения атрибутов по отдельности.

Если static member constant ``convert_only_rounded_rect_to_path`` имеет значение ``true`` и 
``tag::element::rect`` входит в ``convert_to_path``, то преобразованы в **path* будут только rounded rectangles,
а обычные прямоугольники будут обработаны как будто ``tag::element::rect`` входит в ``collect_attributes``.

В ``document_traversal`` должны быть разрешена обработка атрибутов, описывающих геометрию *basic shapes* (**x**, **y**,
**r** и т. д.), т. е. они должны быть включены в ``processed_elements`` или не включены в ``ignored_elements``.
`Associative Sequence`_ ``traits::shapes_attributes_by_element`` содержит список таких атрибутов для всех *basic shapes*.

:ref:`Named class template parameter <named-params>` for *Basic Shapes Policy* is ``basic_shapes_policy``.


Load Basic Shapes Policy Concept
-------------------------------------

::

  struct load_basic_shapes_policy
  {
    static void set_rect(Context & context, Coordinate x, Coordinate y, Coordinate width, Coordinate height,
      Coordinate rx, Coordinate ry);
    static void set_rect(Context & context, Coordinate x, Coordinate y, Coordinate width, Coordinate height);
    static void set_line(Context & context, Coordinate x1, Coordinate y1, Coordinate x2, Coordinate y2);
    static void set_circle(Context & context, Coordinate cx, Coordinate cy, Coordinate r);
    static void set_ellipse(Context & context, Coordinate cx, Coordinate cy, Coordinate rx, Coordinate ry);
  };

*Load Basic Shapes Policy* используется для тех элементов *basic shapes* (кроме **polyline** и **polygon**), 
тэги которых перечислены в ``collect_attributes`` *Basic Shapes Policy*.

Адаптеры, реализующие эти преобразования, используют метод ``length_to_user_coordinate`` *Length Factory*,
чтобы получить численные *user coordinates* из *length*. Эти адаптеры передают значения по умолчанию при отсутствии
атрибута и проверяют корректность значений атрибутов. Если значение disables rendering of the element
в соответствии со стандартом SVG, то функция *Load Basic Shapes Policy* не вызывается, если атрибут
имеет отрицательное значение ошибка для этого элемента в соответствии со стандартом, то вызывается 
функция ``negative_value`` *Error Policy*.

*Load Basic Shapes Policy* по умолчанию (``policy::load_basic_shapes::forward_to_method``) переадресует вызовы 
статических методов к методам объекта ``context``::

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

:ref:`Named class template parameter <named-params>` for *Load Basic Shapes Policy* is ``load_basic_shapes_policy``.