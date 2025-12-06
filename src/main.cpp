///
/// Berrie.cpp : Defines the entry point for the application.
///

#include "bryLexer.h"
#include "debug.h"
#include <string>
#include <iostream>
#include <fstream>
#include "bryEngine.h"
#include <fmt/color.h>
#include "bryChunk.h"
#include <optional>
#include "bryParser.h"


std::optional<std::string> get_source() {
    std::string path_to_source = std::string(PROJECT_ROOT) + "/bry/source1.bry";
    std::ifstream file(path_to_source);

    if (!file.is_open()) {
        fmt::print(stderr, fg(fmt::color::red), "Failed to open the source file.\n");
        std::cerr << "Current directory: " << path_to_source;
        return {};
    }

    return std::string{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
}

int init() {
    auto&& opt_str = get_source();

    if (!opt_str.has_value()) {
        return -1;
    }

    auto& source = opt_str.value();

    Lexer lexer{ source.c_str() };

    // auto&& tokens = lexer.scan_tokens();


    // for (const auto& t : tokens) {
    //     debug::print_token(t);
    // }
    // return 0;

    Chunk chunk;
    BryEngine engine(&chunk);
    BryParser parser(&lexer, &chunk, engine);
    parser.compile();

    if (!parser) return -1;
    // debug::Diassembler diassembler(&chunk);
    // diassembler.print();

    int ret_code = static_cast<int>(engine.run());

    return 0;
}


int main()
{
    // std::string path_to_source = std::string(PROJECT_ROOT) + "/bry/source1.bry";
    // std::ifstream file(path_to_source);
    //
    // if (!file.is_open()) {
    //     fmt::print(stderr, fg(fmt::color::red), "Failed to open the source file.\n");
    //     std::cerr << "Current directory: " << path_to_source;
    //     return -1;
    // }
    //
    // std::string source{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
    //
    // Lexer lexer{ source.c_str() };
    //
    // Chunk chunk;
    //
    // auto&& tokens = lexer.scan_tokens();
    //
    // for (const auto& token : tokens) {
    //     debug::print_token(token);
    // }
    //
    // file.close();
    return init();
}