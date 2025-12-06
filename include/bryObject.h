//
// Created by andreas on 11/22/25.
//

#ifndef BERRIE_BRYOBJECT_H
#define BERRIE_BRYOBJECT_H

#include "bryString.h"
#include "bryChunk.h"
#include "bryObject.h"
#include "bryObject.h"
#include "result.h"

/// Forward declaration
class BryInt;
class BryFloat;
class GC;
class BryString;
class BryEngine;

struct RuntimeError {
    std::string msg;
};

class BryObject {
public:
    enum ObjType {
        NUL,
        BOOL,
        INT,
        FLOAT,
        STRING,
        FUNCTION,
        SENTINEL // dummy value to keep track of the number of items
    };

    BryObject(BryObject* next, bool is_market) noexcept;
    BryObject() noexcept;
    virtual ~BryObject();
    void mark() noexcept;
    void unmark() noexcept;
    void set_next(BryObject* object) noexcept;
    BryObject *get_next() noexcept;
    virtual std::string to_str() const = 0;
    virtual BryObject::ObjType get_type() const noexcept = 0;
    /// Adds two BObject instances (`lhs` + `rhs`) according to their dynamic types.
    static Result add(GC &gc, BryObject *lhs, BryObject *rhs);
    static Result sub(GC &gc, BryObject *lhs, BryObject *rhs);
    static Result mul(GC &gc, BryObject *lhs, BryObject *rhs);
    static Result div(GC &gc, BryObject *lhs, BryObject *rhs);
    static Result eq(GC &gc, BryObject* lhs, BryObject* rhs);
    static Result ls(GC& gc, BryObject* lhs, BryObject* rhs);
    static Result gt(GC& gc, BryObject* lhs, BryObject* rhs);
    static Result nt(GC& gc, BryObject* rhs);
    static Result neg(GC& gc, BryObject* rhs);
    static Result pos(GC& gc, BryObject* rhs);

    static Result binary_no_operator_msg(const char *op, ObjType lhs_type, ObjType rhs_type);
    static Result unary_no_operator_msg(char op, ObjType rhs_type);

    virtual Result add(GC& gc, BryObject* rhs) noexcept = 0;
    virtual Result sub(GC& gc, BryObject* rhs) noexcept = 0;
    virtual Result mul(GC& gc, BryObject* rhs) noexcept = 0;
    virtual Result div(GC& gc, BryObject* rhs) noexcept = 0;
    virtual Result nt(GC& gc) noexcept = 0;
    virtual Result neg(GC& gc) noexcept = 0;
    virtual Result pos(GC& gc) noexcept = 0;
    virtual Result eq(GC& gc, BryObject* rhs) noexcept = 0;
    virtual Result ls(GC& gc, BryObject* rhs) noexcept = 0;
    virtual Result gt(GC& gc, BryObject* rhs) noexcept;

    BryObject* operator+(const BryObject& rhs) noexcept;
protected:
    BryObject* next_;
    bool is_marked_;
};

class BryInt : public BryObject {
public:
    BryInt(std::int64_t value) noexcept;
    std::string to_str() const override;
    std::int64_t get_value() const noexcept;
    Result add(GC &gc, BryObject *rhs) noexcept override;
    Result sub(GC &gc, BryObject *rhs) noexcept override;
    Result mul(GC &gc, BryObject *rhs) noexcept override;
    Result div(GC &gc, BryObject *rhs) noexcept override;
    Result nt(GC &gc) noexcept override;
    Result neg(GC &gc) noexcept override;
    Result pos(GC &gc) noexcept override;
    Result eq(GC &gc, BryObject *rhs) noexcept override;
    Result ls(GC &gc, BryObject *rhs) noexcept override;
    Result gt(GC &gc, BryObject *rhs) noexcept override;

    BryObject::ObjType get_type() const noexcept override;
protected:
    std::int64_t value_;
};

