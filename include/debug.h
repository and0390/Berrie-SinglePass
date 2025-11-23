//
// Created by andreas on 11/22/25.
//

#pragma once

#include "bryToken.h"
#include "tok_type.h"

namespace debug
{
	const char* tokType_to_stringLit(Token_type type);
	void print_token(Token token);
}