SVG++ library
===

SVG++ is a free header-only C++ library

##Introduction##

SVG++ library targets relatively narrow area between XML parser and graphics output software. Формат SVG, предоставляет удобные средства создания изображений, но чтение SVG оборачивается сложной задачей.

SVG++ is NOT:
* An SVG viewer or library that produces raster image from SVG (though some demo app with such functionality included)
* Renders anything itself

Features
--------

* Is a header-only library
* Any XML parser can be used
* Compile time configured - no virtual functions
* Minimal runtime overhead - you pay for only what you get
* Has wide range of possible uses:
  * Fully functional, conforming SVG viewers
  * Simple in-app SVG rasterizers
  * Import modules of vector editing software
  * Implementing path-only input of SVG format with minimal efforts in any graphics or math software
* Requires only Boost library itself (demo, tests and sample have their own requirements)
* Compatible with C++03, but requires conforming implementation, due to heavy usage of templates
* Supports any character type (`char` and `wchar_t`). Other (Unicode from C++11) not tested, but should be ok.

SVG features
-------
* Path data - parsing, simplification
* Transform list - parsing, simplification
* Color, including ICC color - parsing
* Lengths - highly configurable handling
* Basic shapes - optional conversion to path
* Style attribute - parsing, taking in account differences in parsing presentation attributes and style properties
* Automatical marker positions calculation
* Viewport and viewbox handling

Installation
------------

Get latest version from https://github.com/svgpp/svgpp/archive/master.zip

Prerequisite - Boost library (tested with 1.55).
Point your project include path to `svgpp/include` folder and Boost folder, thats all. Just header-only components from Boost are used, so no need to build Boost either.

Compilers supported:
* Visual Studio 2010, 2013
* GCC 4.8.2 in C++03 mode. In C++11 mode it requires Boost 1.56.

Visual Studio 2008 gives "internal compiler error", probably some workaround is possible.

License
-------

Free for commercial and non-commercial use. The project is licensed under the [Boost Software License, Version 1.0](http://www.boost.org/LICENSE_1_0.txt). 
  

