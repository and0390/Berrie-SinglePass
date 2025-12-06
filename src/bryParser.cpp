//
// Created by andreas on 11/22/25.
//

#include "bryParser.h"
#include <charconv>
#include "bryLexer.h"
#include "bryToken.h"
#include "tok_type.h"
#include "bryChunk.h"
#include "bryOpcode.h"
#include "bryObject.h"
#include "fmt/color.h"
#include "bryEngine.h"
#include "utils.h"
#include "bryConstants.h"
#include <array>

// using parse_rule = BryParser::Parse_rule;
// using pcd = BryParser::Precedence;

std::pair<std::uint8_t, std::uint8_t> to_bytes(std::uint16_t i) noexcept {
    std::uint8_t low =  i & 0xFF;
    std::uint8_t high = (i >> 8)  & 0xFF;
    return {low, high};
}

bool sv_equal(std::string_view lhs, std::string_view rhs) noexcept {
    if (lhs.size() != rhs.size()) return false;
    return lhs == rhs;
}

constexpr auto make_pratt_table() {
    std::array<BryParser::Parse_rule, SENTINEL> table{};

    table.at(LEFT_PAREN)     = {&BryParser::parse_and_compile_grouping, nullptr, BryParser::CALL};
    table.at(RIGHT_PAREN)    = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(LEFT_BRACE)     = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(RIGHT_BRACE)    = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(LEFT_SQUARE)    = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(RIGHT_SQUARE)   = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(COMMA)          = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(DOT)            = {nullptr, nullptr, BryParser::CALL};
    table.at(MINUS)          = {&BryParser::parse_and_compile_unary, &BryParser::parse_and_compile_binary, BryParser::TERM};
    table.at(PLUS)           = {&BryParser::parse_and_compile_unary, &BryParser::parse_and_compile_binary, BryParser::TERM};
    table.at(SEMICOLON)      = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(SLASH)          = {nullptr, &BryParser::parse_and_compile_binary, BryParser::FACTOR};
    table.at(STAR)           = {nullptr, &BryParser::parse_and_compile_binary, BryParser::FACTOR};
    table.at(EQUAL)          = {nullptr, nullptr, BryParser::ASSIGNMENT};
    table.at(COLON)          = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(QUESTION)       = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(BANG)           = {&BryParser::parse_and_compile_unary, nullptr, BryParser::UNARY};
    table.at(BANG_EQUAL)     = {nullptr, &BryParser::parse_and_compile_binary, BryParser::ASSIGNMENT};
    table.at(EQUAL_EQUAL)    = {nullptr, &BryParser::parse_and_compile_binary, BryParser::EQUALITY};
    table.at(GREATER)        = {nullptr, &BryParser::parse_and_compile_binary, BryParser::COMPARISON};
    table.at(GREATER_EQUAL)  = {nullptr, &BryParser::parse_and_compile_binary, BryParser::COMPARISON};
    table.at(LESS)           = {nullptr, &BryParser::parse_and_compile_binary, BryParser::COMPARISON};
    table.at(LESS_EQUAL)     = {nullptr, &BryParser::parse_and_compile_binary, BryParser::COMPARISON};
    table.at(IDENTIFIER)     = {&BryParser::parse_and_compile_identifier, nullptr, BryParser::PRIMARY};
    table.at(STRING_LITERAL) = {&BryParser::parse_and_compile_primary, nullptr, BryParser::PRIMARY};
    table.at(INT_LITERAL)    = {&BryParser::parse_and_compile_primary, nullptr, BryParser::PRIMARY};
    table.at(FLOAT_LITERAL)  = {&BryParser::parse_and_compile_primary, nullptr, BryParser::PRIMARY};
    table.at(AND)            = {nullptr, nullptr, BryParser::AND};
    table.at(OR)             = {nullptr, nullptr, BryParser::OR};
    table.at(CLASS)          = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(ELSE)           = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(FALSE)          = {nullptr, nullptr, BryParser::PRIMARY};
    table.at(TRUE)           = {nullptr, nullptr, BryParser::PRIMARY};
    table.at(DEF)            = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(FOR)            = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(IF)             = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(NUL)            = {&BryParser::parse_and_compile_primary, nullptr, BryParser::DEFAULT};
    table.at(RETURN)         = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(SUPER)          = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(THIS)           = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(VAR)            = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(CONST)          = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(WHILE)          = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(BIN_AND)        = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(BIN_OR)         = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(MINUS_EQUAL)    = {nullptr, nullptr, BryParser::ASSIGNMENT};
    table.at(PLUS_EQUAL)     = {nullptr, nullptr, BryParser::ASSIGNMENT};
    table.at(SLASH_EQUAL)    = {nullptr, nullptr, BryParser::ASSIGNMENT};
    table.at(STAR_EQUAL)     = {nullptr, nullptr, BryParser::ASSIGNMENT};
    table.at(PLUS_PLUS)      = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(MINUS_MINUS)    = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(BREAK)          = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(CONTINUE)       = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(PERCENT)        = {nullptr, &BryParser::parse_and_compile_binary, BryParser::FACTOR};
    table.at(PERCENT_EQUAL)  = {nullptr, nullptr, BryParser::ASSIGNMENT};
    table.at(PRINT)          = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(ENDFILE)        = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(LEXICALERROR)   = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(ARROW)          = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(LAMBDA)         = {nullptr, nullptr, BryParser::DEFAULT};
    table.at(ECHO)           = {nullptr, nullptr, BryParser::DEFAULT};

    return table;
}

