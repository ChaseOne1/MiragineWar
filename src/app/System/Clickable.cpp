#include "Clickable.hpp"
#include "Input.hpp"
#include "app/Component/Clickable.hpp"

using namespace app::sys;

Clickable::Clickable()
{
    auto& reg = utility::Registry::GetInstance().GetRegistry();
    reg.on_construct<comp::Clickable>().connect<&Clickable::AddClickable>(this);
    reg.on_destroy<comp::Clickable>().connect<&Clickable::RemoveClickable>(this);

    Input::GetInstance().Subsrcibe(InputTopic::BUTTON_EVENT, std::bind(&Clickable::Tick, this));
}

void Clickable::AddClickable(entt::registry& reg, entt::entity entity)
{
    auto& clickable = reg.get<comp::Clickable>(entity);
    m_Clickables.Insert(entity, clickable.m_MBR);
}

void Clickable::RemoveClickable(entt::registry& reg, entt::entity entity)
{
    // auto& clickable = reg.get<comp::Clickable>(entity);
    // m_Clickables.Delete(entity /*, clickable.m_MBR*/);
}

void Clickable::Tick()
{
    m_Clicked = nullptr;
    //we are interested in button up
    if (Input::GetInstance().IsPressed(Key::MOUSE_SELECT)) return;

    SDL_Event* event = EventBus::GetInstance().GetEvent();

    std::vector<entt::entity> res;
    m_Clickables.Search({ int(event->button.x), int(event->button.y) }, res);

    if (!res.size()) return;
    m_Clicked = &res.front();
    auto& reg = utility::Registry::GetInstance().GetRegistry();
    comp::Clickable& clickable = reg.get<comp::Clickable>(*m_Clicked);
    clickable.m_fnCallback();
}
