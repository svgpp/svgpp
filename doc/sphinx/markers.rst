Markers
=============

SVG++ предоставляет возможность автоматического расчета положения маркеров на линиях 
(**path**, **line**, **polyline** or **polygon** element).

*Markers Policy* позволяет включать и настраивать автоматический расчет положения маркеров.
*Marker Events Policy* настраивает способ передачи позиции маркеров в пользовательский код.

Markers Policy Concept
------------------------------------

::

  struct makers_policy_concept
  {
    typedef /* ... */ directionality_policy;

    static const bool calculate_markers = /* true or false */;
    static const bool always_calculate_auto_orient = /* true or false */; 
  };

``calculate_markers = true`` включает автоматический расчет положения маркеров. Если ``calculate_markers = false``,
то остальные члены класса не используются.

В зависимости от значений marker properties, не для всех вершин нужно расчитывать 
положение маркеров. А в зависимости от значения **orient** attribute of **marker** element, направление маркера может или 
расчитываться для каждой вершины (**auto**) или задаваться значением атрибута.

  ``always_calculate_auto_orient = true`` 
    В этом случае вычисляется положение маркера и направление в режиме **orient="auto"** для каждой вершины.

  ``always_calculate_auto_orient = false``
    В этом случае, в начале обработки каждого path вызывается метод ``marker_get_config`` *Marker Events Policy*,
    который запрашивает у пользовательского кода необходимость расчета положения и направления маркера. Позиции
    маркеров возвращаются только для запрошенных вершин.

``directionality_policy`` определяет как вычисляется направление маркера. По умолчанию направление маркера - 
это значение типа ``double``, содержащее угол в радианах.

В файле ``svgpp/policy/markers.hpp`` определены несколько predefined вариантов *Markers Policy*: 
``policy::markers::calculate_always``, ``policy::markers::calculate`` и ``policy::markers::raw``.
``policy::markers::raw`` используется по умолчанию и выключает автоматический расчет маркеров.


Marker Events Policy Concept
----------------------------------

::

  enum marker_vertex { marker_start, marker_mid, marker_end };
  enum marker_config { marker_none, marker_orient_fixed, marker_orient_auto };

  struct marker_events_policy_concept
  {
    typedef /* ... */ context_type;

    static void marker_get_config(context_type & context, 
      marker_config & start, marker_config & mid, marker_config & end);

    static void marker(context_type & context, marker_vertex v, 
      number_type x, number_type y, directionality_type directionality, unsigned marker_index);
    static void marker(context_type & context, marker_vertex v, 
      number_type x, number_type y, tag::orient_fixed directionality, unsigned marker_index);
  };

Метод ``marker_get_config`` вызывается, если ``makers_policy::always_calculate_auto_orient = false``. 
Пользовательский код должен вернуть для каждого типа вершин (``start``, ``mid`` and ``end``), хочет ли он расчитывать
угол и позицию (``marker_config::marker_orient_auto``), только позицию (``marker_config::marker_orient_fixed``)
или ни то ни другое (``marker_config::marker_none``).

Метод ``marker`` вызывается для каждой расчитанной вершины:

  ``x`` и ``y`` - координаты вершины

  ``v`` - тип вершины (``marker_start``, ``marker_mid`` или ``marker_end``)

  ``directionality`` - направление маркера (по умолчанию тип ``double``, значение в радианах).
  Если ``marker_get_config`` вернул ``marker_orient_fixed`` для вершин этого типа, то вместо значения направления
  передается tag ``tag::orient_fixed``.

  ``marker_index``
    Направление маркеров расчитывается и возвращается не всегда по порядку. Направление маркера первой вершины *subpath* 
    может быть определено только после окончания обработки этого *subpath*, поэтому сначала будут возвращены позиции 
    промежуточных вершин, а только потом позиция начальной вершины.

    ``marker_index`` соответствует порядку, в котором вершины должны отрисовываться (0-based).


*Marker Events Policy* по умолчанию (``policy::marker_events::forward_to_method``) переадресует вызовы статических методов 
методам объекта ``context``.

Пример::

  class Context
  {
  public:
    void marker(svgpp::marker_vertex v, double x, double y, double directionality, unsigned marker_index)
    {
      if (markers_.size() <= marker_index)
        markers_.resize(marker_index + 1);
      auto & m = markers_[marker_index];
      m.v = v;
      m.x = x;
      m.y = y;
      m.directionality = directionality;
    }

  private:
    struct MarkerPosition
    {
      svgpp::marker_vertex v;
      double x, y, directionality;
    };

    std::vector<MarkerPosition> markers_;
  };

  void load()
  {
    document_traversal<
      /* ... */
      svgpp::markers_policy<svgpp::policy::markers::calculate_always>
    >::load_document(xml_root_element, context);
  }