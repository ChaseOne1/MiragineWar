#include "Registry.hpp"
#include "app/Component/TextureGrid.hpp"
#include "app/ScriptManager.hpp"
#include "app/Component/Text.hpp"
#include "app/Component/Texture.hpp"
#include "app/Component/ZIndex.hpp"
#include "game/Component/Logic.hpp"
#include "game/Component/Movement.hpp"
#include "game/Component/UIClickable.hpp"
#include "game/Component/UIElement.hpp"
#include "app/Component/RenderCallback.hpp"
#include "game/Component/Transform.hpp"
#include "game/Component/UIPointable.hpp"

using namespace utility;

template <typename... Comps>
static void RegisterComponents(sol::environment& env)
{
    (Comps::RegisterToEnv(env), ...);
}

static decltype(auto) LuaEntityDestroy(sol::stack_table entity)
{
    return utility::Registry::GetRegistry().destroy(entity.raw_get<entt::entity>("id"));
}

static decltype(auto) LuaEntityValid(sol::stack_table entity)
{
    return utility::Registry::GetRegistry().valid(entity.raw_get<entt::entity>("id"));
}

void Registry::RegisterEnv(sol::environment& env)
{
    // clang-format off
    RegisterComponents<
        game::comp::UIElement,
        app::comp::Texture,
        game::comp::Transform,
        app::comp::ZIndex,
        app::comp::Text,
        app::comp::PreRender,
        app::comp::PostRender,
        game::comp::Movement,
        game::comp::Logic,
        app::comp::TextureGrid,
        game::comp::UIClickable,
        game::comp::UIPointable
    >(env);
    // clang-format on

    env["Entity"] = []() {
        return app::ScriptManager::GetLuaState().create_table_with(
            "id", GetRegistry().create(),
            "destroy", LuaEntityDestroy,
            "valid", LuaEntityValid);
    };
}
