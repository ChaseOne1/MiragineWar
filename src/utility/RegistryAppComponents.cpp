#include "Registry.hpp"
#include "app/Component/TextureGrid.hpp"
#include "app/Component/Text.hpp"
#include "app/Component/Texture.hpp"
#include "app/Component/ZIndex.hpp"
#include "app/Component/RenderCallback.hpp"

using namespace utility;

void Registry::RegisterAppComponents()
{
    // clang-format off
    RegisterComponents<
        app::comp::Texture,
        app::comp::ZIndex,
        app::comp::Text,
        app::comp::PreRender,
        app::comp::PostRender,
        app::comp::TextureGrid
    >();
    // clang-format on
}
