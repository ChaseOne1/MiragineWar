#pragma once

/* 0         7           15
 * +-------HEADER---------+
 * |   width | height     |
 * |----------------------|
 * |   count | pending    |
 * |----------------------|
 * |       float          |
 * +----------------------+
 * |                      |
 * |     IMAGES_DATA      |
 * |   (volatile size)    |
 * |                      |
 * +----------------------+
 */

struct SDL_Texture;

namespace app {
struct AnimSeqFrame
{
    struct AnimInfo
    {
        std::uint8_t width, height;
        std::uint8_t count, pending;
        float speed; // speed in ms
    } m_Idle, m_Run, m_Attack, m_Die;

    std::shared_ptr<SDL_Texture> m_Frames;
};
}
