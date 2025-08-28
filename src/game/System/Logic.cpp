#include "Logic.hpp"
#include "utility/Registry.hpp"
#include "game/Component/Logic.hpp"

using namespace game::sys;
using namespace entt;

void Logic::Tick()
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    auto view = reg.view<comp::Logic>();
    for (auto entity : view) {
        view.get<comp::Logic>(entity).m_fnTick();
    }
}