constexpr auto rule_table = make_pratt_table();

// constexpr std::pair<Token_type, parse_rule> rule_table[] {
//    {LEFT_PAREN,  {&BryParser::parse_and_compile_grouping, nullptr, pcd::CALL}},
//     {RIGHT_PAREN, {nullptr, nullptr, pcd::DEFAULT}},
//     {LEFT_BRACE,  {nullptr, nullptr, pcd::DEFAULT}},
//     {RIGHT_BRACE, {nullptr, nullptr, pcd::DEFAULT}},
//     {LEFT_SQUARE, {nullptr, nullptr, pcd::DEFAULT}},
//     {RIGHT_SQUARE,{nullptr, nullptr, pcd::DEFAULT}},
//     {COMMA,       {nullptr, nullptr, pcd::DEFAULT}},
//     {DOT,         {nullptr, nullptr, pcd::CALL}},
//     {MINUS,       {&BryParser::parse_and_compile_unary, &BryParser::parse_and_compile_binary, pcd::TERM}},
//     {PLUS,        {&BryParser::parse_and_compile_unary, &BryParser::parse_and_compile_binary, pcd::TERM}},
//     {SEMICOLON,   {nullptr, nullptr, pcd::DEFAULT}},
//     {SLASH,       {nullptr, &BryParser::parse_and_compile_binary, pcd::FACTOR}},
//     {STAR,        {nullptr, &BryParser::parse_and_compile_binary, pcd::FACTOR}},
//     {EQUAL,       {nullptr, nullptr, pcd::ASSIGNMENT}},
//     {COLON,       {nullptr, nullptr, pcd::DEFAULT}},
//     {QUESTION,    {nullptr, nullptr, pcd::DEFAULT}},
//     {BANG,        {&BryParser::parse_and_compile_unary, nullptr, pcd::UNARY}},
//     {BANG_EQUAL,  {nullptr, &BryParser::parse_and_compile_binary, pcd::ASSIGNMENT}},
//     {EQUAL_EQUAL, {nullptr, &BryParser::parse_and_compile_binary, pcd::EQUALITY}},
//     {GREATER,     {nullptr, &BryParser::parse_and_compile_binary, pcd::COMPARISON}},
//     {GREATER_EQUAL,{nullptr, &BryParser::parse_and_compile_binary,pcd::COMPARISON}},
//     {LESS,        {nullptr, &BryParser::parse_and_compile_binary, pcd::COMPARISON}},
//     {LESS_EQUAL,  {nullptr, &BryParser::parse_and_compile_binary, pcd::COMPARISON}},
//     {IDENTIFIER,  {&BryParser::parse_and_compile_identifier, nullptr, pcd::PRIMARY}},
//     {STRING_LITERAL,{&BryParser::parse_and_compile_primary, nullptr, pcd::PRIMARY}},
//     {INT_LITERAL, {&BryParser::parse_and_compile_primary, nullptr, pcd::PRIMARY}},
//     {FLOAT_LITERAL,{&BryParser::parse_and_compile_primary, nullptr, pcd::PRIMARY}},
//     {AND,         {nullptr, nullptr, pcd::AND}},
//     {OR,          {nullptr, nullptr, pcd::OR}},
//     {CLASS,       {nullptr, nullptr, pcd::DEFAULT}},
//     {ELSE,        {nullptr, nullptr, pcd::DEFAULT}},
//     {FALSE,       {nullptr, nullptr, pcd::PRIMARY}},
//     {TRUE,        {nullptr, nullptr, pcd::PRIMARY}},
//     {DEF,         {nullptr, nullptr, pcd::DEFAULT}},
//     {FOR,         {nullptr, nullptr, pcd::DEFAULT}},
//     {IF,          {nullptr, nullptr, pcd::DEFAULT}},
//     {NUL,         {&BryParser::parse_and_compile_primary, nullptr, pcd::DEFAULT}},
//     {RETURN,      {nullptr, nullptr, pcd::DEFAULT}},
//     {SUPER,       {nullptr, nullptr, pcd::DEFAULT}},
//     {THIS,        {nullptr, nullptr, pcd::DEFAULT}},
//     {VAR,         {nullptr, nullptr, pcd::DEFAULT}},
//     {CONST,       {nullptr, nullptr, pcd::DEFAULT}},
//     {WHILE,       {nullptr, nullptr, pcd::DEFAULT}},
//     {BIN_AND,     {nullptr, nullptr, pcd::DEFAULT}},
//     {BIN_OR,      {nullptr, nullptr, pcd::DEFAULT}},
//     {MINUS_EQUAL, {nullptr, nullptr, pcd::ASSIGNMENT}},
//     {PLUS_EQUAL,  {nullptr, nullptr, pcd::ASSIGNMENT}},
//     {SLASH_EQUAL, {nullptr, nullptr, pcd::ASSIGNMENT}},
//     {STAR_EQUAL,  {nullptr, nullptr, pcd::ASSIGNMENT}},
//     {PLUS_PLUS,   {nullptr, nullptr, pcd::DEFAULT}},
//     {MINUS_MINUS, {nullptr, nullptr, pcd::DEFAULT}},
//     {BREAK,       {nullptr, nullptr, pcd::DEFAULT}},
//     {CONTINUE,    {nullptr, nullptr, pcd::DEFAULT}},
//     {PERCENT,         {nullptr, &BryParser::parse_and_compile_binary, pcd::FACTOR}},
//     {PERCENT_EQUAL,   {nullptr, nullptr, pcd::ASSIGNMENT}},
//     {PRINT,       {nullptr, nullptr, pcd::DEFAULT}},
//     {ENDFILE,     {nullptr, nullptr, pcd::DEFAULT}},
//     {LEXICALERROR,{nullptr, nullptr, pcd::DEFAULT}},
//     {ARROW,       {nullptr, nullptr, pcd::DEFAULT}},
//     {LAMBDA,      {nullptr, nullptr, pcd::DEFAULT}},
//     {ECHO,      {nullptr, nullptr, pcd::DEFAULT}},
// };

