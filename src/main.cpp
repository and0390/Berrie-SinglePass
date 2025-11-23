// Berrie.cpp : Defines the entry point for the application.
//

#include "bryLexer.h"
#include "debug.h"
#include <string>
#include <iostream>
#include <fstream>
#include <fmt/color.h>
#include <filesystem>
#include "bryChunk.h"
#include "bryParser.h"

int main()
{
    std::string path_to_source = std::string(PROJECT_ROOT) + "/bry/source1.bry";
    std::ifstream file(path_to_source);

    if (!file.is_open()) {
        fmt::print(stderr, fg(fmt::color::red), "Failed to open the source file.\n");
        std::cerr << "Current directory: " << path_to_source;
        return -1;
    }



    std::string source{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };

    Lexer lexer{ source.c_str() };

    Chunk chunk;



    auto&& tokens = lexer.scan_tokens();

    for (const auto& token : tokens) {
        debug::print_token(token);
    }

    file.close();
    return 0;
}