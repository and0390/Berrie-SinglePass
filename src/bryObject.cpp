//
// Created by andreas on 11/22/25.
//

#include "bryObject.h"


BObject::BObject(bool is_marked) noexcept
    : is_marked_(is_marked)
{}

BObject::BObject() noexcept
    : BObject(false)
{}

void BObject::mark() noexcept {
    is_marked_ = true;
}

void BObject::unmark() noexcept {
    is_marked_ = false;
}

BInt::BInt(std::int64_t value) noexcept
    : BObject(), value_(value)
{}

BFloat::BFloat(double value) noexcept
    : BObject(), value_(value)
{}

BString::BString(std::string_view value)
    : BObject(), value_(value)
{}

BFunction::BFunction(Chunk chunk, size_t upvalue_count, size_t arity, std::vector<std::int32_t> param_offset,
    BString *name)
        : chunk_(std::move(chunk)), upvalue_count_(upvalue_count), arity_(arity), param_offset_(std::move((param_offset)))
{}


