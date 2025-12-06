//
// Created by andreas on 11/26/25.
//

#ifndef BERRIE_BRYOPCODE_H
#define BERRIE_BRYOPCODE_H

#include <cstdint>

enum class Opcode : std::uint8_t {
    ADD,
    SUB,
    DIV,
    MOD,
    MUL,

    EQUAL,
    GREATER,
    LESS,

    NOT,
    NEG,
    POS,

    LOAD_CONST,
    INIT_GLOBAL,
    SET_GLOBAL,
    GET_GLOBAL,

    RETURN,
    ECHO,

};

#endif //BERRIE_BRYOPCODE_H