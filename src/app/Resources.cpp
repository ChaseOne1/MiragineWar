#include "Resources.hpp"

using namespace app;

template <>
std::shared_ptr<SDL_Surface> Resources::LoadFromMem<SDL_Surface>(const ResDesc& desc, std::unique_ptr<std::byte[]> data)
{
    SDL_IOStream* stream = SDL_IOFromConstMem(data.get(), desc.m_nSize);
    return std::shared_ptr<SDL_Surface>(IMG_Load_IO(stream, true),
        [](SDL_Surface* surface) { SDL_DestroySurface(surface); });
}

template <>
std::shared_ptr<SDL_Texture> Resources::LoadFromMem<SDL_Texture>(const ResDesc& desc, std::unique_ptr<std::byte[]> data)
{
    SDL_IOStream* stream = SDL_IOFromConstMem(data.get(), desc.m_nSize);
    return std::shared_ptr<SDL_Texture>(IMG_LoadTexture_IO(app::Renderer::GetInstance().GetSDLRenderer(), stream, true),
        [](SDL_Texture* texture) { SDL_DestroyTexture(texture); });
}

template <>
std::shared_ptr<toml::table> Resources::LoadFromMem<toml::table>(const ResDesc& desc, std::unique_ptr<std::byte[]> data)
{
    // TODO: no tested
    // NOTE: just support ascii code
    return std::make_shared<toml::table>(toml::parse({ reinterpret_cast<const char*>(data.get()), desc.m_nSize }));
}

template <>
std::shared_ptr<AnimSeqFrame> Resources::LoadFromMem(const ResDesc& desc, std::unique_ptr<std::byte[]> data)
{
    // this is so dumb, but i dont care
    constexpr unsigned int meta_size = sizeof(AnimSeqFrame::m_Idle) + sizeof(AnimSeqFrame::m_Run) + sizeof(AnimSeqFrame::m_Attack) + sizeof(AnimSeqFrame::m_Die);

    std::shared_ptr<AnimSeqFrame> asf = std::make_shared<AnimSeqFrame>();
    std::memcpy(asf.get(), data.get(), meta_size);

    SDL_IOStream* stream = SDL_IOFromConstMem(data.get() + meta_size, desc.m_nSize - meta_size);
    asf->m_Frames.reset(IMG_LoadTexture_IO(app::Renderer::GetInstance().GetSDLRenderer(), stream, true),
        [](SDL_Texture* texture) { SDL_DestroyTexture(texture); });

    return asf;
}
