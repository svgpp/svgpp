.. _Associative Sequence: http://www.boost.org/doc/libs/1_55_0/libs/mpl/doc/refmanual/associative-sequence.html

Организация библиотеки
=============================

Context
---------

Основная схема разбора SVG с помощью SVG++: программист вызывает какую-то функцию библиотеки, передавая ссылку на *context*
и XML элемент или XML атрибут, а библиотека вызывает статические методы соответствующего *Load Policy*, 
передавая этот же *context* и разобранные значения в виде параметров.

*Load Policies* (*Load Value Policy*, *Load Transform Policy*, *Load Path Policy* etc) определяют какой тип должен иметь *context*.
*Load Policies* по умолчанию используют *context* как объект, вызывая его методы. 



Компоненты
-----------------

Перечислим главные компоненты библиотеки SVG++, начиная с нижнего уровня, каждый следующий базируется на предыдущих:

  *Value Parsers*
    В основе SVG++ лежат *Value Parsers* - функции, преобразующие строковые значения атрибутов и CSS properties
    в вызовы пользовательских функций с удобными для обработки параметрами.

    Например, значение атрибута **x="16mm"** может быть преобразовано в соответствующее ``double`` значение, 
    учитывающее единицы измерения, 
    а **d="M10 10 L15 100"** может стать последовательностью вызовов ``path_move_to(10,10); path_line_to(15, 100);``.

  *Adapters*
    *Value Parsers* максимально сохраняют структуру данных SVG, что позволяет, например, использовать SVG++ для
    построения SVG DOM, но в других приложениях эта информация может быть избыточной. 
    SVG++ предоставляет ряд *Adapters*, которые позволяют упростить программисту обработку данных SVG.
    *Adapters* конфигурируются посредстовом *policies*.

  *Attribute Dispatcher*
    Объект *Attribute Dispatcher* создается на время обработки атрибутов одного элемента.
    Он выбирает и вызывает для каждого атрибута соответствующий *Value Parser*.
    Кроме того, *Attribute Dispatcher* организует работу адаптеров, обрабатывающих несколько атрибутов
    одного объекта. Например, адаптер, преобразующий элемент **line** в **path**, должен собрать
    значения атрибутов **x1**, **y1**, **x2** и **y2** - этим управляет *Attribute Dispatcher*.

  *Attribute Traversal*
    Объект *Attribute Traversal* создается на время обработки атрибутов одного элемента и вызывает
    методы *Attribute Dispatcher*. 
    
    *Attribute Traversal* определяет по имени атрибута его внутренний числовой идентификатор.

    *Attribute Traversal* осуществляет разбор атрибута **style**, так что в последующем значения атрибутов
    и значения CSS properties обрабатываются аналогично.

    *Attribute Traversal* проверяет присутствие обязательных атрибутов элемента.

    *Attribute Traversal* "прячет" presentation attribute, если это же property задано в CSS.

    *Attribute Traversal* позволяет задать порядок обработки атрибутов.

  *Document Traversal*
    Набор статических методов. Осуществляет обход дерева SVG документа, обрабатывая выбранные программистом элементы SVG.

    *Document Traversal* проверяет content model, то есть допустимость появления дочерних элементов.

    *Document Traversal* создает экземпляры и вызывает методы *Attribute Dispatcher* и *Attribute Traversal* для атрибутов элемента.

    *Document Traversal* передает child text nodes тех элементов SVG, которые могут иметь text content, в user code.


*Document Traversal* предоставляет удобный доступ ко всем возможностям библиотеки и, в большинстве случаев, его и нужно 
использовать.

*Value Parsers* имеют простой интерфейс и могут быть легко подключены в приложение, если по каким-то причинам обход 
дерева SVG, предоставляемый *Document Traversal*, не нужен или достаточно разобрать отдельные атрибуты.

*Attribute Traversal* и *Attribute Dispatcher* не имеют отдельного описания в документации и вряд ли будут использованы извне.



.. _tags-section:

Tags
-------

В библиотеке SVG++ для обозначения многих сущностей SVG используется концепция тэгов - пустых структур, которые используются in compile time with overload resolution and metaprogramming techniques. 

::

  namespace tag 
  { 
    namespace element
    {
      struct any {};    // Common base for all element tags. Made for convenience
      struct a: any {};
      struct altGlyph: any {};
      // ...
      struct vkern: any {};
    }

    namespace attribute
    {
      struct accent_height {};
      struct accumulate {};
      // ...
      struct zoomAndPan {};

      namespace xlink 
      {
        struct actuate {};
        struct arcrole {};
        // ...
        struct type {};
      }
    }
  }

Каждому элементу SVG соответствует тэг из пространства имен ``tag::element``, а каждому атрибуту SVG - тэг из пространства 
имен ``tag::attribute``. Атрибутам из XML namespace **xlink** соответствуют тэги in C++ namespace ``tag::attribute::xlink``, 
а атрибутам из XML namespace **xml** - тэги in C++ namespace ``tag::attribute::xml``. 
Есть и иные тэги, которые описаны в других местах документации.


.. _named-params:

Named Class Template Parameters
---------------------------------

SVG++ широко использует 
`named class template parameters <http://www.boost.org/doc/libs/1_56_0/libs/parameter/doc/html/index.html#class-template-parameter-support>`_ 
для compile-time настройки библиотеки. Заданные named class template parameters передаются между компонентами, например,
named class template parameters, заданные ``document_traversal``, передаются вплоть до ``value_parser``.

Конфигурация библиотеки
--------------------------

*Policies* позволяют настраивать многие аспекты библиотеки. Есть два способа задать *policy*:

1. Передать как named class template parameter. Например::
  
    document_traversal<
      length_policy<UserLengthPolicy>
    >::load(/* ... */);

2. Задать специализацию класса ``default_policy`` для нужного *context* type в соответствующем C++ namespace::

    namespace svgpp { namespace policy { namespace length
    {
      template<>
      struct default_policy<UserContext>: UserLengthPolicy
      {};
    }}}

CSS Support
----------------

SVG++ разбирает properties in **style** attribute, если обработка **style** разрешена программистом
(``tag::attribute::style`` входит в список ``processed_attributes`` или не входит в ``ignored_attributes``).

SVG++ не реализует CSS cascading и обработку CSS stylesheet в элементе **style** - это, при необходимости, должен 
делать другой модуль, предоставляя результат в виде атрибутов **style**.
