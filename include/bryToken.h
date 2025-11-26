//
// Created by andreas on 11/22/25.
//
#pragma once

#include "tok_type.h"
#include <cstdint>
#include <string_view>

class Token
{
public:
    Token(std::string_view lexeme, Token_type type, std::int32_t line, const char* message);
    Token();
    std::string_view get_lexeme() const noexcept;
    Token_type get_type() const noexcept;
    std::int32_t get_line() const noexcept;
    const char* get_message() const noexcept;
private:
    std::string_view lexeme_;
    Token_type type_;
    std::int32_t line_;
    const char* message_;
};