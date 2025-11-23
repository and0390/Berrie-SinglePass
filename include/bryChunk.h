//
// Created by andreas on 11/22/25.
//

#pragma once
#include <cstdint>
#include <vector>
#include <variant>

class BObject;

using BValue = std::variant<BObject*>;

class Chunk {
public:
    using ConstantList = std::vector<BValue>;
    using LineList = std::vector<std::int32_t>;
    using CodeList = std::vector<std::uint8_t>;

    Chunk() = default;
    template<typename ConstantType>
    std::size_t add_constant(ConstantType&& type) {
        constants_.push_back((std::forward<ConstantType>(type)));
        return constants_.size() - 1;
    }

    void write(std::uint8_t byte, std::int32_t line);

    const ConstantList& get_constantList() const noexcept;
    const LineList& get_lineList() const noexcept;
    const CodeList& get_codeList() const noexcept;
private:
    ConstantList constants_;
    LineList lines_;
    CodeList code_;
};
