#include "Registry.hpp"
#include "app/ScriptManager.hpp"

using namespace utility;

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
    RegisterAppComponents();
    RegisterGameComponents();

    app::ScriptManager::GetLuaState()["Entity"] = []() {
        return app::ScriptManager::GetLuaState().create_table_with(
            "id", GetRegistry().create(),
            "destroy", LuaEntityDestroy,
            "valid", LuaEntityValid);
    };
}
