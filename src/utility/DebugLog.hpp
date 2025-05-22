#pragma once

namespace utility {
inline void DebugLog(std::string_view msg)
{
#ifdef DEBUG
    SDL_Log("%s", msg.data());
#endif
}
}
