#pragma once

#include <bitset>
#include <deque>
#include <forward_list>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <queue>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <toml++/toml.hpp>
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <entt/entt.hpp>
#include <mathfu/vector.h>
#include <mathfu/matrix.h>
#include <mathfu/glsl_mappings.h>
#include <mathfu/constants.h>
#include <sol/sol.hpp>
#include <mirrow/srefl/reflect.hpp>
#include <slikenet/peerinterface.h>
#include <slikenet/MessageIdentifiers.h>
#include <slikenet/types.h>
#include <slikenet/GetTime.h>
#include <slikenet/BitStream.h>

#include "utility/Singleton.hpp"
#include "utility/Logger.hpp"
