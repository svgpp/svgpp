// Copyright Oleg Maximenko 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/preprocessor/config/config.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/facilities/empty.hpp>

#ifndef SVGPP_CHAR_TYPES
#  ifdef BOOST_NO_CXX11_UNICODE_LITERALS
#    define SVGPP_CHAR_TYPES ((char, BOOST_PP_EMPTY()))((wchar_t, L))
#  else
#    define SVGPP_CHAR_TYPES ((char, BOOST_PP_EMPTY()))((wchar_t, L))((char16_t, u))((char32_t, U))
#  endif
#endif
