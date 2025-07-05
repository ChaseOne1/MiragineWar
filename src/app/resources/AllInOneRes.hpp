#pragma once
#include "GUID.hpp"

namespace app {
namespace res {
#define RESOURCE(name, guid) \
    inline constexpr GUID_t name = guid;

    RESOURCE(TITLE_IMG, 7146235124056844766u)
    RESOURCE(DIALOG_BG_IMG , 4653213818835511564u)
    RESOURCE(BOOT_LO, 13858175482061098151u)
    RESOURCE(GROUND_IMG, 10786170426292651949u)

#undef RESOURCE
}
}