BryParser::BryParser(Lexer* lexer, Chunk* chunk, BryEngine& engine)
    : error_logs_(), tok_prev_(), tok_curr_(), engine_(engine), environment_(std::make_unique<Environment>()), gc_(engine_.gc_), lexer_(lexer), chunk_(chunk), had_error_(false)
{}

/**
 * @brief Parses an expression using Pratt parsing rules.
 * 
 * This function is the core of the Pratt parser. It first parses a "prefix"
 * expression (nud), then iteratively parses "infix" expressions (led) as
 * long as the next operator has precedence higher than or equal to the given one.
 * 
 * It also determines if the parsed expression is assignable (lvalue) based on
 * the current operator precedence.
 * 
 * @param precedence The minimum precedence for this parsing step. Operators with
 * higher precedence will be parsed first.
 * 
 * @note Will panic if the parser encounters a token with no registered prefix
 * (nud) or infix (led) function.
 */
void BryParser::parse_expression(std::uint8_t precedence) {
    if (had_error_) return;

    advance();

    methodPtr prefix = rule_table.at(tok_prev_.get_type()).prefix;

    // Making sure that rvalue cannot be assigned Ex. "hello world!" = 60;
    const bool is_assignable = (precedence <= BryParser::ASSIGNMENT);

    if (!prefix) {
        panic("Failed to parse NUD");
    }

    (this->*prefix)(is_assignable);

    // while (precedence <= rule_table[tok_curr_.get_type()].second.precedence) {
        
    //     advance();
    //     methodPtr infix = rule_table[tok_prev_.get_type()].second.infix;

    //     if (!infix) {
    //         panic("Failed to parse LED");
    //     }

    //     (this->*infix)(is_assignable);
    // }

    
    while (precedence <= rule_table.at(tok_curr_.get_type()).precedence) {
        /// if current token is not a LED, we break the loop
        if(!rule_table.at(tok_curr_.get_type()).infix) break;

        advance();

        methodPtr infix = rule_table.at(tok_prev_.get_type()).infix;

        if (!infix) {
            panic("Failed to parse LED");
        }

        (this->*infix)(is_assignable);
    }
}

