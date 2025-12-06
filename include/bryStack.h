//
// Created by andreas on 11/29/25.
//

#ifndef BERRIE_BRYSTACK_H
#define BERRIE_BRYSTACK_H

#include <cstdint>
#include <array>
#include <assert.h>

#include "utils.h"
#include "fmt/color.h"

template<typename T, std::size_t N>
class BryStack {
public:

    BryStack() : buffer_(), top_(0) {}

    void push(T obj) {
        if (top_ >= size()) {
            fmt::print("[ERROR] Maximum recursion depth reached.");
            std::exit(10);
        }
        buffer_.at(top_++) = obj;
    }

    T pop() {
        if (top_ == 0) {
            assert(false && "Trying to popping from an empty stack");
        }
        return std::move(buffer_.at(--top_));
    }

    T& peek(std::size_t offset) {
        if (offset > N) {
            fmt::print("[ERROR] Maximum recursion depth reached.");
            std::exit(10);
        }
        return buffer_.at(capacity() - 1 - offset);
    }

    std::size_t capacity() const noexcept {
        return  top_;
    }

    size_t size() const noexcept {
        return N;
    }

    std::array<T, N> buffer_;
    size_t top_;
};

#endif //BERRIE_BRYSTACK_H