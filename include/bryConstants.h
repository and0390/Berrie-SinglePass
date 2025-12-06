//
// Created by andreas on 12/5/25.
//

#ifndef BERRIE_BRYCONSTANTS_H
#define BERRIE_BRYCONSTANTS_H

#include "bryObject.h"

inline BryBool bry_true(true);
inline BryBool bry_false(false);
inline BryNull bry_null;

/// Interned values
inline BryBool* bry_true_ = &bry_true;
inline BryBool* bry_false_ = &bry_false;
inline BryNull* bry_null_ = &bry_null;

#endif //BERRIE_BRYCONSTANTS_H