#include "Resources.hpp"
#include "Renderer.hpp"
#include "app/Component/Texture.hpp"
#include "app/ScriptManager.hpp"
#include "app/resources/AnimSeqFrames.hpp"
#include "app/resources/Mox.hpp"

using namespace app;

Resources::Resources()
{
    auto type = app::ScriptManager::GetLuaState().new_usertype<Resources>("Resources", sol::no_constructor);
    type["texture"] = [](std::string_view res, std::string_view idx) {
        return Resources::GetInstance().Require<SDL_Texture>(std::strtoull(res.data(), nullptr, 10), std::strtoull(idx.data(), nullptr, 10));
    };
    type["font"] = [](std::string_view res, std::string_view idx) {
        return std::static_pointer_cast<void>(Resources::GetInstance().Require<TTF_Font>(std::strtoull(res.data(), nullptr, 10), std::strtoull(idx.data(), nullptr, 10)));
    };
    type["asf"] = [](std::string_view res, std::string_view idx) {
        return Resources::GetInstance().Require<AnimSeqFrames>(std::strtoull(res.data(), nullptr, 10), std::strtoull(idx.data(), nullptr, 10));
    };
    type["mox"] = [](std::string_view res, std::string_view idx) {
        return Resources::GetInstance().Require<Mox>(std::strtoull(res.data(), nullptr, 10), std::strtoull(idx.data(), nullptr, 10));
    };
}

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
    // NOTE: the stream must remain open, and the data must be valid before the font is closed,
    // setting closeio to true allows SDL_TTF to manage streams automatically
    SDL_IOStream* stream = SDL_IOFromConstMem(data.release(), desc.m_nSize);
    return std::shared_ptr<TTF_Font>(TTF_OpenFontIO(stream, true, Settings::GetSettings()["Text"]["default_font_size"]), [](TTF_Font* font) { TTF_CloseFont(font); });
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
