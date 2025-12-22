#include "Registry.hpp"
#include "app/ScriptManager.hpp"
#include "app/Component/Render/Texture.hpp"
#include "app/Component/Render/ZIndex.hpp"
#include "game/Component/UIElement.hpp"
#include "game/Component/Transform.hpp"

using namespace utility;

#define GET_ID(ent) (ent.raw_get<entt::entity>("id"))

template <typename Ctor, typename C>
struct LuaEmplaceBound;

template <template <typename...> typename Ctor, typename... Ts, typename C>
struct LuaEmplaceBound<Ctor<Ts...>, C>
{
    // XXX: here is a copy
    static decltype(auto) Emplace(sol::stack_table entity, Ts... args)
    {
        if constexpr (std::is_void_v<std::invoke_result_t<decltype(&entt::registry::emplace_or_replace<C, Ts...>), entt::registry*, entt::entity, Ts...>>)
            utility::Registry::GetRegistry().emplace_or_replace<C>(GET_ID(entity), std::move(args)...);
        else
            return utility::Registry::GetRegistry().emplace_or_replace<C>(GET_ID(entity), std::move(args)...);
    }
};

template <typename C>
static decltype(auto) LuaRemove(sol::stack_table entity)
{
    return utility::Registry::GetRegistry().remove<C>(GET_ID(entity));
}

template <typename C>
static decltype(auto) LuaErase(sol::stack_table entity)
{
    return utility::Registry::GetRegistry().erase<C>(GET_ID(entity));
}

template <typename C>
static decltype(auto) LuaGet(sol::stack_table entity)
{
    return utility::Registry::GetRegistry().get<C>(GET_ID(entity));
}

template <typename C>
static decltype(auto) LuaHas(sol::stack_table entity)
{
    return utility::Registry::GetRegistry().all_of<C>(GET_ID(entity));
}

template <typename T>
static void RegisterComponent(sol::environment& env /*, sol::stack_table& ent */)
{
    auto refl = mirrow::srefl::reflect<T>();
    SDL_Log("Registry::RegisterComponent: %s", refl.name().data());

    sol::usertype<T> type = env.new_usertype<T>(refl.name(),
        "remove", LuaRemove<T>,
        "erase", LuaErase<T>,
        "get", LuaGet<T>,
        "has", LuaHas<T>
        );

    refl.visit_member_variables([&type](auto&& filed) {
        type[filed.name()] = filed.pointer();
    });

    if constexpr (refl.has_ctors()) {
        using ctor = mirrow::util::list_head_t<typename decltype(refl)::type::ctors>;
        type.set(sol::meta_function::construct, LuaEmplaceBound<ctor, T>::Emplace); // T.new()

        // ent[refl.name()] = LuaEmplaceBound<ctor, T>::Emplace; ent:T(params)
    }
}

template <typename... Comps>
static void RegisterComponents(sol::environment& env)
{
    (RegisterComponent<Comps>(env), ...);
}

static decltype(auto) LuaEntityDestroy(sol::stack_table entity, sol::this_state lua)
{
    return utility::Registry::GetRegistry().destroy(entity.raw_get<entt::entity>("id"));
}

static decltype(auto) LuaEntityValid(sol::stack_table entity, sol::this_state lua)
{
    return utility::Registry::GetRegistry().valid(entity.raw_get<entt::entity>("id"));
}

void Registry::RegisterEnv(sol::environment& env)
{
    RegisterComponents<game::comp::UIElement, app::comp::Texture, game::comp::Transform, app::comp::ZIndex, mathfu::vec2>(env);

    env["Entity"] = []() {
        return app::ScriptManager::GetLuaState().create_table_with(
            "id", GetRegistry().create(),
            "destroy", LuaEntityDestroy,
            "valid", LuaEntityValid);
    };
}