void BryParser::advance() {
    tok_prev_ = tok_curr_;
    for (;;) {
        tok_curr_ = lexer_->next();
        if (tok_curr_.get_type() != LEXICALERROR) break;
        current_report(tok_curr_.get_message());
    }
}

void BryParser::parse_and_compile_binary(bool) {
    Token_type prev_type = tok_prev_.get_type();

    // Acquiring current parse rule information
    auto&& rule = rule_table.at(prev_type);

    /// Check and parse operator with higher precedence
    parse_expression(static_cast<Precedence>(rule.precedence + 1));

    switch (prev_type) {
        case PLUS: emit_byte(static_cast<std::uint8_t>(Opcode::ADD)); break;
        case MINUS: emit_byte(static_cast<std::uint8_t>(Opcode::SUB)); break;
        case SLASH: emit_byte(static_cast<std::uint8_t>(Opcode::DIV)); break;
        case STAR: emit_byte(static_cast<std::uint8_t>(Opcode::MUL)); break;
        case PERCENT: emit_byte(static_cast<std::uint8_t>(Opcode::MOD)); break;
        case EQUAL_EQUAL: emit_byte(static_cast<std::uint8_t>(Opcode::EQUAL)); break;
        case BANG_EQUAL: emit_bytes(static_cast<std::uint8_t>(Opcode::EQUAL), static_cast<std::uint8_t>(Opcode::NOT)); break;
        case LESS: emit_byte(static_cast<std::uint8_t>(Opcode::LESS)); break;
        case GREATER: emit_byte(static_cast<std::uint8_t>(Opcode::GREATER)); break;
        case LESS_EQUAL: emit_bytes(static_cast<std::uint8_t>(Opcode::GREATER), static_cast<std::uint8_t>(Opcode::NOT)); break;
        case GREATER_EQUAL: emit_bytes(static_cast<std::uint8_t>(Opcode::LESS), static_cast<std::uint8_t>(Opcode::NOT)); break;
        default: break;
    }
}

void BryParser::parse_and_compile_unary(bool) {
    Token_type prev_type = tok_prev_.get_type();

    parse_expression(UNARY);
    switch (prev_type) {
        case MINUS: emit_byte(static_cast<std::uint8_t>(Opcode::NEG)); break;
        case PLUS: emit_byte(static_cast<std::uint8_t>(Opcode::POS)); break;
        case BANG: emit_byte(static_cast<std::uint8_t>(Opcode::NOT)); break;
        default: break;
    }
}