class BryBool : public  BryInt {
public:
    BryBool(bool value) noexcept;
    BryObject::ObjType get_type() const noexcept override;
};

class BryFloat : public BryObject {
public:
    BryFloat(double value) noexcept;
    std::string to_str() const override;
    double get_value() const noexcept;
    Result add(GC &gc, BryObject *rhs) noexcept override;
    Result sub(GC &gc, BryObject *rhs) noexcept override;
    Result mul(GC &gc, BryObject *rhs) noexcept override;
    Result div(GC &gc, BryObject *rhs) noexcept override;
    Result nt(GC &gc) noexcept override;
    Result neg(GC &gc) noexcept override;
    Result pos(GC &gc) noexcept override;
    Result eq(GC &gc, BryObject *rhs) noexcept override;
    Result ls(GC &gc, BryObject *rhs) noexcept override;
    Result gt(GC &gc, BryObject *rhs) noexcept override;

    BryObject::ObjType get_type() const noexcept override;
private:
    double value_;
    // size_t faction_;
};

class BryString : public BryObject {
public:
    friend struct hashBryStringptr;
    friend struct eqBryStringptr;
    BryString(std::string_view value);
    BryString(const String& str);
    BryString(String&& str);
    std::string to_str() const override;
    String get_value() const;
    Result add(GC &gc, BryObject *rhs) noexcept override;
    Result sub(GC &gc, BryObject *rhs) noexcept override;
    Result mul(GC &gc, BryObject *rhs) noexcept override;
    Result div(GC &gc, BryObject *rhs) noexcept override;
    Result nt(GC &gc) noexcept override;
    Result neg(GC &gc) noexcept override;
    Result pos(GC &gc) noexcept override;
    Result eq(GC &gc, BryObject *rhs) noexcept override;
    Result ls(GC &gc, BryObject *rhs) noexcept override;
    Result gt(GC &gc, BryObject *rhs) noexcept override;

    BryObject::ObjType get_type() const noexcept override;
private:
    String value_;
};

class BryNull : public BryObject {
public:
    BryNull();
    Result add(GC &gc, BryObject *rhs) noexcept override;
    Result sub(GC &gc, BryObject *rhs) noexcept override;
    Result mul(GC &gc, BryObject *rhs) noexcept override;
    Result div(GC &gc, BryObject *rhs) noexcept override;
    Result nt(GC &gc) noexcept override;
    Result neg(GC &gc) noexcept override;
    Result pos(GC &gc) noexcept override;
    Result eq(GC &gc, BryObject *rhs) noexcept override;
    Result ls(GC &gc, BryObject *rhs) noexcept override;
    Result gt(GC &gc, BryObject *rhs) noexcept override;

    std::string to_str() const override;
    BryObject::ObjType get_type() const noexcept override;
};

class BryFunction : public BryObject {
public:
    BryFunction(Chunk chunk, size_t upvalue_count, size_t arity, std::vector<std::int32_t> param_offset, BryString* name);
    Result add(GC &gc, BryObject *rhs) noexcept override;
    Result sub(GC &gc, BryObject *rhs) noexcept override;
    Result mul(GC &gc, BryObject *rhs) noexcept override;
    Result div(GC &gc, BryObject *rhs) noexcept override;
    Result nt(GC &gc) noexcept override;
    Result neg(GC &gc) noexcept override;
    Result pos(GC &gc) noexcept override;
    Result eq(GC &gc, BryObject *rhs) noexcept override;
    Result ls(GC &gc, BryObject *rhs) noexcept override;
    Result gt(GC &gc, BryObject *rhs) noexcept override;

    std::string to_str() const override;
    BryObject::ObjType get_type() const noexcept override;
private:
    Chunk chunk_;
    size_t upvalue_count_;
    size_t arity_;
    std::vector<std::int32_t> param_offset_;
    BryString* name_;
};


#endif //BERRIE_BRYOBJECT_H
