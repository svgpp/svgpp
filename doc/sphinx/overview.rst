.. _Associative Sequence: http://www.boost.org/doc/libs/1_55_0/libs/mpl/doc/refmanual/associative-sequence.html

Library Organization
=============================

Context
---------

Main pattern of SVG++ usage: programmer calls some library function, passing reference to *context*
and reference to XML element or value of XML attribute; library calls static methods of corresponding *Events Policy*,
passing *context* and parsed values as arguments.

*Events Policies* (*Value Events Policy*, *Transform Events Policy*, *Path Events Policy* etc) 
know how to pass value to corresponding *context* type.
Default *Events Policies* treats *context* as an object, calling its methods. 


Components
-----------------

Below are shown the main SVG++ components, starting from the lowest level. Every following is based on the preceding:
  
  *Grammars*
    Boost.Spirit implementations of grammars in SVG Specification.

  *Value Parsers*
    *Value Parsers* are functions that convert string values of attributes and CSS properties
    to calls of user functions with convenient presentation of SVG data. 
    Some of them use *grammars*. 

    E.g., attribute value **x="16mm"** may be passed as corresponding ``double`` value, 
    taking into account units, and **d="M10 10 L15 100"** may become sequence of 
    calls like ``path_move_to(10,10); path_line_to(15, 100);``.

  *Adapters*
    *Value Parsers* as much as possible saves SVG information, this allows, for example, 
    to use SVG++ for SVG DOM generation. In other applications this information
    may be excessive. SVG++ provides some *adapters*, that may simplify processing of SVG.
    *Adapters* are configured by *policies*.

  *Attribute Dispatcher*
    *Attribute Dispatcher* object is created internally for each element.
    It chooses and calls corresponding *Value Parser* for each attribute.
    Besides that *Attribute Dispatcher* manages adapters that processed several attributes of same element.
    For example, adapter that converts **line** element to **path** must collect values
    of **x1**, **y1**, **x2** and **y2** attributes - this is managed by *Attribute Dispatcher*.

  *Attribute Traversal*
    * *Attribute Traversal* object is created internally for each element and it calls methods of *Attribute Dispatcher*.
    * Finds out numeric id of attribute by its name.
    * Parses **style** attribute, so that values of attributes and values of CSS properties are handled identically.
    * Checks presence of mandatory attributes in element.
    * "Hides" `presentation attribute <http://www.w3.org/TR/SVG/styling.html#UsingPresentationAttributes>`_, 
      if this property is already set in the **style** attribute.
    * Allows attribute ordering.

  *Document Traversal*
    * Traverses SVG document tree, processes elements selected for processing by the programmer.
    * Checks content model, i.e. whether each child element is allowed for this parent.
    * Creates instances of *Attribute Dispatcher* and *Attribute Traversal* and passes processing to them for each element.
    * Passes to the user code child text nodes of SVG elements that are allowed to carry text content by SVG Specification.

*Document Traversal* provides convenient access to entire library and, in most cases, 
it is the only SVG++ component with which programmer interacts.

*Grammars* are quite independent components and can be easily used separately.

*Value Parsers* have simple interface and can be easily called from application if for some reason
traversing of SVG document tree by *Document Traversal* isn't applicable or only few attributes need to be parsed.

*Attribute Traversal* and *Attribute Dispatcher* aren't described in the documentation and unlikely to be used from outside.


.. _tags-section:

Tags
-------

SVG++ widely uses *tag* concept to reference various SVG entities in compile time with 
overload resolution and metaprogramming techniques. *Tag* here is just an empty class.

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

Each SVG element corresponds to tag in ``tag::element`` C++ namespace, and each SVG attribute (or property) 
corresponds to tag in ``tag::attribute`` namespace. 
Attributes from XML namespace **xlink** corresponds to tags in namespace ``tag::attribute::xlink``, 
and attributes from XML namespace **xml** corresponds to tags in namespace ``tag::attribute::xml``. 
There are also other tags, described in other parts of the documentation.


.. _named-params:

Named Class Template Parameters
---------------------------------

