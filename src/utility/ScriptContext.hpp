#pragma once
#include "Registry.hpp"
#include "utility/Topics.hpp"

namespace utility::detail {
constexpr std::string_view LUA_REGISTRY_KEY = "_registry_";

namespace comp {
    template <typename Ctor, typename C>
    struct LuaEmplaceBound;

    template <template <typename...> typename Ctor, typename... Ts, typename C>
    struct LuaEmplaceBound<Ctor<Ts...>, C>
    {
        static decltype(auto) Emplace(sol::this_environment env, sol::stack_table entity, Ts... args)
        {
            if constexpr (std::is_void_v<std::invoke_result_t<decltype(&entt::registry::emplace_or_replace<C, Ts...>),
                              entt::registry*, entt::entity, Ts...>>)
                utility::Registry::GetRegistry().emplace_or_replace<C>(entity.raw_get<entt::entity>("id"), std::move(args)...);
            else
                return utility::Registry::GetRegistry().emplace_or_replace<C>(entity.raw_get<entt::entity>("id"), std::move(args)...);
        }
    };

    template <typename C>
    static decltype(auto) LuaRemove(sol::stack_table entity)
    {
        return utility::Registry::GetRegistry().remove<C>(entity.raw_get<entt::entity>("id"));
    }

    template <typename C>
    static decltype(auto) LuaGet(sol::stack_table entity)
    {
        return utility::Registry::GetRegistry().get<C>(entity.raw_get<entt::entity>("id"));
    }

    template <typename C>
    static decltype(auto) LuaHas(sol::this_environment env, sol::stack_table entity)
    {
        const entt::registry& registry = *static_cast<sol::environment>(env).get<entt::registry*>(LUA_REGISTRY_KEY);
        return registry.all_of<C>(entity.raw_get<entt::entity>("id"));
    }

    template <typename Ctors, typename C>
    struct OverloadSet;

    template <template <typename...> typename Ctors, typename... ArgsTuples, typename C>
    struct OverloadSet<Ctors<ArgsTuples...>, C>
    {
        inline static const sol::overload_set value = sol::overload(&LuaEmplaceBound<ArgsTuples, C>::Emplace...);
    };
}

namespace type {
    template <typename Ctor, typename T>
    struct LuaConstructBound;

    template <template <typename...> typename Ctor, typename... Ts, typename T>
    struct LuaConstructBound<Ctor<Ts...>, T>
    {
        static decltype(auto) Construct(Ts... args) { return T { std::move(args)... }; }
    };

    template <typename Ctors, typename C>
    struct OverloadSet;

    template <template <typename...> typename Ctors, typename... ArgsTuples, typename C>
    struct OverloadSet<Ctors<ArgsTuples...>, C>
    {
        inline static const sol::overload_set value = sol::overload(&LuaConstructBound<ArgsTuples, C>::Construct...);
    };

    template <typename T>
    static void RegisterType(sol::environment& env);
}
}

namespace utility {
class ScriptContext
{
public:
    static constexpr std::string_view SMI_Initialize = "init";
    static constexpr std::string_view SMI_Reload = "reload";

    sol::environment m_env;
    sol::state_view m_state { m_env.lua_state() };
    sol::table m_this_module;

public:
    ScriptContext(std::string_view filename);
    ~ScriptContext();

    bool IsReady() const noexcept { return m_env.valid(); }

    template <typename T>
    void RegisterComponent()
    {
        auto refl = mirrow::srefl::reflect<T>();
        LOGD("RegisterComponent: {}", refl.name().data());

        sol::state_view state(m_env.lua_state());
        const std::string require_code = fmt::format(R"(return require("builtin.components.{}"))", refl.name());
        sol::table prototype = state.script(require_code, m_env);
        auto instance_type = m_env.new_usertype<T>(fmt::format("_{}_", refl.name()));

        if constexpr (refl.has_ctors()) {
            using ctors = typename decltype(refl)::type::ctors;
            if constexpr (mirrow::util::list_size_v<ctors> == 1) {
                using ctor = mirrow::util::list_head_t<typename decltype(refl)::type::ctors>;
                prototype["new"] = &detail::comp::LuaEmplaceBound<ctor, T>::Emplace;
            } else {
                prototype["new"] = detail::comp::OverloadSet<ctors, T>::value;
            };
        }

        prototype["has"] = &detail::comp::LuaHas<T>;
        prototype["get"] = &detail::comp::LuaGet<T>;
        prototype["remove"] = &detail::comp::LuaRemove<T>;

        refl.visit_fields([&instance_type](auto&& field) { instance_type[field.name()] = field.pointer(); });
    }

    void RegisterTypes();
    void RegisterEntity();
    void RegisterComponents();
    void RegisterScriptLib();

private:
    sol::table m_package_loaded;
    std::forward_list<std::pair<std::filesystem::path, TopicsSubscriberID>> m_SubscriberIDs;

    void OnFileChanged(const std::filesystem::path&, const std::string&);
    sol::object LuaRequire(sol::string_view, sol::environment&);
    sol::load_result LoadFileAsModule(const std::string&, sol::environment&);
};
}
