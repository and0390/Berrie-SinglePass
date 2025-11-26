//
// Created by andreas on 11/26/25.
//

#ifndef BERRIE_BRYOPCODE_H
#define BERRIE_BRYOPCODE_H

enum class Opcode : std::uint8_t {
    ADD, SUB, DIV ,MOD ,MUL,

    EQUAL, GREATER, LESS,

    NOT, NEG, POS
};

#endif //BERRIE_BRYOPCODE_H