SVG++ widely uses
`named class template parameters <http://www.boost.org/doc/libs/1_56_0/libs/parameter/doc/html/index.html#class-template-parameter-support>`_ 
for compile-time library configuration. It looks like this::

  svgpp::document_traversal<
    svgpp::length_policy<SomeUserLengthPolicy>,
    svgpp::path_policy<SomeUserPathPolicy>
    /* ... */
  >::load_document(/* ... */);

In this example ``SomeUserLengthPolicy`` type is passed as ``length_policy`` parameter,
and ``SomeUserPathPolicy`` type is passed as ``path_policy`` parameter.

Named class template parameters are passed through SVG++ components down to *Value Parsers* level.

Library Customization
--------------------------

*Policies* allows customization of most library aspects. There are two ways of setting *policy*:

1. Pass policy as a named class template parameter. For example::
  
    document_traversal<
      length_policy<UserLengthPolicy>
    >::load_document(/* ... */);

2. Create specialization of class ``default_policy`` for the *context* type in proper C++ namespace::

    namespace svgpp { namespace policy { namespace length
    {
      template<>
      struct default_policy<UserContext>: UserLengthPolicy
      {};
    }}}

.. _xml-parser:

XML Parser
-------------

SVG++ uses external XML parsing libraries. 
Interaction with XML parser is handled by specialization of *XML Policy* classes.

``XMLElement`` template parameter is used to automatically choose *XML Policy* for XML parser used. 

Programmer must include XML parser library header files, after that include 
header file of corresponding *XML Policy* from SVG++ and only after that include other SVG++ headers. For example::

  #include <rapidxml_ns/rapidxml_ns.hpp>
  #include <svgpp/policy/xml/rapidxml_ns.hpp>
  #include <svgpp/svgpp.hpp>

.. _xml_policy_types:

Below are XML parsing libraries supported by SVG++, their respective *XML Policy* header files 
and expected XMLElement type:

+--------------------------+-----------------------------------------------+-------------------------------------------+
|XML Parser Library        | Policy header                                 | XMLElement template parameter             |
+==========================+===============================================+===========================================+
|RapidXML NS               | <svgpp/policy/xml/rapidxml_ns.hpp>            | ``rapidxml_ns::xml_node<Ch> const *``     |
+--------------------------+-----------------------------------------------+-------------------------------------------+
|libxml2                   | <svgpp/policy/xml/libxml2.hpp>                | ``xmlNode *``                             |
+--------------------------+-----------------------------------------------+-------------------------------------------+
|MSXML                     | <svgpp/policy/xml/msxml.hpp>                  | ``IXMLDOMElement *``                      |
+--------------------------+-----------------------------------------------+-------------------------------------------+
|Xerces                    | <svgpp/policy/xml/xerces.hpp>                 | ``xercesc::DOMElement const *``           |
+--------------------------+-----------------------------------------------+-------------------------------------------+


.. _passing-string:

Strings
------------

SVG++ supports different character types - ``char`` and ``wchar_t``, and on supporting compilers
``char16_t`` and ``char32_t``. Character type is defined by XML parsing library used.

Strings are passed to the user code by some unspecified model of
`Forward Range <http://www.boost.org/doc/libs/1_56_0/libs/iterator/doc/new-iter-concepts.html#forward-traversal-iterators-lib-forward-traversal-iterators>`_
concept. Example of processing::

  struct Context
  {
    template<class Range>
    void set(svgpp::tag::attribute::result, Range const & r)
    {
      std::string value;
      value_.assign(boost::begin(r), boost::end(r));
    }
  };

If template function can't be used (e.g. it is virtual function), then 
`boost::any_range <http://www.boost.org/doc/libs/1_56_0/libs/range/doc/html/range/reference/ranges/any_range.html>`_
can be used as string range type.


CSS Support
----------------

SVG++ parses properties in **style** attribute, if **style** processing is :ref:`enabled <parse_style>` 
by the programmer.

SVG++ doesn't support CSS cascading and CSS stylesheet in **style** element. It may be handled, if needed,
by some other component, that provides result as **style** attribute.
