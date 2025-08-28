#include "GarbageCollection.hpp"
#include "game/Component/Invalid.hpp"
#include "utility/Registry.hpp"

using namespace game::sys;

void GarbageCollection::Tick()
{
    auto& reg = utility::Registry::GetInstance().GetRegistry();
    auto view = reg.view<game::comp::Invalid>();
    reg.destroy(view.begin(), view.end());
}
