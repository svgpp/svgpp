.. _iri-section:

IRI
=====

`IRIs <http://www.w3.org/TR/SVG/linking.html#IRIandURI>`_, в том числе заданные с помощью `FuncIRI <http://www.w3.org/TR/SVG/types.html#DataTypeFuncIRI>`_
syntax, are passed to user code the same way as other :ref:`strings <passing-string>` in SVG++. 

Единственная возможная настройка в *IRI Policy* определяет, различать ли local references to document fragment (IRIs prefixed with "#").
Если используется ``policy::iri::distinguish_local`` (по умолчанию), то local references to document fragment
передаются как пара параметров: {``tag::iri_fragment()``, *<fragment string>*}. Если задать ``policy::iri::raw``,
то любой IRI передается одним параметром - :ref:`строкой <passing-string>`.

:ref:`Named class template parameter <named-params>` for *IRI Policy* is ``iri_policy``.

Пример использования с *IRI Policy* по умолчанию::

  struct Context
  {
    template<class String>
    void set(tag::attribute::xlink::href, String const & iri)
    {
      std::cout << "IRI: " << std::string(boost::begin(iri), boost::end(iri)) << "\n";
    }

    template<class String>
    void set(tag::attribute::xlink::href, tag::iri_fragment, String const & iri)
    {
      std::cout << "Fragment: " << std::string(boost::begin(iri), boost::end(iri)) << "\n";
    }
  };

  void func()
  {
    Context context;
    value_parser<tag::type::iri>::parse(tag::attribute::xlink::href(), context, 
      std::string("http://foo.com/bar#123"), tag::source::attribute());
    value_parser<tag::type::iri>::parse(tag::attribute::xlink::href(), context, 
      std::string("#rect1"), tag::source::attribute());
    // Output: 
    //  "IRI: http://foo.com/bar#123"
    //  "Fragment: rect1"
  }

