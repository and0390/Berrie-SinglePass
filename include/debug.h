//
// Created by andreas on 11/22/25.
//

#pragma once

#include "bryOpcode.h"
#include "bryToken.h"
#include "tok_type.h"

class Chunk;

namespace debug
{
	const char* tokType_to_stringLit(Token_type type);
	void print_token(const Token &token);

	class Diassembler {
	public:
		Diassembler(Chunk* chunk);
		void print();
	private:
		std::uint8_t read_byte();
		std::uint16_t read_index();
		Chunk* chunk_;
		size_t ip_;
	};
}
