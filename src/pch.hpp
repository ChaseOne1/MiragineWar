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

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <entt/entt.hpp>
#include <mathfu/vector.h>
#include <mathfu/matrix.h>
#include <mathfu/glsl_mappings.h>
#include <mathfu/constants.h>

// be careful with the order of headers we write,
// as it may cause some symbol errors

/*clang-format off*/
#include "utility/Singleton.hpp"
#include "utility/Registry.hpp"
#include "app/AppMetadata.hpp"
#include "app/EventBus.hpp"
#include "app/Settings.hpp"
#include "app/Window.hpp"
#include "app/Renderer.hpp"
/*clang-format on*/
