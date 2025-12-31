#include "ScriptLib.hpp"

#include "Resources.hpp"
#include "ScriptTypes.hpp"
#include "app/Component/Render/Text.hpp"

using namespace app;

static std::tuple<int, int> GetTextSize(app::comp::Text& text)
{
    std::tuple<int, int> size;
    TTF_GetTextSize(text.m_Text.get(), &std::get<0>(size), &std::get<1>(size));
    return size;
}

// alignment after wrapping
static void SetFontWarpAlignment(std::shared_ptr<void>& font, TTF_HorizontalAlignment alignment) { TTF_SetFontWrapAlignment((TTF_Font*)font.get(), alignment); }

template <typename Ctor, typename T>
struct LuaConstructBound;

template <template <typename...> typename Ctor, typename... Ts, typename T>
struct LuaConstructBound<Ctor<Ts...>, T>
{
    // XXX: here is a copy
    static decltype(auto) Construct(sol::stack_table entity, Ts... args) { return T{ std::move(args)... }; }
};

template <typename T>
static void RegisterType(sol::environment& env)
{
    auto refl = mirrow::srefl::reflect<T>();
    SDL_Log("ScriptLib::RegisterType: %s", refl.name().data());
    sol::usertype<T> type = env.new_usertype<T>(refl.name().data() + std::string("Type"));

    refl.visit_member_variables([&type](auto&& filed) { type[filed.name()] = filed.pointer(); });

    if constexpr (refl.has_ctors()) {
        using ctor = mirrow::util::list_head_t<typename decltype(refl)::type::ctors>;
        type.set(sol::meta_function::construct, LuaConstructBound<ctor, T>::Construct); // T.new()
    }
}

template <typename... Ts>
static void RegisterTypes(sol::environment& env)
{
    (RegisterType<Ts>(env), ...);
}

void ScriptLib::RegisterEnv(sol::environment& env)
{
    RegisterTypes<
        mathfu::vec2,
        SDL_FPoint
    >(env);

    env.new_usertype<ScriptLib>("ScriptLib", sol::no_constructor,
        "GetTextSize", GetTextSize,
        "SetFontWarpAlignment", SetFontWarpAlignment
    );

    std::shared_ptr<void> ptr = std::make_shared<int>(10);
}