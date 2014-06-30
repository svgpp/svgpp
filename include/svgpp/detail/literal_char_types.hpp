#pragma once

#include <boost/preprocessor/config/config.hpp>
#include <boost/preprocessor/wstringize.hpp>

#ifndef SVGPP_CHAR_TYPES
#  ifdef BOOST_NO_CXX11_UNICODE_LITERALS
#    define SVGPP_CHAR_TYPES ((char, BOOST_PP_STRINGIZE))((wchar_t, BOOST_PP_WSTRINGIZE))
#  else
#    define SVGPP_CHAR_TYPES ((char, BOOST_PP_STRINGIZE))((wchar_t, BOOST_PP_WSTRINGIZE))((char16_t, SVGPP_uSTRINGIZE))((char32_t, SVGPP_USTRINGIZE))
#  endif
#endif

#if ~BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_MWCC()
#   define SVGPP_u8STRINGIZE(text) SVGPP_u8STRINGIZE_I(text)
#   define SVGPP_uSTRINGIZE(text) SVGPP_uSTRINGIZE_I(text)
#   define SVGPP_USTRINGIZE(text) SVGPP_USTRINGIZE_I(text)
#else
#   define SVGPP_u8STRINGIZE(text) SVGPP_u8STRINGIZE_OO((text))
#   define SVGPP_u8STRINGIZE_OO(par) SVGPP_u8STRINGIZE_I ## par
#   define SVGPP_uSTRINGIZE(text) SVGPP_uSTRINGIZE_OO((text))
#   define SVGPP_uSTRINGIZE_OO(par) SVGPP_uSTRINGIZE_I ## par
#   define SVGPP_USTRINGIZE(text) SVGPP_USTRINGIZE_OO((text))
#   define SVGPP_USTRINGIZE_OO(par) SVGPP_USTRINGIZE_I ## par
#endif
#
#define SVGPP_u8STRINGIZE_I(text) SVGPP_u8STRINGIZE_II(#text)
#define SVGPP_u8STRINGIZE_II(str) u8 ## str
#define SVGPP_uSTRINGIZE_I(text) SVGPP_uSTRINGIZE_II(#text)
#define SVGPP_uSTRINGIZE_II(str) u ## str
#define SVGPP_USTRINGIZE_I(text) SVGPP_USTRINGIZE_II(#text)
#define SVGPP_USTRINGIZE_II(str) U ## str
