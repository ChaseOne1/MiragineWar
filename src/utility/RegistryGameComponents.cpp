#include "Registry.hpp"
#include "game/Component/Logic.hpp"
#include "game/Component/Movement.hpp"
#include "game/Component/UIClickable.hpp"
#include "game/Component/UIElement.hpp"
#include "game/Component/Transform.hpp"
#include "game/Component/UIPointable.hpp"

using namespace utility;

void Registry::RegisterGameComponents()
{
    // clang-format off
    RegisterComponents<
        game::comp::UIElement,
        game::comp::Transform,
        game::comp::Movement,
        game::comp::Logic,
        game::comp::UIClickable,
        game::comp::UIPointable
    >();
    // clang-format on
}
