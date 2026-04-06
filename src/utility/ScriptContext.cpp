#include "ScriptContext.hpp"
#include "ScriptTypes.hpp"

#include "app/Component/Texture.hpp"
#include "app/Component/Text.hpp"
#include "app/Component/ZIndex.hpp"
#include "app/Component/TextureRotation.hpp"
#include "app/Component/TextureGrid.hpp"
#include "app/Component/RenderCallback.hpp"

#include "app/resources/AnimSeqFrames.hpp"
#include "game/Component/Synchronize.hpp"
#include "game/Component/Transform.hpp"
#include "game/Component/Movement.hpp"
#include "game/Component/Logic.hpp"
#include "game/Component/Visible.hpp"
#include "game/Component/SoldierTag.hpp"
#include "game/Component/UIElement.hpp"
#include "game/Component/UIPointable.hpp"
#include "game/Component/UIClickable.hpp"

#include "app/System/Time.hpp"

using namespace utility;

ScriptContext::ScriptContext(std::string_view filename)
    : m_env(ScriptManager::GetLuaState(), sol::create, ScriptManager::GetLuaState().globals())
    , m_package_loaded(ScriptManager::GetLuaState(), sol::create)
{
    m_env["require"] =
        [this](const sol::string_view name, sol::this_environment env) { return LuaRequire(name, env); };
    m_env["package"] = ScriptManager::GetLuaState().create_table_with("loaded", m_package_loaded);
    for (const auto& [k, v] : ScriptManager::GetLuaState()["package"]["loaded"].get<sol::table>()) {
        m_package_loaded[k] = v;
    }

    sol::load_result script(
        ScriptManager::GetLuaState().load_file(ScriptManager::GetScriptFilePathString(filename)));
    if (!script.valid()) {
        LOGE("load {} failed, status: {}", filename, static_cast<std::size_t>(script.status()));
        m_env = sol::nil;
        return;
    }

    sol::protected_function script_func = script.get<sol::protected_function>();
    sol::set_environment(m_env, script_func);
    if (const auto result = script_func(); result.valid() && result.get_type() == sol::type::table) {
        m_this_module = result.get<sol::table>();
    } else {
        LOGE("do {} failed, status: {}", filename, static_cast<std::size_t>(result.status()));
        m_env = sol::nil;
        return;
    }
}

ScriptContext::~ScriptContext()
{
    for (auto& [path, sid] : m_SubscriberIDs) ScriptManager::Unsubscribe(path, sid);
}

sol::load_result ScriptContext::LoadFileAsModule(const std::string& path, sol::environment& env)
{
    sol::load_status status = static_cast<sol::load_status>(luaL_loadfilex(env.lua_state(), path.c_str(),
        sol::to_string(sol::load_mode::any).c_str()));
    if (status == sol::load_status::ok) {
        env.push();
        lua_setupvalue(env.lua_state(), -2, 1);
    }
    return sol::load_result(env.lua_state(), sol::absolute_index(env.lua_state(), -1), 1, 1, status);
}

void ScriptContext::OnFileChanged(const std::filesystem::path& path, const std::string& name)
{
    sol::load_result result = LoadFileAsModule(path.string(), m_env);
    if (result.valid()) {
        sol::table new_module = result();
        sol::table old_module = m_env["package"]["loaded"][name];
        for (auto& kv : new_module) old_module[kv.first] = kv.second;
        if (sol::object meta = new_module[sol::metatable_key]; meta.valid()) old_module[sol::metatable_key] = meta;
    } else {
        LOGE("failed to reload module {} from {}, status: {}",
            name, path.string(), static_cast<std::size_t>(result.status()));
    }
}

sol::object ScriptContext::LuaRequire(sol::string_view name, sol::environment& env)
{
    if (sol::object module = m_package_loaded[name]; module.valid())
        return module;

    std::string path(name.size(), '\0');
    std::replace_copy(name.cbegin(), name.cend(), path.begin(), '.', '/');
    path = fmt::format("{}{}.lua", ScriptManager::SCRIPT_ROOT_PATH.string(), path);

    sol::load_result result = LoadFileAsModule(path, env);
    if (!result.valid()) {
        LOGE("load {} failed when require, status: {}",
            path, static_cast<std::size_t>(result.status()));
        return {};
    }

    sol::object new_module = result();
    m_package_loaded[name] = new_module;

    // cannot move any path in them, because the evaluation order is uncertain
    m_SubscriberIDs.emplace_front(path,
        ScriptManager::Subscribe(path, [this, path, name = std::string(name)]() { OnFileChanged(path, name); }));

    return new_module;
}

