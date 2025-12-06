//
// Created by andreas on 11/26/25.
//

#ifndef BERRIE_BRYGC_H
#define BERRIE_BRYGC_H

#include <cstddef>
#include <string_view>
#include <utility>

// Forward declaration of BObject
class BryObject;
class BryBool;
class BryString;
class BryEngine;

class GC {
public:
    GC(BryEngine& engine) noexcept;
    template<typename T, typename... Args>
    [[nodiscard]]
    T* make_object(Args&&... args) {
        if constexpr (std::is_same_v<T, BryBool>) {
            /// BryBool is stack allocated
            static_assert(sizeof...(args) == 1, "from make_object BryBool must have exactly one argument");
            return make_bool(std::get<0>(std::tuple{args...}));
        }
        else if constexpr (std::is_same_v<T, BryString>) {
            static_assert(sizeof...(args) == 1, "from make_object BryInt must have exactly one argument");
            return make_string(std::get<0>(std::tuple{args...}));
        }
        else {
            T *obj = make_object_impl<T>(std::forward<Args>(args)...);

            // Additional logic for mark and sweep later
            if (allocated_ > threshold_) {

            }
            return obj;
        }
    }
private:
    BryBool* make_bool(bool value) const noexcept;
    BryString* make_string(std::string_view sv) noexcept(false);
    BryString* make_string(const BryString& obj) noexcept(false);

    template<typename T, typename... Args>
    [[nodiscard]]
    T* make_object_impl(Args&&... args) {
        T *obj = new T(std::forward<Args>(args)...);
        obj->set_next(objects_);
        objects_ = obj;
        allocated_ += sizeof(T);
        return obj;
    }

    std::size_t allocated_;
    std::size_t threshold_;
    BryEngine& engine_;
    BryObject* objects_;
};



#endif //BERRIE_BRYGC_H