//
// Created by andreas on 11/22/25.
//

#include "bryParser.h"
#include "bryLexer.h"
#include "tok_type.h"
#include "bryChunk.h"

using parse_rule = Parser::Parse_rule;
using pcd = Parser::Precedence;

constexpr std::pair<Token_type, parse_rule> rule_table[] {
   {LEFT_PAREN,  {nullptr, nullptr, pcd::PREC_CALL}},
    {RIGHT_PAREN, {nullptr, nullptr, pcd::PREC_NONE}},
    {LEFT_BRACE,  {nullptr, nullptr, pcd::PREC_NONE}},
    {RIGHT_BRACE, {nullptr, nullptr, pcd::PREC_NONE}},
    {LEFT_SQUARE, {nullptr, nullptr, pcd::PREC_NONE}},
    {RIGHT_SQUARE,{nullptr, nullptr, pcd::PREC_NONE}},
    {COMMA,       {nullptr, nullptr, pcd::PREC_NONE}},
    {DOT,         {nullptr, nullptr, pcd::PREC_CALL}},
    {MINUS,       {nullptr, nullptr, pcd::PREC_TERM}},
    {PLUS,        {nullptr, nullptr, pcd::PREC_TERM}},
    {SEMICOLON,   {nullptr, nullptr, pcd::PREC_NONE}},
    {SLASH,       {nullptr, nullptr, pcd::PREC_FACTOR}},
    {STAR,        {nullptr, nullptr, pcd::PREC_FACTOR}},
    {EQUAL,       {nullptr, nullptr, pcd::PREC_ASSIGNMENT}},
    {COLON,       {nullptr, nullptr, pcd::PREC_NONE}},
    {QUESTION,    {nullptr, nullptr, pcd::PREC_NONE}},
    {BANG,        {nullptr, nullptr, pcd::PREC_UNARY}},
    {BANG_EQUAL,  {nullptr, nullptr, pcd::PREC_ASSIGNMENT}},
    {EQUAL_EQUAL, {nullptr, nullptr, pcd::PREC_EQUALITY}},
    {GREATER,     {nullptr, nullptr, pcd::PREC_COMPARISON}},
    {GREATER_EQUAL,{nullptr, nullptr,pcd::PREC_COMPARISON}},
    {LESS,        {nullptr, nullptr, pcd::PREC_COMPARISON}},
    {LESS_EQUAL,  {nullptr, nullptr, pcd::PREC_COMPARISON}},
    {IDENTIFIER,  {nullptr, nullptr, pcd::PREC_PRIMARY}},
    {STRING_LITERAL,{nullptr, nullptr, pcd::PREC_PRIMARY}},
    {INT_LITERAL, {nullptr, nullptr, pcd::PREC_PRIMARY}},
    {FLOAT_LITERAL,{nullptr, nullptr, pcd::PREC_PRIMARY}},
    {AND,         {nullptr, nullptr, pcd::PREC_AND}},
    {OR,          {nullptr, nullptr, pcd::PREC_OR}},
    {CLASS,       {nullptr, nullptr, pcd::PREC_NONE}},
    {ELSE,        {nullptr, nullptr, pcd::PREC_NONE}},
    {FALSE,       {nullptr, nullptr, pcd::PREC_PRIMARY}},
    {TRUE,        {nullptr, nullptr, pcd::PREC_PRIMARY}},
    {DEF,         {nullptr, nullptr, pcd::PREC_NONE}},
    {FOR,         {nullptr, nullptr, pcd::PREC_NONE}},
    {IF,          {nullptr, nullptr, pcd::PREC_NONE}},
    {NUL,         {nullptr, nullptr, pcd::PREC_NONE}},
    {RETURN,      {nullptr, nullptr, pcd::PREC_NONE}},
    {SUPER,       {nullptr, nullptr, pcd::PREC_NONE}},
    {THIS,        {nullptr, nullptr, pcd::PREC_NONE}},
    {VAR,         {nullptr, nullptr, pcd::PREC_NONE}},
    {CONST,       {nullptr, nullptr, pcd::PREC_NONE}},
    {WHILE,       {nullptr, nullptr, pcd::PREC_NONE}},
    {BIN_AND,     {nullptr, nullptr, pcd::PREC_NONE}},
    {BIN_OR,      {nullptr, nullptr, pcd::PREC_NONE}},
    {MINUS_EQUAL, {nullptr, nullptr, pcd::PREC_ASSIGNMENT}},
    {PLUS_EQUAL,  {nullptr, nullptr, pcd::PREC_ASSIGNMENT}},
    {SLASH_EQUAL, {nullptr, nullptr, pcd::PREC_ASSIGNMENT}},
    {STAR_EQUAL,  {nullptr, nullptr, pcd::PREC_ASSIGNMENT}},
    {PLUS_PLUS,   {nullptr, nullptr, pcd::PREC_NONE}},
    {MINUS_MINUS, {nullptr, nullptr, pcd::PREC_NONE}},
    {BREAK,       {nullptr, nullptr, pcd::PREC_NONE}},
    {CONTINUE,    {nullptr, nullptr, pcd::PREC_NONE}},
    {MOD,         {nullptr, nullptr, pcd::PREC_FACTOR}},
    {MOD_EQUAL,   {nullptr, nullptr, pcd::PREC_ASSIGNMENT}},
    {PRINT,       {nullptr, nullptr, pcd::PREC_NONE}},
    {ENDFILE,     {nullptr, nullptr, pcd::PREC_NONE}},
    {LEXICALERROR,{nullptr, nullptr, pcd::PREC_NONE}},
    {ARROW,       {nullptr, nullptr, pcd::PREC_NONE}},
    {LAMBDA,      {nullptr, nullptr, pcd::PREC_NONE}},
};

Parser::Parser(Lexer* lexer, Chunk* chunk)
    : tok_prev_(), tok_curr_(), lexer_(lexer), chunk_(chunk)
{}

void Parser::parse_expression(Precedence precedence) {
    advance();

    methodPtr prefix = rule_table[tok_prev_.get_type()].second.prefix;

    const bool is_assignable = (precedence <= pcd::PREC_ASSIGNMENT);

    (this->*prefix)(is_assignable);

    while (precedence <= rule_table[tok_prev_.get_type()].second.precedence) {
        advance();
        methodPtr infix = rule_table[tok_prev_.get_type()].second.infix;
        (this->*infix)(is_assignable);
    }
}

void Parser::advance() {
    tok_prev_ = tok_curr_;

    for (;;) {
        tok_curr_ = lexer_->next();
        if (tok_curr_.get_type() != LEXICALERROR) break;
    }
}

void Parser::binary(bool) {
    Token_type prev_type = tok_prev_.get_type();
    parse_rule rule = rule_table[prev_type].second;

    parse_expression(static_cast<Precedence>(rule.precedence + 1));

    switch (prev_type) {
        case PLUS:
    }
}

Chunk * Parser::current_chunk() const noexcept {
    return chunk_;
}

void Parser::emit_byte(std::uint8_t byte) {
    current_chunk()->write()
}
