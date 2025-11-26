//
// Created by andreas on 11/22/25.
//


#pragma once

enum Token_type
{
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_SQUARE,
    RIGHT_SQUARE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,
    EQUAL,
    COLON,
    QUESTION,

    BANG,
    BANG_EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    IDENTIFIER,
    STRING_LITERAL,
    INT_LITERAL,
    FLOAT_LITERAL,

    AND,
    OR,
    CLASS,
    ELSE,
    FALSE,
    TRUE,
    DEF,
    FOR,
    IF,
    NUL,
    RETURN,
    SUPER,
    THIS,
    VAR,
    CONST,
    WHILE,
    BIN_AND,
    BIN_OR,
    MINUS_EQUAL,
    PLUS_EQUAL,
    SLASH_EQUAL,
    STAR_EQUAL,
    PLUS_PLUS,
    MINUS_MINUS,
    BREAK,
    CONTINUE,
    PERCENT,
    PERCENT_EQUAL,
    PRINT,
    ENDFILE,
    LEXICALERROR,
    ARROW,
    LAMBDA
};