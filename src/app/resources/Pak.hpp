#pragma once
#include "app/resources/ResourceDescription.hpp"

namespace app {
class Pak
{
private:
    inline static toml::table ms_PakDescs = toml::parse_file((AppMetaData::msc_szAssetsPath / "pak.meta").generic_u8string());

    std::ifstream m_PakFile;

public:
    Pak(GUID_t);

    // template <typename ImgType>
    // ImgType* LoadImage(const ResourceDescription& resource)
    // {
    //     std::vector<std::byte> bin(resource.m_nSize);
    //     m_PakFile.seekg(resource.m_nPakOffset, std::ios::beg);
    //     m_PakFile.read(reinterpret_cast<char*>(bin.data()), resource.m_nSize);
    //     SDL_IOStream* stream = SDL_IOFromConstMem(bin.data(), bin.size()); // TODO: map file

    //     if constexpr (std::is_same_v<ImgType, SDL_Texture>)
    //         return IMG_LoadTexture_IO(app::Renderer::GetInstance().GetRenderer(), stream, true);
    //     else if (std::is_same_v<ImgType, SDL_Surface>)
    //         return IMG_Load_IO(stream, true);
    //     else
    //         return nullptr;
    // }

    std::unique_ptr<std::byte[]> Load(const ResourceDescription& resource);
};
};
