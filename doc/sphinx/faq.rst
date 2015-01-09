FAQ
==========

Compiler error: "Too many template arguments for class template 'document_traversal'"
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

Increase BOOST_PARAMETER_MAX_ARITY value from default ``8`` value. Place define like this before any Boost or SVG++ include::

  #define BOOST_PARAMETER_MAX_ARITY 15


SVG++ stops parsing on incorrect attribute value and throws exception
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

SVG Specification says "The document shall be rendered up to, but not including, the first element which has an error". 
Therefore such behavior is correct, it will be better to fix SVG document.

But if you want to continue rendering document ignoring error in an attribute, you should inherit your class 
from ``policy::error::raise_exception``, override its ``parse_failed`` method to not throw exception, 
but return ``true`` instead::

  struct custom_error_policy: svgpp::policy::error::raise_exception<BaseContext>
  {
      template<class AttributeTag, class AttributeValue>
      static bool parse_failed(BaseContext const &, AttributeTag,
                               AttributeValue const & value)
      {
          return true;
      }
  };

where ``BaseContext`` is common base class for all contexts.
Then pass this new class as template parameter ``error_policy`` to ``document_traversal`` class::

  document_traversal<
    error_policy<custom_error_policy>
    /* ... */
  >::/* ... */