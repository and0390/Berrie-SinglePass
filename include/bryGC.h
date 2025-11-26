//
// Created by andreas on 11/26/25.
//

#ifndef BERRIE_BRYGC_H
#define BERRIE_BRYGC_H
#include <cstddef>

class BObject; // Forward declaration of BObject

class GC {
public:
    GC() noexcept;

    void allocate(BObject* object);


private:
    std::size_t allocated_;
    std::size_t threshold_;
    BObject* head_;
};



#endif //BERRIE_BRYGC_H