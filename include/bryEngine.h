//
// Created by andreas on 11/26/25.
//

#ifndef BERRIE_BRYENGINE_H
#define BERRIE_BRYENGINE_H

#include <optional>
#include <unordered_set>
#include "bryStack.h"
#include "bryGC.h"
#include "result.h"

/// Forward declaration
class BryObject;
class BryString;
class Chunk;

class BryEngine {
public:
    enum EngineStatus {
        OK,
        RUNTIME_ERROR
    };
    BryEngine(Chunk* chunk);
    friend class BryParser;
    friend class BryObject;
    friend class GC;
    EngineStatus run();
private:
    using binaryfn = Result (*)(GC&, BryObject*, BryObject*);
    using unaryfn = Result (*)(GC&, BryObject*);
    std::uint16_t read_index() noexcept;
    std::uint8_t read_byte() noexcept;
    std::int32_t read_line() const noexcept;
    void print_error(std::string_view msg) const;
    std::optional<EngineStatus> execute_binary_op(binaryfn call);
    std::optional<EngineStatus> execute_unary_op(unaryfn call);

    GC gc_;
    BryStack<BryObject*, 255> stack_;
    std::unordered_map<BryString*, BryObject*, hashBryStringptr, eqBryStringptr> variable_environment_;
    std::unordered_set<BryString*, hashBryStringptrTransparent, eqBryStringptrTransparent> string_table_;
    Chunk* chunk_;
    size_t ip_;
};


#endif //BERRIE_BRYENGINE_H