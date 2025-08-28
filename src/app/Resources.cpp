#include "Resources.hpp"
#include "Renderer.hpp"
#include "app/Component/Render/Texture.hpp"
#include "app/resources/AnimSeqFrames.hpp"
#include "app/resources/Mox.hpp"

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
std::shared_ptr<app::comp::Texture> Resources::LoadFromMem<app::comp::Texture>(const ResDesc& desc, std::unique_ptr<std::byte[]> data)
{
    SDL_IOStream* stream = SDL_IOFromConstMem(data.get(), desc.m_nSize);
    SDL_Surface* surface = IMG_Load_IO(stream, true);
    std::shared_ptr<SDL_Texture> texture(SDL_CreateTextureFromSurface(app::Renderer::GetInstance().GetSDLRenderer(), surface),
        [](SDL_Texture* texture) { SDL_DestroyTexture(texture); });
    const int w = surface->w, h = surface->h;
    SDL_DestroySurface(surface);

    return std::make_shared<app::comp::Texture>(std::move(texture),
        std::make_optional<SDL_FRect>({ 0.f, 0.f, float(w), float(h) }));
}

template <>
std::shared_ptr<toml::table> Resources::LoadFromMem<toml::table>(const ResDesc& desc, std::unique_ptr<std::byte[]> data)
{
    // NOTE: just support ascii code
    return std::make_shared<toml::table>(toml::parse({ reinterpret_cast<const char*>(data.get()), desc.m_nSize }));
}

template <>
std::shared_ptr<AnimSeqFrames> Resources::LoadFromMem(const ResDesc& desc, std::unique_ptr<std::byte[]> data)
{
    constexpr unsigned int meta_size = sizeof(AnimSeqFrames::AnimInfo) * AnimSeqFrames::ANIM_NUM;

    std::shared_ptr<AnimSeqFrames> asf = std::make_shared<AnimSeqFrames>();
    std::memcpy((void*)asf.get(), data.get(), meta_size);

    SDL_IOStream* stream = SDL_IOFromConstMem(data.get() + meta_size, desc.m_nSize - meta_size);
    asf->m_Frames.reset(IMG_LoadTexture_IO(app::Renderer::GetInstance().GetSDLRenderer(), stream, true),
        [](SDL_Texture* texture) { SDL_DestroyTexture(texture); });

    return asf;
}

template <>
std::shared_ptr<TTF_Font> Resources::LoadFromMem(const ResDesc& desc, std::unique_ptr<std::byte[]> data)
{
    // the stream must remain open, and the data must be valid before the font is closed
    SDL_IOStream* stream = SDL_IOFromConstMem(data.get(), desc.m_nSize);
    return std::shared_ptr<TTF_Font>(TTF_OpenFontIO(stream, true, **Settings::GetInstance().GetSettings().at_path("Text.font_size").as_floating_point()), [datas = std::move(data)](TTF_Font* font) { TTF_CloseFont(font); });
}

template <>
std::shared_ptr<Mox> Resources::LoadFromMem(const ResDesc& desc, std::unique_ptr<std::byte[]> data)
{
    std::shared_ptr<Mox> mox = std::make_shared<Mox>();
    std::memcpy((void*)mox.get(), data.get(), sizeof(Mox::Header));

    SDL_IOStream* stream = SDL_IOFromConstMem(data.get() + sizeof(Mox::Header), desc.m_nSize - sizeof(Mox::Header));
    mox->m_Texture.reset(IMG_LoadTexture_IO(app::Renderer::GetInstance().GetSDLRenderer(), stream, true), [](SDL_Texture* texture) { SDL_DestroyTexture(texture); });

    return mox;
}
