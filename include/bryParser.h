//
// Created by andreas on 11/22/25.
//
#pragma once

#include "bryToken.h"
#include "bryObject.h"
#include "bryOpcode.h"


class Chunk;
class Lexer;

class Parser {
public:

    enum Precedence
    {
        PREC_NONE,
        PREC_ASSIGNMENT,
        PREC_TERNARY,
        PREC_OR,
        PREC_AND,
        PREC_EQUALITY,
        PREC_COMPARISON,
        PREC_TERM,
        PREC_FACTOR,
        PREC_UNARY,
        PREC_CALL,
        PREC_PRIMARY
    };

    using methodPtr = void (Parser::*)(bool);

    struct Parse_rule {
        methodPtr prefix;
        methodPtr infix;
        Precedence precedence;
    };

    Parser(Lexer* lexer, Chunk* chunk);
    void parse_expression(Precedence precedence);
    void advance();

    /// @brief function to parse dan write binary operator into chunks
    void parse_and_compile_binary(bool);
    void parse_and_compile_unary(bool);
    void parse_and_compile_int(bool);

    Chunk* current_chunk() const noexcept;
    void emit_byte(Opcode byte);
    void emit_bytes(Opcode byte1, Opcode byte2);


private:
    Token tok_prev_;
    Token tok_curr_;
    Lexer* lexer_;
    Chunk* chunk_;
};