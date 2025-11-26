//
// Created by andreas on 11/26/25.
//

#include "bryGC.h"

#include "fmt/chrono.h"


GC::GC() noexcept
    : allocated_(0), threshold_(100), head_(nullptr)
{}

void GC::allocate(BObject *object) {

}
