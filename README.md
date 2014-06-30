#SVG++ C++ SVG template library#
===

##Introduction##

SVG++ library targets relatively narrow area between XML parser and graphics output software. Формат SVG, предоставляет удобные средства создания изображений, но чтение SVG оборачивается сложной задачей.

SVG++ is NOT:
* An SVG viewer or library that produces raster image from SVG (though some demo app with such functionality included)
* Renders anything itself

SVG++:
* Is a header-only library
* Compile time configured - no virtual functions
* Minimal runtime overhead - you will pay for only what you get
* Has wide range of possible uses:
  * Fully functional, conforming SVG viewers
  * Simple in-app SVG rasterizers
  * Import modules of vector editing software
  * Implementing path-only input of SVG format with minimal efforts in any graphics or math software
* Requires only Boost library itself (demo, tests and sample have their own requirements)
* Compatible with C++03, but requires good implementation, due to heavy usage of templates
* Free for commercial and non-commercial use. Distributed under the [Boost Software License](http://www.boost.org/LICENSE_1_0.txt)

 
  

