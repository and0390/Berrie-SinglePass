//
// Created by andreas on 11/23/25.
//

#include "bryToken.h"

Token::Token(std::string_view lexeme, Token_type type, std::int32_t line, const char* message)
    : lexeme_{ lexeme }, type_{ type }, line_{ line }, message_{ message }
{}

Token::Token() = default;

void Token::set_message(const char *msg) noexcept {
    message_ = msg;
}

const char * Token::get_message() const noexcept {
    return message_;
}

std::string_view Token::get_lexeme() const noexcept {
    return lexeme_;
}

Token_type Token::get_type() const noexcept {
    return type_;
}

std::int32_t Token::get_line() const noexcept {
    return line_;
}
