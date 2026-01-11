#include "ScriptLib.hpp"
#include "ScriptTypes.hpp"
#include "app/Component/RenderCallback.hpp"
#include "app/Component/Texture.hpp"
#include "app/System/Time.hpp"
#include "app/resources/AnimSeqFrames.hpp"
#include "app/resources/Mox.hpp"

static decltype(auto) GenerateAnimCallback(app::comp::Texture& tex, app::AnimSeqFrames::ANIM lua_anim, const std::shared_ptr<app::AnimSeqFrames>& asf)
{
    return app::comp::PreRender::RenderCallback_t([&tex, lua_anim, start = app::sys::Time::NowMSec(), &asf]() mutable {
        const uint64_t passed = app::sys::Time::NowMSec() * asf->m_Info[lua_anim - 1].speed;
        tex.m_SrcFRect->x = asf->m_Info[lua_anim - 1].GetFrameX(passed);
    });
}

template <typename Ctor, typename T>
struct LuaConstructBound;

template <template <typename...> typename Ctor, typename... Ts, typename T>
struct LuaConstructBound<Ctor<Ts...>, T>
{
    // XXX: here is a copy
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
static void RegisterType(sol::environment& env)
{
    auto refl = mirrow::srefl::reflect<T>();
    SDL_Log("ScriptLib::RegisterType: %s", refl.name().data());
    sol::usertype<T> type = env.new_usertype<T>(refl.name().data() + std::string("Type"));

    refl.visit_fields([&type](auto&& field) { type[field.name()] = field.pointer(); });

    if constexpr (refl.has_ctors()) {
        using ctors = typename decltype(refl)::type::ctors;
        if constexpr (mirrow::util::list_size_v<ctors> == 1) {
            using ctor = mirrow::util::list_head_t<typename decltype(refl)::type::ctors>;
            type.set(sol::meta_function::construct, &LuaConstructBound<ctor, T>::Construct); // T.new()
        } else {
            type.set(sol::meta_function::construct, OverloadSet<ctors, T>::value); // T.new()
        }
    }
}

template <typename... Ts>
static void RegisterTypes(sol::environment& env)
{
    (RegisterType<Ts>(env), ...);
}

void app::ScriptLib::RegisterEnv(sol::environment& env)
{
    // clang-format off
    RegisterTypes<
        mathfu::vec2,
        SDL_FPoint,
        SDL_FRect,
        app::AnimSeqFrames,
        app::AnimSeqFrames::AnimInfo,
        app::Mox::Header,
        app::Mox
    >(env);
    // clang-format on

    auto type = env.new_usertype<ScriptLib>("ScriptLib", sol::no_constructor);
    type["GenerateAnimCallback"] = GenerateAnimCallback;
}
