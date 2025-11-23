//
// Created by andreas on 11/22/25.
//
#pragma once

#include <cstddef>
#include <cstdint>
#include "tok_type.h"
#include "bryToken.h"
#include <optional>
#include <string_view>
#include <vector>

/// @brief Responsible for converting raw source text into a stream of tokens.
class Lexer
{
public:
    /// @brief Creates a lexer from a null-terminated character buffer.
    /// @param source Pointer to the source code string (must outlive the lexer).
    Lexer(std::string_view source) noexcept;

    /// @brief Scans and returns the next token in the source stream.
    /// @return The next @c Token parsed from the input.
    [[nodiscard]] Token next();

    /// @brief Checks whether the end of the input has been reached.
    /// @return @c true if all characters have been consumed.
    [[nodiscard]] bool is_at_end() const noexcept;

#ifndef NDEBUG
    std::vector<Token> scan_tokens();
#endif

private:
    /// @brief Consumes and returns the current character, then advances.
    /// @return The consumed character.
    char advance();

    /// @brief Creates a token of the given type using the current lexeme range.
    /// @param type The token type to produce.
    /// @return The constructed @c Token.
    Token add_token(Token_type type);

    /// @brief Skips whitespace and comments if present.
    /// @return @c std::nullopt if nothing meaningful was skipped;
    ///         otherwise a complete @c Token if skipping results in a token (Error).
    std::optional<Token> skip_whitespace();

    /// @brief If the next character matches @p expected, consumes it.
    /// @param expected Character to match.
    /// @return @c true if matched and consumed.
    [[nodiscard]] bool match(char expected) noexcept;

    /// @brief Returns the current character without consuming it.
    /// @return The current character.
    [[nodiscard]] char peek() const noexcept;

    /// @brief Checks if the given character is a decimal digit.
    /// @param c Character to check.
    /// @return @c true if [0�9].
    [[nodiscard]] bool is_digit(char c) const noexcept;

    /// @brief Parses a string literal starting with the given quote character.
    /// @param quote The opening quote character.
    /// @return A string literal @c Token.
    Token string(char quote);

    /// @brief Returns the next character after @c peek() without consuming.
    /// @return The next character, or '\0' if at end.
    [[nodiscard]] char peek_next() const noexcept;

    /// @brief Parses a numeric literal (integer or floating-point).
    /// @return The parsed number token.
    Token number();

    /// @brief Checks if the character is a valid identifier start.
    /// @param c Character to check.
    /// @return @c true if alphabetical or underscore.
    [[nodiscard]] bool is_alpha(char c) const noexcept;

    /// @brief Parses a floating-point number starting with a dot.
    /// @return The parsed number token.
    Token dot_number();

    /// @brief Checks if the character is alphanumeric or underscore.
    /// @param c Character to check.
    /// @return @c true if [A�Z], [a�z], [0�9], or '_'.
    [[nodiscard]] bool is_alpha_numeric(char c) const noexcept;

    /// @brief Parses an identifier or keyword token.
    /// @return The resulting token.
    Token identifier();

private:
    std::string_view source_;   ///< View of the full source code being lexed.
    std::int32_t start_;        ///< Start index of the current lexeme.
    std::int32_t current_;      ///< Current index in the input.
    std::int32_t line_;         ///< Current line number for error reporting.
};
