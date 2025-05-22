#pragma once

namespace game::comp {
struct Clickable
{
    std::function<void()> m_pfCallback {};
};
}