namespace utility::detail::type {
template <typename T>
static void RegisterType(sol::environment& env)
{
    auto refl = mirrow::srefl::reflect<T>();
    LOGD("RegisterType: {}", refl.name().data());

    sol::state_view state(env.lua_state());
    const std::string require_code = fmt::format(R"(return require("builtin.types.{}"))", refl.name());
    sol::table prototype = state.script(require_code, env);
    sol::usertype<T> type = env.new_usertype<T>(fmt::format("_{}_", refl.name()));

    if constexpr (refl.has_ctors()) {
        using ctors = typename decltype(refl)::type::ctors;
        if constexpr (mirrow::util::list_size_v<ctors> == 1) {
            using ctor = mirrow::util::list_head_t<typename decltype(refl)::type::ctors>;
            prototype["new"] = &LuaConstructBound<ctor, T>::Construct;
        } else {
            prototype["new"] = OverloadSet<ctors, T>::value;
        }
    }

    refl.visit_fields([&type](auto&& field) { type[field.name()] = field.pointer(); });
}
}

void ScriptContext::RegisterTypes()
{
    using detail::type::RegisterType;
    RegisterType<mathfu::vec2>(m_env);
    RegisterType<SDL_FPoint>(m_env);
    RegisterType<SDL_FRect>(m_env);
    RegisterType<app::Mox>(m_env);
    RegisterType<app::MoxHeader>(m_env);
    RegisterType<app::AnimSeqFrames>(m_env);
    RegisterType<app::AnimSeqFrames::AnimInfo>(m_env);
}

void ScriptContext::RegisterEntity()
{
    sol::table entity = m_state.script(R"(return require("builtin.entity"))", m_env);

    entity["new"] = [this]() mutable {
        return m_state.create_table_with(
            "id", utility::Registry::GetRegistry().create(),
            "valid", [&](sol::stack_table entity) { return utility::Registry::GetRegistry()
                                                        .valid(entity.raw_get<entt::entity>("id")); },
            "destroy", [&](sol::stack_table entity) { utility::Registry::GetRegistry()
                                                          .destroy(entity.raw_get<entt::entity>("id")); });
    };
}

void ScriptContext::RegisterComponents()
{
    using namespace app::comp;
    using namespace game::comp;

    RegisterComponent<Texture>();
    RegisterComponent<Text>();
    RegisterComponent<ZIndex>();
    RegisterComponent<TextureRotation>();
    RegisterComponent<TextureGrid>();
    RegisterComponent<PreRender>();
    RegisterComponent<PostRender>();
    RegisterComponent<Transform>();
    RegisterComponent<Movement>();
    RegisterComponent<Logic>();
    RegisterComponent<Visible>();
    RegisterComponent<SoldierTag>();
    RegisterComponent<UIElement>();
    RegisterComponent<UIPointable>();
    RegisterComponent<UIClickable>();
    RegisterComponent<SoldierTag>();
    RegisterComponent<Synchronize>();
}

static decltype(auto) GenerateAnimCallback(app::comp::Texture& tex, app::AnimSeqFrames::ANIM lua_anim, const std::shared_ptr<app::AnimSeqFrames>& asf)
{
    return app::comp::PreRender::RenderCallback_t([&tex, lua_anim, start = app::sys::Time::NowMSec(), &asf]() mutable {
        const uint64_t passed = app::sys::Time::NowMSec() * asf->m_Info[lua_anim - 1].speed;
        tex.m_SrcFRect->x = asf->m_Info[lua_anim - 1].GetFrameX(passed);
    });
}

void ScriptContext::RegisterScriptLib()
{
    sol::table script_lib = m_state.script(R"(return require("builtin.script_lib"))", m_env);
    script_lib["generate_anim_callback"] = GenerateAnimCallback;
}
