SVG++ documentation
=================================

About
------------

SVG++ library can be thought of as a framework, containing parsers for various SVG syntaxes, adapters that simplify
handling of parsed data and a lot of other utilities and helpers for the most common tasks.
Take a look at :ref:`Tutorial <tutorial>` to get the idea about what it is. 
Library sources available at https://github.com/svgpp/svgpp.

**SVG++ features**

* Is a header-only library
* Can be used with any XML parser
* Compile time configured - no virtual functions
* Minimal runtime overhead - you pay only for what you get
* Has wide range of possible uses:

  * Fully functional, conforming SVG viewers
  * Simple in-app SVG rasterizers
  * Import modules of vector editing software
  * Implementing path-only input of SVG format with minimal efforts in any graphics or math software
* Requires only Boost library itself (demo, tests and sample have their own requirements)
* Compatible with C++03, but requires conforming implementation, due to heavy usage of templates
* Supports any character type (`char` and `wchar_t`). Other (Unicode from C++11) were not tested, but should be ok.

**SVG 1.1 features covered by SVG++**

* Path data - parsing, simplification
* Transform list - parsing, simplification
* Color, including ICC color - parsing
* Lengths - highly configurable handling of SVG lengths, full support of all SVG length units
* Basic shapes - optional conversion to path
* Style attribute - parsing, taking in account differences in parsing presentation attributes and style properties
* Automatic marker positions calculation
* Viewport and viewbox handling

**SVG++ is NOT**

* An SVG viewer or library that produces raster image from SVG 
  (though C++ demo app included, which renders SVG with `AGG <http://antigrain.com/>`_, GDI+ or `Skia <https://code.google.com/p/skia/>`_ graphics library)
* Renders anything itself

**Contents**

.. toctree::
   :maxdepth: 2

   installation
   lesson01
   overview
   reference
   advanced
   faq
   license
   help
