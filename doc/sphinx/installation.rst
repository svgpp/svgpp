Installation
====================

The latest version of SVG++ can be downloaded from SVG++ repository, at https://github.com/svgpp/svgpp, or
as a package at https://github.com/svgpp/svgpp/archive/master.zip

SVG++ requires Boost library (was tested with Boost >= 1.55). Only header-only libraries from Boost are
used, no build required.

SVG++ consists of header files only and does not require any libraries to link against. 
Including ``svgpp/svgpp.hpp`` will be sufficient for most projects.

Library was tested with this compilers:

* Visual Studio 2010, 2012, 2013
* GCC 4.8 (in C++11 mode it requires Boost >= 1.56)
* Clang 3.2

.. note::
  Компиляция модуля, использующего много возможностей SVG++, может потребовать большого объема оперативной памяти.
  Учитывайте это при запуске компиляции на виртуальных машинах.