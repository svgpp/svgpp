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

.. _external-parser_section:

Compiler is out of memory or compilation takes too long
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

Some compilers (e.g. Visual C++, especially 2015) may have difficulties building advanced applications
that uses SVG++ library. 
Heavy usage of templates in SVG++ itself and also in its dependencies Boost.Spirit and Boost.MPL 
results in large number of templates instantiated during compilation.

SVG++ 1.1 introduces some workaround about this problem - some Boost.Spirit parsers now can be
moved to another translation unit at the cost of additional call to the virtual function on each value that
the parser produces.

Before first SVG++ header is included some macro should be defined for each parser to be moved::

  #define SVGPP_USE_EXTERNAL_PATH_DATA_PARSER 1
  #define SVGPP_USE_EXTERNAL_TRANSFORM_PARSER 1

And new source file should be added to the project that contains instantiations for some templates with 
parameters used in the application::

  #include <svgpp/parser/external_function/parse_path_data_impl.hpp>
  #include <svgpp/parser/external_function/parse_transform_impl.hpp>

  SVGPP_PARSE_PATH_DATA_IMPL(const char *, double)
  SVGPP_PARSE_TRANSFORM_IMPL(const char *, double)

First parameters to ``SVGPP_PARSE_PATH_DATA_IMPL()`` and ``SVGPP_PARSE_TRANSFORM_IMPL()`` macros are type of iterators
that are provided by used XML parser policy. And the second parameters are *coordinate* and *number* types used.