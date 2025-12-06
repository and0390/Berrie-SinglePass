//
// Created by andreas on 11/22/25.
//

#include "bryObject.h"
#include "bryObject.h"
#include "bryEngine.h"
#include "bryGC.h"
#include <boost/safe_numerics/safe_integer.hpp>
#include "result.h"

constexpr size_t NUM_TYPES = BryObject::SENTINEL;

#define BRY_ASSERT(rhs, src_type, dst_type, method_name) \
    assert((rhs) && "static conversion from '" #src_type "' to '" #dst_type "' failed in '" #method_name "'");
#define BRY_ASSERT1(rhs, dst_type, method_name)\
    BRY_ASSERT(rhs, BObject, dst_type, method_name)

using safeBryInt = boost::safe_numerics::safe<std::int64_t>;

const char* type_str(BryObject::ObjType type) {
    switch (type) {
        case BryObject::INT: return "int";
        case BryObject::BOOL: return "bool";
        case BryObject::FLOAT: return  "float";
        case BryObject::STRING: return "string";
        case BryObject::NUL: return "nullType";
        case BryObject::FUNCTION: return "function";
        default: return "unknown";
    }
}

BryObject::BryObject(BryObject* next, bool is_marked) noexcept
    : next_(next), is_marked_(is_marked)
{}

BryObject::BryObject() noexcept
    : BryObject(nullptr, false)
{}

BryObject::~BryObject() = default;

void BryObject::mark() noexcept {
    is_marked_ = true;
}

void BryObject::unmark() noexcept {
    is_marked_ = false;
}

void BryObject::set_next(BryObject *object) noexcept {
    next_ = object;
}

BryObject *BryObject::get_next() noexcept {
    return next_;
}

Result BryObject::add(GC &gc, BryObject *lhs, BryObject *rhs) {
    return lhs->add(gc, rhs);
}

Result BryObject::sub(GC &gc, BryObject *lhs, BryObject *rhs) {
    return  lhs->sub(gc, rhs);
}

Result BryObject::mul(GC &gc, BryObject *lhs, BryObject *rhs) {
    return lhs->mul(gc, rhs);
}

Result BryObject::div(GC &gc, BryObject *lhs, BryObject *rhs) {
    return lhs->div(gc, rhs);
}

Result BryObject::eq(GC &gc, BryObject *lhs, BryObject *rhs) {
    return lhs->eq(gc, rhs);
}

Result BryObject::ls(GC& gc, BryObject* lhs, BryObject* rhs) {
    return lhs->ls(gc, rhs);
}

Result BryObject::gt(GC& gc, BryObject* lhs, BryObject* rhs) {
    return lhs->gt(gc, rhs);
}

Result BryObject::nt(GC &gc, BryObject *rhs) {
    return rhs->nt(gc);
}

Result BryObject::neg(GC &gc, BryObject *rhs) {
    return rhs->neg(gc);
}

Result BryObject::pos(GC &gc, BryObject *rhs) {
    return rhs->pos(gc);
}

Result BryObject::binary_no_operator_msg(const char *op, ObjType lhs_type, ObjType rhs_type) {
    return fmt::format("unsupported operand for binary operator '{}' | '{}' and '{}'", *op, type_str(lhs_type), type_str(rhs_type));
}

Result BryObject::unary_no_operator_msg(char op, ObjType rhs_type) {
    return failure(fmt::format("unsupported operand for unary operator '{}' | '{}'", op, type_str(rhs_type)));
}

Result BryObject::nt(GC &gc) noexcept {
    return gc.make_object<BryBool>(false);
}

Result BryObject::gt(GC &gc, BryObject *rhs) noexcept {
    return rhs->ls(gc, this);
}

BryInt::BryInt(std::int64_t value) noexcept
    : BryObject(), value_(value)
{}

std::string BryInt::to_str() const {
    return std::to_string(value_);
}

std::int64_t BryInt::get_value() const noexcept {
    return value_;
}

Result BryInt::add(GC &gc, BryObject *rhs) noexcept {
    switch (rhs->get_type()) {
        case BOOL:
        case INT: {
            BryInt* conv_ptr = static_cast<BryInt*>(rhs);
            BRY_ASSERT1(conv_ptr, BInt, "BryInt::add");

            safeBryInt l_value = value_;
            safeBryInt r_value = conv_ptr->get_value();

            try {
                auto result = l_value + r_value;
                return gc.make_object<BryInt>(static_cast<std::int64_t>(result));
            } catch (const boost::safe_numerics::safe_numerics_error&) {
                return  failure(std::string("integer overflow / underflow occured"));
            }
        }
        case FLOAT: {
            BryFloat* conv_ptr = static_cast<BryFloat*>(rhs);
            BRY_ASSERT1(conv_ptr, BFloat, "BryInt::add")

            return gc.make_object<BryFloat>(value_ + conv_ptr->get_value());
        };
        case STRING: {
            BryString* conv_ptr = static_cast<BryString *>(rhs);
            BRY_ASSERT1(conv_ptr, BString, "BryInt::add");

            return gc.make_object<BryString>(value_ + conv_ptr->get_value());
        }
        default: return binary_no_operator_msg("+", get_type(), rhs->get_type());
    }
}

Result BryInt::sub(GC &gc, BryObject *rhs) noexcept {
    switch (rhs->get_type()) {
        case BOOL:
        case INT: {
            BryInt* conv_ptr = static_cast<BryInt *>(rhs);
            BRY_ASSERT1(conv_ptr, BryInt, "BryInt::sub");

            safeBryInt l_value = value_;
            safeBryInt r_value = conv_ptr->get_value();

            try {
                auto result = l_value - r_value;
                return gc.make_object<BryInt>(static_cast<std::int64_t>(result));
            } catch (const boost::safe_numerics::safe_numerics_error&) {
                return  failure(std::string("integer overflow / underflow occured"));
            }
        }
        case FLOAT: {
            BryFloat* conv_ptr = static_cast<BryFloat*>(rhs);
            BRY_ASSERT1(conv_ptr, BFloat, "BryInt::sub")

            auto r_value = conv_ptr->get_value();
            return gc.make_object<BryFloat>(value_ - r_value);
        }
        case STRING: {
            BryString* conv_ptr = static_cast<BryString *>(rhs);
            BRY_ASSERT1(conv_ptr, BString, "BryInt::sub");

            return gc.make_object<BryString>(value_ - conv_ptr->get_value());
        }
        default: return binary_no_operator_msg("-", get_type(), rhs->get_type());
    }
}

Result BryInt::mul(GC &gc, BryObject *rhs) noexcept {
    switch (rhs->get_type()) {
        case BOOL:
        case INT: {
            BryInt* conv_ptr = static_cast<BryInt*>(rhs);
            BRY_ASSERT1(conv_ptr, BInt, "BryInt::mul");

            safeBryInt l_value = value_;
            safeBryInt r_value = conv_ptr->value_;

            try {
                auto result = l_value * r_value;
                return gc.make_object<BryInt>(static_cast<std::int64_t>(result));
            } catch (const boost::safe_numerics::safe_numerics_error&) {
                return  failure(std::string("integer overflow / underflow occured"));
            }
        }
        case FLOAT: {
            BryFloat* conv_ptr = static_cast<BryFloat*>(rhs);
            BRY_ASSERT1(conv_ptr, BFloat, "BryInt::mul")

            return gc.make_object<BryFloat>(value_ + conv_ptr->get_value());
        };
        case STRING: {
            BryString* conv_ptr = static_cast<BryString *>(rhs);
            BRY_ASSERT1(conv_ptr, BString, "BryInt::mul");

            return gc.make_object<BryString>(value_ + conv_ptr->get_value());
        }
        default: return  binary_no_operator_msg("*", get_type(), rhs->get_type());
    }
}

Result BryInt::div(GC &gc, BryObject *rhs) noexcept {
    switch (rhs->get_type()) {
        case BOOL:
        case INT: {
            BryInt* conv_ptr = static_cast<BryInt*>(rhs);
            BRY_ASSERT1(conv_ptr, BInt, "BryInt::div");

            safeBryInt l_value = value_;
            safeBryInt r_value = conv_ptr->value_;

            try {
                auto result = l_value / r_value;
                return gc.make_object<BryInt>(static_cast<std::int64_t>(result));
            } catch (const boost::safe_numerics::safe_numerics_error&) {
                if (r_value == 0) return failure("division by zero");
                return failure("integer overflow occurred");
            }
        }
        case FLOAT: {
            BryFloat* conv_ptr = static_cast<BryFloat*>(rhs);
            BRY_ASSERT1(conv_ptr, BFloat, "BryInt::div")

            return gc.make_object<BryFloat>(value_ / conv_ptr->get_value());
        };
        default: return  binary_no_operator_msg("/", get_type(), rhs->get_type());
    }
}

Result BryInt::nt(GC &gc) noexcept {
    return gc.make_object<BryBool>(value_);
}

Result BryInt::neg(GC &gc) noexcept {
    return gc.make_object<BryInt>(value_ < 0 ? value_ : -value_);
}

Result BryInt::pos(GC &gc) noexcept {
    return gc.make_object<BryInt>(value_ > 0 ? value_ : +value_);
}

Result BryInt::eq(GC &gc, BryObject *rhs) noexcept {
    switch (rhs->get_type()) {
        case BOOL:
        case INT: {
            BryInt* conv_ptr = static_cast<BryInt *>(rhs);
            BRY_ASSERT1(conv_ptr, BryInt, "BryInt::eq")

            return gc.make_object<BryBool>(value_ == conv_ptr->get_value());
        }
        case FLOAT: {
            BryFloat* conv_ptr = static_cast<BryFloat *>(rhs);
            BRY_ASSERT1(conv_ptr, BryFloat, "BryInt::eq")

            return gc.make_object<BryBool>(value_ == conv_ptr->get_value());
        }
        default: return binary_no_operator_msg("==", get_type(), rhs->get_type());
    }
}

Result BryInt::ls(GC &gc, BryObject *rhs) noexcept {
    switch (rhs->get_type()) {
        case BOOL:
        case INT: {
            BryInt* conv_ptr = static_cast<BryInt *>(rhs);
            BRY_ASSERT1(conv_ptr, BryFloat, "BryInt::ls")
            return gc.make_object<BryBool>(value_ < conv_ptr->get_value());
        }
        case FLOAT: {
            BryFloat* conv_ptr = static_cast<BryFloat *>(rhs);
            BRY_ASSERT1(conv_ptr, BryFloat, "BryInt::ls")
            return gc.make_object<BryBool>(value_ < conv_ptr->get_value());
        }
        default: {
            return binary_no_operator_msg("<", get_type(), rhs->get_type());
        }
    }
}

Result BryInt::gt(GC &gc, BryObject *rhs) noexcept {
    return  BryObject::gt(gc, rhs);
}

BryObject::ObjType BryInt::get_type() const noexcept {
    return ObjType::INT;
}

BryBool::BryBool(bool value) noexcept
    : BryInt(value)
{}

BryObject::ObjType BryBool::get_type() const noexcept {
    return ObjType::BOOL;
}

BryFloat::BryFloat(double value) noexcept
    : BryObject(), value_(value)
{}

std::string BryFloat::to_str() const {
    return  std::to_string(value_);
}

double BryFloat::get_value() const noexcept {
    return value_;
}

Result BryFloat::add(GC &gc, BryObject *rhs) noexcept {
    switch (rhs->get_type()) {
        case BOOL:
        case INT: {
            BryInt* conv_ptr = static_cast<BryInt *>(rhs);
            BRY_ASSERT1(conv_ptr, BInt, "BryFloat::add");

            return gc.make_object<BryFloat>(value_ + conv_ptr->get_value());
        }
        case FLOAT: {
            BryFloat* conv_ptr = static_cast<BryFloat *>(rhs);
            BRY_ASSERT1(conv_ptr, BFloat, "BryFloat::add");

            return gc.make_object<BryFloat>(value_ + conv_ptr->get_value());
        }
        case STRING: {
            BryString* conv_ptr = static_cast<BryString *>(rhs);
            BRY_ASSERT1(conv_ptr, BString, "BryFloat::add");
            return  gc.make_object<BryString>(value_ + conv_ptr->get_value());
        }
        default: return binary_no_operator_msg("+", get_type(), rhs->get_type());
    }
}

Result BryFloat::sub(GC &gc, BryObject *rhs) noexcept {
    switch (rhs->get_type()) {
        case BOOL:
        case INT: {
            BryInt* conv_ptr = static_cast<BryInt *>(rhs);
            BRY_ASSERT1(conv_ptr, BInt, "BryFloat::sub");

            return gc.make_object<BryFloat>(value_ - conv_ptr->get_value());
        }
        case FLOAT: {
            BryFloat* conv_ptr = static_cast<BryFloat *>(rhs);
            BRY_ASSERT1(conv_ptr, BFloat, "BryFloat::sub");

            return gc.make_object<BryFloat>(value_ - conv_ptr->get_value());
        }
        case STRING: {
            BryString* conv_ptr = static_cast<BryString *>(rhs);
            BRY_ASSERT1(conv_ptr, BString, "BryFloat::sub");
            return  gc.make_object<BryString>(value_ - conv_ptr->get_value());
        }
        default: return binary_no_operator_msg("-", get_type(), rhs->get_type());
    }
}

Result BryFloat::mul(GC &gc, BryObject *rhs) noexcept {
    switch (rhs->get_type()) {
        case BOOL:
        case INT: {
            BryInt* conv_ptr = static_cast<BryInt *>(rhs);
            BRY_ASSERT1(conv_ptr, BInt, "BryFloat::mul");

            return gc.make_object<BryFloat>(value_ * conv_ptr->get_value());
        }
        case FLOAT: {
            BryFloat* conv_ptr = static_cast<BryFloat *>(rhs);
            BRY_ASSERT1(conv_ptr, BFloat, "BryFloat::mul");

            return gc.make_object<BryFloat>(value_ * conv_ptr->get_value());
        }
        case STRING: return failure(std::string("cannot multiply sequence by floating-point number"));
        default: return binary_no_operator_msg("*", get_type(), rhs->get_type());
    }
}

Result BryFloat::div(GC &gc, BryObject *rhs) noexcept {
    switch (rhs->get_type()) {
        case BOOL:
        case INT: {
            BryInt* conv_ptr = static_cast<BryInt *>(rhs);
            BRY_ASSERT1(conv_ptr, BInt, "BryFloat::div");

            return gc.make_object<BryFloat>(value_ / conv_ptr->get_value());
        }
        case FLOAT: {
            BryFloat* conv_ptr = static_cast<BryFloat *>(rhs);
            BRY_ASSERT1(conv_ptr, BFloat, "BryFloat::div");

            return gc.make_object<BryFloat>(value_ / conv_ptr->get_value());
        }
        default: return binary_no_operator_msg("/", get_type(), rhs->get_type());
    }
}

Result BryFloat::nt(GC &gc) noexcept {
    return gc.make_object<BryBool>(value_);
}

Result BryFloat::neg(GC &gc) noexcept {
    return gc.make_object<BryFloat>(value_ < 0 ? value_ : -value_);
}

Result BryFloat::pos(GC &gc) noexcept {
    return  gc.make_object<BryFloat>(value_ > 0 ? value_ : +value_);
}

Result BryFloat::eq(GC &gc, BryObject *rhs) noexcept {
    switch (rhs->get_type()) {
        case BOOL:
        case INT: {
            BryInt* conv_ptr = static_cast<BryInt *>(rhs);
            BRY_ASSERT1(conv_ptr, BryInt, "BryInt::eq")

            return gc.make_object<BryBool>(value_ == conv_ptr->get_value());
        }
        case FLOAT: {
            BryFloat* conv_ptr = static_cast<BryFloat *>(rhs);
            BRY_ASSERT1(conv_ptr, BryFloat, "BryInt::eq")

            return gc.make_object<BryBool>(value_ == conv_ptr->get_value());
        }
        default: return binary_no_operator_msg("==", get_type(), rhs->get_type());
    }
}

Result BryFloat::ls(GC &gc, BryObject *rhs) noexcept {
    switch (rhs->get_type()) {
        case BOOL:
        case INT: {
            BryInt* conv_ptr = static_cast<BryInt *>(rhs);
            BRY_ASSERT1(conv_ptr, BryFloat, "BryFloat::ls")
            return gc.make_object<BryBool>(value_ < conv_ptr->get_value());
        }
        case FLOAT: {
            BryFloat* conv_ptr = static_cast<BryFloat *>(rhs);
            BRY_ASSERT1(conv_ptr, BryFloat, "BryFloat::ls")
            return gc.make_object<BryBool>(value_ < conv_ptr->get_value());
        }
        default: {
            return binary_no_operator_msg("<", get_type(), rhs->get_type());
        }
    }
}

Result BryFloat::gt(GC &gc, BryObject *rhs) noexcept {
    return BryObject::gt(gc, rhs);
}

BryObject::ObjType BryFloat::get_type() const noexcept {
    return ObjType::FLOAT;
}

BryString::BryString(std::string_view value)
    : BryObject(), value_(value)
{}

BryString::BryString(const String &str)
    : BryObject(), value_(str)
{}

BryString::BryString(String &&str)
    : BryObject(), value_(std::move(str))
{}

std::string BryString::to_str() const {
    return std::string(value_.c_str());
}

String BryString::get_value() const {
    return value_;
}

Result BryString::add(GC &gc, BryObject *rhs) noexcept {
    switch (rhs->get_type()) {
        case BOOL:
        case INT : {
            BryInt* conv_ptr = static_cast<BryInt*>(rhs);
            BRY_ASSERT1(conv_ptr, BInt, "BryString::add");

            return  gc.make_object<BryString>(value_ + conv_ptr->get_value());
        }
        case FLOAT : {
            BryFloat* conv_ptr = static_cast<BryFloat*>(rhs);
            BRY_ASSERT1(conv_ptr, BInt, "BryString::add");

            return  gc.make_object<BryString>(value_ + conv_ptr->get_value());
        }
        case STRING: {
            BryString* conv_ptr = static_cast<BryString *>(rhs);
            BRY_ASSERT1(conv_ptr, BInt, "BryString::add");

            return  gc.make_object<BryString>(value_ + conv_ptr->get_value());
        }
        default: return  binary_no_operator_msg("+", get_type(), rhs->get_type());
    }
}

Result BryString::sub(GC &gc, BryObject *rhs) noexcept {
    switch (rhs->get_type()) {
        case BOOL:
        case INT : {
            BryInt* conv_ptr = static_cast<BryInt*>(rhs);
            BRY_ASSERT1(conv_ptr, BInt, "BryString::sub");

            return  gc.make_object<BryString>(value_ - conv_ptr->get_value());
        }
        case FLOAT : {
            BryFloat* conv_ptr = static_cast<BryFloat*>(rhs);
            BRY_ASSERT1(conv_ptr, BInt, "BryString::sub");

            return  gc.make_object<BryString>(value_ - conv_ptr->get_value());
        }
        case STRING: {
            BryString* conv_ptr = static_cast<BryString *>(rhs);
            BRY_ASSERT1(conv_ptr, BInt, "BryString::sub");

            return  gc.make_object<BryString>(value_ - conv_ptr->get_value());
        }
        default: return  binary_no_operator_msg("-", get_type(), rhs->get_type());
    }
}

Result BryString::mul(GC &gc, BryObject *rhs) noexcept {
    switch (rhs->get_type()) {
        case BOOL:
        case INT : {
            BryInt* conv_ptr = static_cast<BryInt*>(rhs);
            BRY_ASSERT1(conv_ptr, BInt, "BryString::mul");

            return  gc.make_object<BryString>(value_ * conv_ptr->get_value());
        }
        default: return failure(std::string("cannot multiply sequence by floating-point number"));
    }
}

Result BryString::div(GC &gc, BryObject *rhs) noexcept {
    return  binary_no_operator_msg("/", get_type(), rhs->get_type());
}

Result BryString::nt(GC &gc) noexcept {
    return gc.make_object<BryBool>(value_.size());
}

Result BryString::neg(GC &gc) noexcept {
    return unary_no_operator_msg('-', get_type());
}

Result BryString::pos(GC &gc) noexcept {
    return  unary_no_operator_msg('+', get_type());
}

Result BryString::eq(GC &gc, BryObject *rhs) noexcept {
    switch (rhs->get_type()) {
        case STRING: {
            BryString* conv_ptr = static_cast<BryString *>(rhs);
            BRY_ASSERT1(conv_ptr, BryString, "BryString::eq");

            return gc.make_object<BryBool>(value_ == conv_ptr->get_value());
        }
        default: return binary_no_operator_msg("==", get_type(), rhs->get_type());
    }
}

Result BryString::ls(GC &gc, BryObject *rhs) noexcept {
    switch (rhs->get_type()) {
        case STRING: {
            BryString* conv_ptr = static_cast<BryString *>(rhs);
            BRY_ASSERT1(conv_ptr, BryString, "BryString::ls");

            return gc.make_object<BryBool>(value_ < conv_ptr->get_value());
        }
        default: return binary_no_operator_msg("<", get_type(), rhs->get_type());
    }
}

Result BryString::gt(GC &gc, BryObject *rhs) noexcept {
    return BryObject::gt(gc, rhs);
}

BryObject::ObjType BryString::get_type() const noexcept {
    return ObjType::STRING;
}

BryNull::BryNull()
    : BryObject()
{}

Result BryNull::add(GC &gc, BryObject *rhs) noexcept {
    return binary_no_operator_msg("+", get_type(), rhs->get_type());
}

Result BryNull::sub(GC &gc, BryObject *rhs) noexcept {
    return binary_no_operator_msg("-", get_type(), rhs->get_type());
}

Result BryNull::mul(GC &gc, BryObject *rhs) noexcept {
    return binary_no_operator_msg("*", get_type(), rhs->get_type());
}

Result BryNull::div(GC &gc, BryObject *rhs) noexcept {
    return binary_no_operator_msg("/", get_type(), rhs->get_type());
}

Result BryNull::nt(GC &gc) noexcept {
    return gc.make_object<BryBool>(false);
}

Result BryNull::neg(GC &gc) noexcept {
    return unary_no_operator_msg('-', get_type());
}

Result BryNull::pos(GC &gc) noexcept {
    return  unary_no_operator_msg('+', get_type());
}

Result BryNull::eq(GC &gc, BryObject *rhs) noexcept {
    switch (rhs->get_type()) {
        case NUL: {
            BryNull* conv_ptr = static_cast<BryNull *>(rhs);
            BRY_ASSERT1(conv_ptr, BryString, "BryNull::eq");

            return gc.make_object<BryBool>(this == conv_ptr);
        }
        default: return binary_no_operator_msg("==", get_type(), rhs->get_type());
    }
}

Result BryNull::ls(GC &gc, BryObject *rhs) noexcept {
    switch (rhs->get_type()) {
        case NUL: return gc.make_object<BryBool>(false);
        default: return binary_no_operator_msg("<", get_type(), rhs->get_type());
    }
}

Result BryNull::gt(GC &gc, BryObject *rhs) noexcept {
    return BryObject::gt(gc, rhs);
}

std::string BryNull::to_str() const {
    return "None";
}

BryObject::ObjType BryNull::get_type() const noexcept {
    return BryObject::NUL;
}

BryFunction::BryFunction(Chunk chunk, size_t upvalue_count, size_t arity, std::vector<std::int32_t> param_offset, BryString *name)
    : chunk_(std::move(chunk)), upvalue_count_(upvalue_count), arity_(arity), param_offset_(std::move((param_offset)))
{}

std::string BryFunction::to_str() const {
    return name_->to_str();
}

Result BryFunction::add(GC &gc, BryObject *rhs) noexcept {
    return  binary_no_operator_msg("+", get_type(), rhs->get_type());
}

Result BryFunction::sub(GC &gc, BryObject *rhs) noexcept {
    return binary_no_operator_msg("-", get_type(), rhs->get_type());
}

Result BryFunction::mul(GC &gc, BryObject *rhs) noexcept {
    return binary_no_operator_msg("*", get_type(), rhs->get_type());
}

Result BryFunction::div(GC &gc, BryObject *rhs) noexcept {
    return  binary_no_operator_msg("/", get_type(), rhs->get_type());
}

Result BryFunction::nt(GC &gc) noexcept {
    return BryObject::nt(gc);
}

Result BryFunction::neg(GC &gc) noexcept {
    return  unary_no_operator_msg('-', get_type());
}

Result BryFunction::pos(GC &gc) noexcept {
    return unary_no_operator_msg('+', get_type());
}

Result BryFunction::eq(GC &gc, BryObject *rhs) noexcept {
    switch (rhs->get_type()) {
        case FUNCTION: {
            BryFunction* conv_ptr = static_cast<BryFunction *>(rhs);
            BRY_ASSERT1(conv_ptr, BryString, "BryFunction::eq");

            return gc.make_object<BryBool>(this == conv_ptr);
        }
        default: return binary_no_operator_msg("==", get_type(), rhs->get_type());
    }
}

Result BryFunction::ls(GC &gc, BryObject *rhs) noexcept {
    switch (rhs->get_type()) {
        case FUNCTION: return  gc.make_object<BryBool>(false);
        default: return binary_no_operator_msg("<", get_type(), rhs->get_type());
    }
}

Result BryFunction::gt(GC &gc, BryObject *rhs) noexcept {
    return BryObject::gt(gc, rhs);
}

BryObject::ObjType BryFunction::get_type() const noexcept {
    return ObjType::NUL;
}


