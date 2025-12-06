//
// Created by andreas on 11/22/25.
//
#pragma once

#include "bryToken.h"
#include <vector>
#include <array>
#include <memory>

/// forward declaration
class Chunk;
class Lexer;
class BryObject;
class BryEngine;
class GC;

/// a struct to represent error during parsing


///
/// @brief Single-pass berry parser
///
class BryParser {
    static constexpr size_t BRY_BUFFER_SIZE = UINT16_MAX;

    struct Parse_error {
        enum Type {
            SYNTAX_ERROR,
              OVERFLOW
          };
        int line;
        const char* message;
    };

    using methodPtr = void (BryParser::*)(bool);

    enum Precedence
    {
        DEFAULT,
        ASSIGNMENT,
        TERNARY,
        OR,
        AND,
        EQUALITY,
        COMPARISON,
        TERM,
        FACTOR,
        UNARY,
        CALL,
        PRIMARY
    };

    struct Parse_rule {
        methodPtr prefix;
        methodPtr infix;
        Precedence precedence;
    };

    struct Locals_meta {
        std::string_view var_name  = {};
        int depth                  = 0;
        bool is_mutable            = true;
        bool is_captured           = false;
    };

    struct Environment {
        std::array<Locals_meta, BRY_BUFFER_SIZE> locals_meta_list = {};
        std::unique_ptr<Environment> enclosing = nullptr;
        int local_count = 0;
        int scope_depth = 0;
    };
    friend constexpr auto make_pratt_table();
public:
    BryParser(Lexer* lexer, Chunk* chunk, BryEngine& engine);
    void compile();
    explicit operator bool() const noexcept;
private:
    void parse_expression(std::uint8_t precedence);
    void advance();
    void report(Token& token, const char* message);
    /// @brief function to parse dan write binary operator into chunks
    void parse_and_compile_binary(bool);
    void parse_and_compile_unary(bool);
    void parse_and_compile_int(bool);
    void parse_and_compile_float(bool);
    void parse_and_compile_string(bool);
    void parse_and_compile_expression();
    void parse_and_compile_primary(bool);
    void expression_stmt();
    bool check(Token_type type);
    bool match(Token_type type);
    void stmt();
    void decl();
    void echo_stmt();
    void consume(Token_type type, const char* msg);
    void emit_constant(BryObject *constant);
    void current_report(const char* msg);
    void previous_report(const char* msg);
    void emit_byte_and_index(std::uint8_t b, std::uint16_t i);
    void consume_or(Token_type type) noexcept;
    void parse_and_compile_identifier(bool is_assignable);
    void identifier_impl(bool is_assignable, std::string_view name);
    void var_decl();
    std::uint16_t parse_identifier(const char *msg);
    void set_identifier(std::uint16_t i);
    void declare_locals(const char* msg);
    const std::vector<Parse_error> &get_error_logs() const noexcept;
    /// @brief write constant into the chunk and return its index.
    std::uint16_t make_constant(BryObject *literal);
    void parse_and_compile_grouping(bool);
    Chunk* current_chunk() const noexcept;
    void emit_byte(std::uint8_t byte);
    void emit_bytes(std::uint8_t byte1, std::uint8_t byte2);
    void print_error() const;

    std::vector<Parse_error> error_logs_;
    Token tok_prev_;
    Token tok_curr_;
    BryEngine& engine_;
    std::unique_ptr<Environment> environment_;
    GC& gc_;
    Lexer* lexer_;
    Chunk* chunk_;
    bool had_error_;
};



