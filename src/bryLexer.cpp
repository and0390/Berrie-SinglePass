#include "tok_type.h"
#include "bryLexer.h"
#include <cstddef>
#include <cstring>
#include <cassert>


bool Lexer::is_alpha(char c) const noexcept
{ 
	return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_'; 
}

bool Lexer::is_digit(char c) const noexcept 
{ 
	return c >= '0' && c <= '9'; 
}

bool Lexer::is_at_end() const noexcept 
{ 
	return current_ >= source_.size(); 
}

std::vector<Token> Lexer::scan_tokens()
{
	std::vector<Token> tokens;
	while (!is_at_end()) {
		tokens.push_back(next());
	}
	return tokens;
}

Token Lexer::add_token(Token_type type)
{
	return Token{ {source_.data() + start_, static_cast<size_t>(current_ - start_)}, type, line_, ""};
}

char Lexer::advance()
{
	if (current_ == INT32_MAX) {

	}
	current_++;
	return source_[current_ - 1];
}

Lexer::Lexer(std::string_view source) noexcept
	: source_{ source }, start_{ 0 }, current_{ 0 }, line_{ 1 }
{}

Token Lexer::next()
{

	std::optional<Token> perhaps_error = skip_whitespace();

	if (perhaps_error.has_value()) return perhaps_error.value();

	start_ = current_;

	if (is_at_end()) return Token{ {"", 0u}, ENDFILE, line_ ,""};

	char c = advance();

	switch (c)
	{
	case '(': return add_token(LEFT_PAREN); break;
	case ')': return add_token(RIGHT_PAREN); break;
	case '{': return add_token(LEFT_BRACE); break;
	case '[': return add_token(LEFT_SQUARE); break;
	case ']': return add_token(RIGHT_SQUARE); break;
	case '}': return add_token(RIGHT_BRACE); break;
	case ',': return add_token(COMMA); break;
	case '.': {
		if (is_digit(peek())) return dot_number();
		return add_token(DOT);
	}
	case '-': {
		if (match('=')) return add_token(MINUS_EQUAL);
		return add_token(MINUS);
	}
	case '+': {
		if (match('=')) return add_token(PLUS_EQUAL);
		return add_token(PLUS);
	}
	case ';': return add_token(SEMICOLON); break;
	case '*': return add_token(match('=') ? STAR_EQUAL : STAR); break;
	case '!': return add_token(match('=') ? BANG_EQUAL : BANG); break;
	case '=': {
		if (match('=')) {
			return add_token(EQUAL_EQUAL);
		}
		else if (match('>')) {
			return add_token(ARROW);
		}
		else {
			return add_token(EQUAL);
		}
	}
	case '<': return add_token(match('=') ? LESS_EQUAL : LESS); break;
	case '>': return add_token(match('=') ? GREATER_EQUAL : GREATER); break;
	case ':': return add_token(COLON); break;
	case '?': return add_token(QUESTION); break;
	case '/': return add_token(match('=') ? SLASH_EQUAL : SLASH);
	case '%': return add_token(match('=') ? MOD_EQUAL : MOD);
	case '"': return string('"'); break;
	case '\'': return string('\''); break;
	case '|': return add_token(match('|') ? OR : BIN_OR);
	case '&': return add_token(match('&') ? AND : BIN_AND);
	default: {
			if (is_digit(c)) return number();
			else if (is_alpha(c)) return identifier();
			return Token{ {source_.data() + start_, static_cast<size_t>(current_ - start_)}, LEXICALERROR, line_, "Unrecognized character" };
		}
	}
}

std::optional<Token> Lexer::skip_whitespace()
{
	for (;;)
	{
		switch (peek())
		{
		case ' ':
		case '\r':
		case '\t':
			advance();
			break;
		case '\n':
			line_++;
			advance();
			break;
		case '/': {
			if (peek_next() == '/') {
				while (peek() != '\n' && !is_at_end()) advance();
			}
			else if (peek_next() == '*') {
				advance(); // consume '/'
				advance(); //consume '*'

				bool unterminated = true;
				while (!is_at_end()) {
					const char c = advance();
					if (c == '\n') line_++;
					if (c == '*' && peek() == '/') {
						advance(); // consume '/'
						unterminated = false;
						break;
					}
				}

				if (unterminated) {
					return Token{ {source_.data() + start_, static_cast<size_t>(current_ - start_)}, LEXICALERROR,  line_, "Unterminated comment"};
				}
			}
			else {
				return {};
			}
			break;
		}
		default:
			return {};
		}
	}
}

bool Lexer::match(char expected) noexcept
{
	if (is_at_end()) return false;
	if (source_[current_] != expected) return false;
	current_++;
	return true;
}

char Lexer::peek() const noexcept
{
	if (is_at_end()) return '\0';
	return source_[current_];
}

Token Lexer::string(char trigger)
{
	while (!is_at_end() && peek() != trigger) {
		if (peek() == '\n') {
			line_++;
			return Token{
				{ source_.data() + start_, static_cast<size_t>(current_ - start_) },
				LEXICALERROR,
				line_,
				"Unterminated string"
			};
		}
		advance();
	}

	if (is_at_end()) {
		return Token{
			{ source_.data() + start_, static_cast<size_t>(current_ - start_) },
			LEXICALERROR,
			line_,
			"Unterminated string"
		};
	}

	advance(); // consume closing quote

	return Token{
		{ source_.data() + start_ + 1, static_cast<size_t>((current_ - 1) - (start_ + 1)) },
		STRING_LITERAL,
		line_,
		""
	};
}

Token Lexer::number()
{
	while (is_digit(peek())) advance();

	// Optional fractional part
	if (peek() == '.' && !is_alpha(peek_next())) {
		advance(); // consume '.'

		// optional digits after '.'
		while (is_digit(peek())) advance();

		return add_token(FLOAT_LITERAL);
	}

	return add_token(INT_LITERAL);
}

char Lexer::peek_next() const noexcept
{
	if (current_ + 1 >= source_.size()) return '\0';
	return source_[current_ + 1];
}

bool Lexer::is_alpha_numeric(char c) const noexcept
{
	return is_alpha(c) || is_digit(c);
}

Token Lexer::dot_number()
{
	while (is_digit(peek())) advance();
	return add_token(FLOAT_LITERAL);
}

constexpr std::pair<std::string_view, Token_type> keywords[] = {
	{"and",     AND},
	{"class",   CLASS},
	{"continue",CONTINUE},
	{"else",    ELSE},
	{"def",     DEF},
	{"for",     FOR},
	{"false",   FALSE},
	{"if",      IF},
	{"null",    NUL},
	{"return",  RETURN},
	{"super",   SUPER},
	{"or",      OR},
	{"this",    THIS},
	{"true",    TRUE},
	{"lambda",  LAMBDA},
	{"while",   WHILE},
	{"break",   BREAK},
	{"var",     VAR},
	{"const",     CONST},
};

Token Lexer::identifier()
{
	while (is_alpha_numeric(peek())) advance();

	const std::size_t size = current_ - start_;

	std::string_view lexeme{ source_.data() + start_, size};

	//check if keyword first
	for (const auto& [name, type] : keywords) {
		if (lexeme == name) {
			return add_token(type);
		}
	}

	return add_token(IDENTIFIER);
}
