//
// Created by andreas on 11/22/25.
//

#include "bryChunk.h"


void Chunk::write(std::uint8_t byte, std::int32_t line) {
    code_.push_back(byte);
    lines_.push_back(line);
}

const Chunk::ConstantList & Chunk::get_constantList() const noexcept {
    return constants_;
}

const Chunk::LineList & Chunk::get_lineList() const noexcept {
    return  lines_;
}

const Chunk::CodeList & Chunk::get_codeList() const noexcept {
    return code_;
}
