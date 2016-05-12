// Copyright Oleg Maximenko 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <svgpp/definitions.hpp>
#include <svgpp/parser/grammar/preserveAspectRatio.hpp>

namespace svgpp { namespace detail 
{

template<class Iterator>
bool parse_preserveAspectRatio(Iterator & it, Iterator end, preserveAspectRatio_value & value);

}}