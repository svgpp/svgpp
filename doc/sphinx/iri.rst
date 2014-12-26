.. _iri-section:

IRI
=====

`IRIs <http://www.w3.org/TR/SVG/linking.html#IRIandURI>`_, including ones that are set with
`FuncIRI <http://www.w3.org/TR/SVG/types.html#DataTypeFuncIRI>`_
syntax, are passed to the user code the same way as other :ref:`strings <passing-string>` in SVG++. 

The only setting in *IRI Policy* configures whether to distinguish
local references to document fragment (IRIs prefixed with "#") from non-local IRI references. 
If ``policy::iri::distinguish_local`` used (as by default), then local reference to document fragment
is passed as pair of parameters: {``tag::iri_fragment()``, *<fragment string>*}. 
If ``policy::iri::raw`` is set, then any IRI is passed as single :ref:`string <passing-string>`.

:ref:`Named class template parameter <named-params>` for *IRI Policy* is ``iri_policy``.

Example of using default *IRI Policy* (``src/samples/sample_iri.cpp``):
  
.. literalinclude:: ../../src/samples/sample_iri.cpp 