void BryParser::parse_and_compile_primary(bool) {

    const Token& tok = tok_prev_;

    switch (tok.get_type())
    {
        case INT_LITERAL: {
            std::int64_t value = 0;
            std::string_view lexeme = tok.get_lexeme();
            auto [ptr, ec] = std::from_chars(lexeme.data(), lexeme.data() + lexeme.size(), value);
            emit_constant(gc_.make_object<BryInt>(value));
            break;
        }
        case FLOAT_LITERAL: {
            double value = 0;
            std::string_view lexeme = tok.get_lexeme();
            auto [ptr, ec] = std::from_chars(lexeme.data(), lexeme.data() + lexeme.size(), value);
            emit_constant(gc_.make_object<BryFloat>(value));
            break;
        }
        case STRING_LITERAL: {
            std::string_view lexeme = tok.get_lexeme();
            emit_constant(gc_.make_object<BryString>(lexeme));
            break;
        }
        case TRUE: {
            emit_constant(bry_true_);
            break;
        }
        case FALSE: {
            emit_constant(bry_false_);
            break;
        }
        case NUL: {
            emit_constant(bry_null_);
            break;
        }
        default: {
            panic("Unrecognized primary");
            break;
        }
    }
}

void BryParser::parse_and_compile_int(bool) {
    std::int64_t value = 0;
    std::string_view lexeme = tok_curr_.get_lexeme();
    auto [ptr, ec] = std::from_chars(lexeme.data(), lexeme.data() + lexeme.size(), value);

    emit_constant(engine_.gc_.make_object<BryInt>(value));
    // Do something...
}

void BryParser::parse_and_compile_float(bool) {
    double value = 0;
    std::string_view lexeme = tok_curr_.get_lexeme();
    auto [ptr, ec] = std::from_chars(lexeme.data(), lexeme.data() + lexeme.size(), value);

    emit_constant(engine_.gc_.make_object<BryInt>(value));
    // Do Something...
}

void BryParser::parse_and_compile_string(bool) {
    std::string_view lexeme = tok_curr_.get_lexeme();
    emit_constant(gc_.make_object<BryString>(lexeme));
}

void BryParser::parse_and_compile_expression() {
    parse_expression(ASSIGNMENT);
}

void BryParser::expression_stmt() {
    parse_and_compile_expression();
    consume_or(SEMICOLON);
}

bool BryParser::check(Token_type type) {
    return tok_curr_.get_type() == type;
}

bool BryParser::match(Token_type type) {
    if (!check(type)) return false;
    advance();
    return true;
}

void BryParser::compile() {
    advance();
    while (!match(Token_type::ENDFILE)) {
        decl();
    }
    emit_byte(static_cast<std::uint8_t>(Opcode::RETURN));
    if (had_error_) print_error();
}

void BryParser::parse_and_compile_grouping(bool) {
    parse_and_compile_expression();
    consume(Token_type::RIGHT_PAREN, "Grouping expression must end with ')'");
}

void BryParser::stmt() {
    if (match(Token_type::ECHO)) {
        echo_stmt();
    }
    else expression_stmt();
}

void BryParser::decl() {
    if (match(VAR)) var_decl();
    else stmt();
}

void BryParser::echo_stmt() {
    parse_and_compile_expression();
    emit_byte(static_cast<std::uint8_t>(Opcode::ECHO));
    consume_or(SEMICOLON);
}

void BryParser::consume(Token_type type, const char *msg) {
    if (!check(type)) {
        current_report(msg);
        return;
    }
    advance();
}

/// @brief Emits a constant reference into the bytecode stream.
///
/// Inserts the given constant into the constant table using `make_constant()`,
/// then splits the resulting index (`size_t`) into two 8-bit values:
/// - `low`  = lower 8 bits of the index
/// - `high` = upper 8 bits of the index
///
/// Since the bytecode format only supports 8-bit operands, the 16-bit constant
/// index must be encoded as two separate bytes. The function emits:
///   1. `Opcode::CONSTANT`
///   2. the low byte of the index
///   3. the high byte of the index
///
/// @param constant Pointer to the constant object to be inserted.
/// @see make_constant()
/// @see emit_bytes()
/// @see emit_byte()
void BryParser::emit_constant(BryObject *constant) {
    auto i = make_constant(constant);
    std::uint8_t low =  i & 0xFF;
    std::uint8_t high = (i >> 8)  & 0xFF;
    emit_bytes(static_cast<std::uint8_t>(Opcode::LOAD_CONST), low);
    emit_byte(high);
}

