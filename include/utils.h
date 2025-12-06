//
// Created by andreas on 12/1/25.
//

#ifndef BERRIE_UTILS_H
#define BERRIE_UTILS_H

#include <cstdint>
#include <string_view>

#include "bryObject.h"

/// forward declaration
class BryString;

void panic(const char* msg);

struct hashBryStringptr {
    std::size_t operator()(const BryString* obj) const noexcept;
};

struct eqBryStringptr {
    bool operator()(const BryString* lhs, const BryString* rhs) const noexcept;
};

struct hashBryStringptrTransparent {
    using is_transparent = void;

    size_t operator()(std::string_view sv) const noexcept;
    size_t operator()(const BryString* obj) const noexcept;
};

struct eqBryStringptrTransparent {
    using is_transparent = void;

    bool operator()(const BryString* lhs, const BryString* rhs) const noexcept;
    bool operator()(std::string_view lhs, const BryString* rhs) const noexcept;
    bool operator()(const BryString* lhs, std::string_view rhs) const noexcept;
    bool operator()(std::string_view lhs, std::string_view rhs) const noexcept;
};

#endif //BERRIE_UTILS_H