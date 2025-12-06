//
// Created by andreas on 11/26/25.
//

#include "bryGC.h"
#include "bryObject.h"
#include "bryConstants.h"
#include "bryEngine.h"

GC::GC(BryEngine &engine) noexcept
    : engine_(engine), allocated_(0), threshold_(1024), objects_(nullptr)
{}

BryBool * GC::make_bool(bool value) const noexcept {
    return value == true ? bry_true_ : bry_false_;
}

BryString * GC::make_string(std::string_view sv) noexcept(false) {
    auto it = engine_.string_table_.find(sv);

    if (it != engine_.string_table_.end()) return  *it;

    auto* obj = make_object_impl<BryString>(sv);
    engine_.string_table_.insert(obj);
    return obj;
}

BryString * GC::make_string(const BryString &obj) noexcept(false) {
    return  make_string(static_cast<std::string_view>(obj.get_value()));
}


