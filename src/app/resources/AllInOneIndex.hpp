#pragma once
#include "GUID.hpp"

namespace app {
namespace idx {

#define INDEX(name, guid) \
    inline constexpr GUID_t name = guid;
//--------------------------------------------------------
//generate by lower case name
    INDEX(MISC_IDX, 5582194082939956840u)
    INDEX(ENV_IDX, 16376991237526966004u)
//--------------------------------------------------------
#undef RESOURCE
}
}
