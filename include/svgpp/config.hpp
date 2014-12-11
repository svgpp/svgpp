// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

// TODO: should be set to "static" if compiler supports thread-safe initialization of local static variables
// or non-multithreaded configuration of library requested
#ifndef SVGPP_STATIC_IF_SAFE
#define SVGPP_STATIC_IF_SAFE static
#endif

#ifndef SVGPP_NO_EXCEPTIONS
#define SVGPP_INTERCEPT_EXCEPTIONS 1
#endif