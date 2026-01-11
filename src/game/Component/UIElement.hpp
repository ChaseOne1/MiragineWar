#pragma once
#include "app/Layout.hpp"
#include "app/ScriptComponent.hpp"

namespace game::comp {
struct UIElement : app::ScriptComponent<UIElement>
{
    app::Layout::Anchor m_Anchor;

    UIElement(app::Layout::Anchor anchor)
        : m_Anchor(anchor)
    { }
};
}

#include "mirrow/srefl/srefl_begin.hpp"
// clang-format off
srefl_class(game::comp::UIElement,
    ctors(
        ctor(app::Layout::Anchor)
    )
 )
// clang-format on
#include "mirrow/srefl/srefl_end.hpp"
