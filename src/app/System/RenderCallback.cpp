#include "RenderCallback.hpp"
#include "app/Component/RenderCallback.hpp"
#include "utility/Registry.hpp"

using namespace app::sys;

void PreRender::Tick()
{
    auto& reg = utility::Registry::GetInstance().GetRegistry();
    auto view = reg.view<app::comp::PreRender>();

    for (auto& entity : view) {
        view.get<app::comp::PreRender>(entity).m_fnPreRender();
    }
}

void PostRender::Tick()
{
    auto& reg = utility::Registry::GetInstance().GetRegistry();
    auto view = reg.view<app::comp::PostRender>();

    for (auto& entity : view) {
        view.get<app::comp::PostRender>(entity).m_fnPostRender();
    }
}
