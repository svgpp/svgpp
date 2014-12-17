.. _viewport-section:

Viewport
================================

When establishing new viewport с использованием элементов **svg**, **symbol** (instanced by **use**) or **image** нужно 
обработать несколько атрибутов (**x**, **y**, **width**, **height**, **preserveAspectRatio**, **viewbox**), чтобы
определить новую user coordinate system и clipping path. SVG++ может сделать это сама и предоставить результат
в удобном виде. Для включения и настройки такой обработки предназначен *Viewport Policy*. 
*Viewport Events Policy* определяет способ передачи значений в пользовательский код.

Элементы **marker** похожим образом устанавливают новую систему координат и clipping path, когда 
отрисовываются в вершинах пути, поэтому маркеры тоже могут обрабатываться посредством *Viewport Policy*
и *Viewport Events Policy*.

Viewport Policy Concept
--------------------------------

::

  struct viewport_policy_concept
  {
    static const bool calculate_viewport        = /* true or false */;
    static const bool calculate_marker_viewport = /* true or false */;
    static const bool viewport_as_transform     = /* true or false */;
  };

``calculate_viewport = true``
  Включает обработку атрибутов, описывающих viewport. Значения будут переданы вызовами методов ``set_viewport`` и 
  ``set_viewbox_transform`` *Viewport Events Policy*.

``calculate_marker_viewport = true``
  То же самое для элементов **marker**.

``viewport_as_transform = true``
  Проверяется только если ``calculate_viewport = true``.
  Метод ``set_viewbox_transform`` *Viewport Events Policy* не используется, вместо него новая система координат
  задается посредством :ref:`Transform Events Policy <transform-section>`.

:ref:`Named class template parameter <named-params>` for *Viewport Policy* is ``viewport_policy``.

В файле ``svgpp/policy/viewport.hpp`` определены несколько predefined вариантов *Viewport Policy*. Используемый по умолчанию
``policy::viewport::raw`` не использует обработку viewport средствами SVG++. 
``policy::viewport::as_transform`` выставляет все булевские члены в ``true``, то есть обрабатывает атрибуты viewport 
и передает изменение системы координат посредством *Transform Events Policy*.


Viewport Events Policy Concept
--------------------------------

::

  struct viewport_events_policy_concept
  {
    static void set_viewport(context_type & context, number_type viewport_x, number_type viewport_y, 
      number_type viewport_width, number_type viewport_height);

    static void set_viewbox_transform(context_type & context, number_type translate_x, number_type translate_y, 
      number_type scale_x, number_type scale_y, bool defer);

    static void get_reference_viewport_size(context_type & context, 
      number_type & viewport_width, number_type & viewport_height);
  };

``set_viewport`` 
  Передает положение нового viewport.

``set_viewbox_transform``
  Передает сдвиг и масштабирование, заданные комбинацией атрибутов **viewbox** и **preserveAspectRatio**. 
  Не используется, если ``viewport_as_transform = true``.

``get_reference_viewport_size``
  См. ниже :ref:`viewport_referenced_element`.

:ref:`Named class template parameter <named-params>` for *Viewport Events Policy* is ``viewport_events_policy``.

.. _viewport_referenced_element:

Ссылающийся элемент
------------------------

При обработке **svg** referenced by **use** or **image**, **symbol** referenced by **use** нужно знать ширину и высоту,
заданные ссылающимся элементом. 

Для того, чтобы SVG++ получила информацию, что загружается элемент referenced by **use** or **image**,
надо передать тэг ссылающегося элемента через параметр :ref:`referencing_element <referencing_element>` ``document_traversal``.
В этом случае будет вызываться ``get_reference_viewport_size``.
В реализации ``get_reference_viewport_size`` нужно присвоить ``viewport_width``
значение атрибута **width** ссылающегося элемента, если этот атрибут присутствует. 
И присвоить ``viewport_height`` значение атрибута **height** ссылающегося элемента, если этот атрибут присутствует.


Обрабатываемые атрибуты
--------------------------------

Если в *Viewport Policy* ``calculate_viewport = true``, то SVG++ перехватывает и обрабатывает атрибуты, 
перечисленные в ``traits::viewport_attributes``::

  namespace traits 
  {
    typedef boost::mpl::set6<
      tag::attribute::x, 
      tag::attribute::y, 
      tag::attribute::width, 
      tag::attribute::height, 
      tag::attribute::viewBox, 
      tag::attribute::preserveAspectRatio
    > viewport_attributes;
  }

Если в *Viewport Policy* ``calculate_marker_viewport = true``, то SVG++ перехватывает и обрабатывает атрибуты маркера, 
перечисленные в ``traits::marker_viewport_attributes``::

  namespace traits 
  {
    typedef boost::mpl::set6<
      tag::attribute::refX, 
      tag::attribute::refY, 
      tag::attribute::markerWidth, 
      tag::attribute::markerHeight, 
      tag::attribute::viewBox, 
      tag::attribute::preserveAspectRatio
    > marker_viewport_attributes;
  }

Обработка этих атрибутов должна быть :ref:`разрешена <processed_attributes>` пользователем.

Порядок обработки атрибутов
-------------------------------------

Атрибуты будут обработаны и результат передан посредством *Viewport Events Policy* после обработки всех атрибутов 
элемента SVG или по приходу нотификации с тэгом ``tag::event::after_viewport_attributes``.