void BryParser::current_report(const char *msg) {
    report(tok_curr_, msg);
}

void BryParser::previous_report(const char *msg) {
    report(tok_prev_, msg);
}

void BryParser::report(Token &tok, const char *msg) {
    had_error_ = true;
    error_logs_.push_back(Parse_error(tok.get_line(), msg));
}

BryParser::operator bool() const noexcept {
    return !had_error_;
}

void BryParser::emit_byte_and_index(std::uint8_t b, std::uint16_t i) {
    auto bytes = to_bytes(i);
    emit_bytes(b, bytes.first);
    emit_byte(bytes.second);
}

void BryParser::consume_or(Token_type type) noexcept {
    if (tok_curr_.get_type() != type) return;
    advance();
}

void BryParser::parse_and_compile_identifier(bool is_assignable) {
    identifier_impl(is_assignable, tok_prev_.get_lexeme());
}

void BryParser::identifier_impl(bool is_assignable, std::string_view name) {

    auto idx = make_constant(gc_.make_object<BryString>(name));

    if (is_assignable && check(EQUAL)) {
        switch (tok_curr_.get_type()) {
            case EQUAL: {
                advance();
                parse_and_compile_expression();
                break;
            }
            default: panic("in BryParser::identifier_impl, failed to recognize the type of assignment");
        }

        emit_byte_and_index(static_cast<std::uint8_t>(Opcode::SET_GLOBAL), idx);
    }

    emit_byte_and_index(static_cast<std::uint8_t>(Opcode::GET_GLOBAL), idx);
}

void BryParser::var_decl() {
    auto glob_var = parse_identifier("missing variable name");
    if (match(EQUAL)) {
        parse_and_compile_expression();
        consume_or(SEMICOLON);
    }
    else {
        consume_or(SEMICOLON);
    }
    set_identifier(glob_var);
}

std::uint16_t BryParser::parse_identifier(const char *msg) {
    consume(IDENTIFIER, msg);
    return make_constant(gc_.make_object<BryString>(tok_prev_.get_lexeme()));
}

void BryParser::set_identifier(std::uint16_t i) {
    emit_byte_and_index(static_cast<std::uint8_t>(Opcode::INIT_GLOBAL), i);
}

void BryParser::declare_locals(const char *msg) {
    if (environment_->scope_depth == 0) return;
    if (environment_->local_count == BRY_BUFFER_SIZE) previous_report("too many local variables");

    for (int i = environment_->local_count - 1; i >= 0; --i) {
        auto&& loc_meta = environment_->locals_meta_list.at(i);
        if (loc_meta.depth != 1 && loc_meta.depth < environment_->scope_depth) break;
        if (sv_equal(tok_prev_.get_lexeme(), loc_meta.var_name)) previous_report("variable already exists in this scope");
    }

    auto& loc_meta = environment_->locals_meta_list.at(environment_->local_count++);
}

std::uint16_t BryParser::make_constant(BryObject *literal) {
    size_t i = chunk_->add_constant(literal);
    if (i > UINT16_MAX) {
        fmt::print("[ERROR] Chunk overflow.");
        std::exit(5);
    }
    return static_cast<std::uint16_t>(i);
}

Chunk * BryParser::current_chunk() const noexcept {
    return chunk_;
}

void BryParser::emit_byte(std::uint8_t byte) {
    current_chunk()->write(static_cast<std::uint8_t>(byte), tok_prev_.get_line());
}

void BryParser::emit_bytes(std::uint8_t byte1, std::uint8_t byte2) {
    emit_byte(byte1);
    emit_byte(byte2);
}

void BryParser::print_error() const {
    for (const auto& log : get_error_logs()) {
        fmt::println("Error at line {}: {};", log.line, log.message);
    }
}

const std::vector<BryParser::Parse_error> & BryParser::get_error_logs() const noexcept {
    return error_logs_;
}
