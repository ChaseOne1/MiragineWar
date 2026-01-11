#pragma once
#include "utility/Registry.hpp"

namespace app {
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

template <typename Ctors, typename C>
struct OverloadSet;

template <template <typename...> typename Ctors, typename... ArgsTuples, typename C>
struct OverloadSet<Ctors<ArgsTuples...>, C>
{
    inline static const sol::overload_set value = sol::overload(&LuaEmplaceBound<ArgsTuples, C>::Emplace...);
};

template <typename T>
static void RegisterComponent(sol::environment& env /*, sol::stack_table& ent */)
{
    auto refl = mirrow::srefl::reflect<T>();
    SDL_Log("Registry::RegisterComponent: %s", refl.name().data());
    auto type = env.new_usertype<T>(refl.name().data() + std::string("Comp"));
    type["remove"] = LuaRemove<T>;
    type["erase"] = LuaErase<T>;
    type["get"] = LuaGet<T>;
    type["has"] = LuaHas<T>;

    refl.visit_fields([&type](auto&& field) { type[field.name()] = field.pointer(); });

    if constexpr (refl.has_ctors()) {
        using ctors = typename decltype(refl)::type::ctors;
        if constexpr (mirrow::util::list_size_v<ctors> == 1) {
            using ctor = mirrow::util::list_head_t<typename decltype(refl)::type::ctors>;
            type.set(sol::meta_function::construct, &LuaEmplaceBound<ctor, T>::Emplace); // T.new()
        } else {
            type.set(sol::meta_function::construct, OverloadSet<ctors, T>::value); // T.new()
        }
    }
}

template <typename C>
struct ScriptComponent
{
    static void RegisterToEnv(sol::environment& env)
    {
        RegisterComponent<C>(env);
    }
};

// CRTP disables aggregate initialization, which makes things more complicated sometimes,
// using macros to provide another convenient method.
#define REGISTER_TO_ENV(T) static void RegisterToEnv(sol::environment& env) { RegisterComponent<T>(env); }

#undef GET_ID
}
