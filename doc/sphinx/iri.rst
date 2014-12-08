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

Пример использования с *IRI Policy* по умолчанию (``src/samples/sample_iri.cpp``):
  
.. literalinclude:: ../../src/samples/sample_iri.cpp 

