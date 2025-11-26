//
// Created by andreas on 11/22/25.
//

#include "debug.h"
#include <iostream>
#include <fmt/core.h>
#include <fmt/format.h>

namespace debug {
	const char* tokType_to_stringLit(Token_type type)
	{
		switch (type)
		{
			case LEFT_PAREN:       return "LEFT_PAREN";
			case RIGHT_PAREN:      return "RIGHT_PAREN";
			case LEFT_BRACE:       return "LEFT_BRACE";
			case RIGHT_BRACE:      return "RIGHT_BRACE";
			case LEFT_SQUARE:      return "LEFT_SQUARE";
			case RIGHT_SQUARE:     return "RIGHT_SQUARE";
			case COMMA:            return "COMMA";
			case DOT:              return "DOT";
			case MINUS:            return "MINUS";
			case PLUS:             return "PLUS";
			case SEMICOLON:        return "SEMICOLON";
			case SLASH:            return "SLASH";
			case STAR:             return "STAR";
			case EQUAL:            return "EQUAL";
			case COLON:            return "COLON";
			case QUESTION:         return "QUESTION";
			case BANG:             return "BANG";
			case BANG_EQUAL:       return "BANG_EQUAL";
			case EQUAL_EQUAL:      return "EQUAL_EQUAL";
			case GREATER:          return "GREATER";
			case GREATER_EQUAL:    return "GREATER_EQUAL";
			case LESS:             return "LESS";
			case LESS_EQUAL:       return "LESS_EQUAL";

			case IDENTIFIER:       return "IDENTIFIER";
			case STRING_LITERAL:   return "STRING_LITERAL";
			case INT_LITERAL:      return "INT_LITERAL";
			case FLOAT_LITERAL:    return "FLOAT_LITERAL";

			case AND:              return "AND";
			case OR:               return "OR";
			case CLASS:            return "CLASS";
			case ELSE:             return "ELSE";
			case FALSE:            return "FALSE";
			case TRUE:             return "TRUE";
			case DEF:              return "DEF";
			case FOR:              return "FOR";
			case IF:               return "IF";
			case NUL:              return "NUL";
			case RETURN:           return "RETURN";
			case SUPER:            return "SUPER";
			case THIS:             return "THIS";
			case VAR:              return "VAR";
			case CONST:              return "VAL";
			case WHILE:            return "WHILE";

			case BIN_AND:          return "BIN_AND";
			case BIN_OR:           return "BIN_OR";

			case MINUS_EQUAL:      return "MINUS_EQUAL";
			case PLUS_EQUAL:       return "PLUS_EQUAL";
			case SLASH_EQUAL:      return "SLASH_EQUAL";
			case STAR_EQUAL:       return "STAR_EQUAL";

			case PLUS_PLUS:        return "PLUS_PLUS";
			case MINUS_MINUS:      return "MINUS_MINUS";

			case BREAK:            return "BREAK";
			case CONTINUE:         return "CONTINUE";

			case PERCENT:              return "MOD";
			case PERCENT_EQUAL:        return "MOD_EQUAL";

			case PRINT:            return "PRINT";

			case ENDFILE:          return "EOF";
			case LEXICALERROR:	   return "LEXICAL ERROR";

			case ARROW:            return "ARROW";
			case LAMBDA:           return "LAMBDA";

			default:               return "UNKNOWN_TOKEN";
		}
	}

	void print_token(Token token)
	{
		auto&& PLexeme = token.get_lexeme().size() == 0 ? "None" : token.get_lexeme();
		auto&& PMessage = std::strlen(token.get_message()) == 0 ? "None" : token.get_message();
		fmt::print("Lexeme: '{}', Line: {}, Token type: {}, Message: {};\n", PLexeme, token.get_line(), tokType_to_stringLit(token.get_type()), PMessage);
	}

} //namespace debug
