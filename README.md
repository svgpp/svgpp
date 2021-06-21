SVG++ library 1.3.0
=======================

Please visit http://svgpp.org/ for description, tutorial and reference.

#### About

SVG++ library can be thought of as a framework, containing parsers for various SVG syntaxes, adapters that simplify
handling of parsed data and a lot of other utilities and helpers for the most common tasks.
Take a look at [Tutorial](http://svgpp.org/lesson01.html) to get the idea about what it is.

#### SVG++ features

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
* Compatible with C++11, but requires conforming implementation, due to heavy usage of templates
* Supports any character type (`char` and `wchar_t`). Other (Unicode from C++11) were not tested, but should be ok.

#### SVG 1.1 features covered by SVG++

* Path data - parsing, simplification
* Transform list - parsing, simplification
* Color, including ICC color - parsing
* Lengths - highly configurable handling of SVG lengths, full support of all SVG length units
* Basic shapes - optional conversion to path
* Style attribute - parsing, taking in account differences in parsing presentation attributes and style properties
* Automatic marker positions calculation
* Viewport and viewbox handling

#### SVG++ is NOT

* An SVG viewer or library that produces raster image from SVG
  (though C++ demo app included, which renders SVG with [AGG](http://antigrain.com), GDI+ or [Skia](https://code.google.com/p/skia/) graphics library)
* Renders anything itself

#### What's new

* SVG++ update 1.2.1 fixes handling text content inside `<a>` element that is a child of `<text>` element.
* SVG++ update 1.2 focuses on reducing compiler memory usage by allowing separation
  of template heavy Boost.Spirit code to other compilation unit.

#### How to Help

* *CSS Processor.* Processing CSS is outside of SVG++ scope, but it'll be nice to have some generic implementation of CSS
selectors to use at preprocessing stage.
* *SVG Filter Effects.* Some filters are already implemented in demo application but the complete solution requires more efforts.
Generic enough parts may fit in SVG++ source base. Usage of Boost.GIL may make the code more reusable.
* *Animation.* Implementation of animation in the demo application. Probably some utilities may get to SVG++.
* Try to reduce compiler memory usage by switching to some C++11 metaprogramming library (brigand?).
Evaluate upgrade to Boost.Spirit 3.0.
* Extend demo with Cairo support?
* More wrappers for XML parsers?
* Evolve demo to fully functional SVG rendering component for one of the graphic engines.
A major gap in demo's SVG implementation is text and font support. It'll be too complex to make it cross-engine, but
for the single engine chosen and probably with third-party libraries like FreeType it's pretty feasible.
