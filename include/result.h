//
// Created by andreas on 12/4/25.
//

#ifndef BERRIE_RESULT_H
#define BERRIE_RESULT_H

#include <boost/outcome/result.hpp>

/// forward declaration
class BryObject;

using namespace BOOST_OUTCOME_V2_NAMESPACE;
using Result = result<BryObject*, std::string, boost::outcome_v2::policy::all_narrow>;

#endif //BERRIE_RESULT_H