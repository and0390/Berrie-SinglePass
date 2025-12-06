//
// Created by andreas on 12/6/25.
//

#include "utils.h"
#include "fmt/color.h"
#include "bryObject.h"

void panic(const char* msg) {
    fmt::print(stderr, fg(fmt::color::red), "InternalFailure: {}\n", msg);
    std::terminate(); // immediately aborts the program
}

std::size_t hashBryStringptr::operator()(const BryString* obj) const noexcept {
    /// Checking the values, no interning yet
    return std::hash<std::string_view>()(static_cast<std::string_view>(obj->value_));
}

bool eqBryStringptr::operator()(const BryString* lhs, const BryString* rhs) const noexcept {
    if (lhs == rhs) return true;
    if (!lhs || !rhs) return false;
    return lhs->value_ == rhs->value_;
}

size_t hashBryStringptrTransparent::operator()(std::string_view sv) const noexcept {
    return std::hash<std::string_view>()(sv);
}

size_t hashBryStringptrTransparent::operator()(const BryString *obj) const noexcept {
    return (*this)(static_cast<std::string_view>(obj->get_value()));
}

bool eqBryStringptrTransparent::operator()(const BryString *lhs, const BryString *rhs) const noexcept {
    if (lhs == rhs) return true;
    if (!lhs || !rhs) return false;
    return lhs->get_value() == rhs->get_value();
}

bool eqBryStringptrTransparent::operator()(std::string_view lhs, const BryString *rhs) const noexcept {
    return lhs == rhs->get_value();
}

bool eqBryStringptrTransparent::operator()(const BryString *lhs, std::string_view rhs) const noexcept {
    return lhs->get_value() == rhs;
}

bool eqBryStringptrTransparent::operator()(std::string_view lhs, std::string_view rhs) const noexcept {
    return lhs == rhs;
}
