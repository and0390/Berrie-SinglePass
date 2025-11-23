//
// Created by andreas on 11/22/25.
//

#ifndef BERRIE_BRYOBJECT_H
#define BERRIE_BRYOBJECT_H

#include  <cstdint>
#include "bryString.h"
#include "bryChunk.h"

class BObject {
public:
    BObject(bool is_market) noexcept;
    BObject() noexcept;
    virtual ~BObject() = default;
    void mark() noexcept;
    void unmark() noexcept;
protected:
    bool is_marked_;
};

class BInt : public BObject {
public:
    BInt(std::int64_t value) noexcept;
protected:
    std::int64_t value_;
};

class BFloat : public BObject {
public:
    BFloat(double value) noexcept;
private:
    double value_;
};

class BString : public BObject {
public:
   BString(std::string_view value);
private:
    String value_;
};


class BFunction : public BObject {
public:
    BFunction(Chunk chunk, size_t upvalue_count, size_t arity, std::vector<std::int32_t> param_offset, BString* name);

private:
    Chunk chunk_;
    size_t upvalue_count_;
    size_t arity_;
    std::vector<std::int32_t> param_offset_;
    BString* name_;
};

#endif //BERRIE_BRYOBJECT_H