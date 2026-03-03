#include "Registry.hpp"
#include "app/ScriptManager.hpp"

using namespace utility;

<<<<<<< HEAD
=======
template <typename... Comps>
static void RegisterComponents()
{
    (Comps::RegisterToEnv(), ...);
}

>>>>>>> 343712d6ff595ccd56d5f2e647c983ac9f822329
static decltype(auto) LuaEntityDestroy(sol::stack_table entity)
{
    return utility::Registry::GetRegistry().destroy(entity.raw_get<entt::entity>("id"));
}

static decltype(auto) LuaEntityValid(sol::stack_table entity)
{
    return utility::Registry::GetRegistry().valid(entity.raw_get<entt::entity>("id"));
}

void Registry::RegisterToLua()
{
<<<<<<< HEAD
    RegisterAppComponents();
    RegisterGameComponents();
=======
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
    >();
    // clang-format on
>>>>>>> 343712d6ff595ccd56d5f2e647c983ac9f822329

    app::ScriptManager::GetLuaState()["Entity"] = []() {
        return app::ScriptManager::GetLuaState().create_table_with(
            "id", GetRegistry().create(),
            "destroy", LuaEntityDestroy,
            "valid", LuaEntityValid);
    };
}
