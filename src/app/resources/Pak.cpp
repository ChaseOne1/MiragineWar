#include "Pak.hpp"

using namespace app;
using namespace utility;

Pak::Pak(GUID_t file)
    : m_PakFile(AppMetaData::msc_szAssetsPath / ms_PakDescs[std::to_string(file)].value_or<std::string>(""),
          std::ios::in | std::ios::binary)
{
    if (!m_PakFile)
        throw std::invalid_argument("Invalid Pak:" + ms_PakDescs[std::to_string(file)].value_or<std::string>(""));
}

std::unique_ptr<std::byte[]> Pak::Load(const ResourceDescription& resource)
{
    std::unique_ptr<std::byte[]> data = std::make_unique<std::byte[]>(resource.m_nSize);

    m_PakFile.seekg(resource.m_nPakOffset, std::ios::beg);
    m_PakFile.read(reinterpret_cast<char*>(data.get()), resource.m_nSize);
    m_PakFile.clear();

    return data;
}
// SDL_Surface* Pak::LoadSurface(const ResourceDesc& resource)
//{
//     std::vector<std::byte> bin(resource.m_nSize);
//     m_PakFile.seekg(resource.m_nPakOffset, std::ios::beg);
//     m_PakFile.read(reinterpret_cast<char*>(bin.data()), resource.m_nSize);
//
//     auto deleter = [](SDL_IOStream* stm) { SDL_CloseIO(stm); };
//     std::unique_ptr<SDL_IOStream, decltype(deleter)> stream(SDL_IOFromConstMem(bin.data(), bin.size()), std::move(deleter));
//
//     switch (resource.m_eType) {
//     case app::ResourceType::PNG:
//         return IMG_LoadPNG_IO(stream.get());
//     default:
//         return nullptr;
//     }
//
//     return nullptr; // unreachable
// }
//
// SDL_Texture* Pak::LoadTexture(const ResourceDesc& resource)
//{
//     std::vector<std::byte> bin(resource.m_nSize);
//     m_PakFile.seekg(resource.m_nPakOffset, std::ios::beg);
//     m_PakFile.read(reinterpret_cast<char*>(bin.data()), resource.m_nSize);
//
//     SDL_IOStream* stream = SDL_IOFromConstMem(bin.data(), bin.size());
//
//     switch (resource.m_eType) {
//     case app::ResourceType::PNG:
//     case app::ResourceType::JPG:
//         return IMG_LoadTexture_IO(app::Renderer::GetInstance().GetRenderer(), stream, true);
//     default:
//         return nullptr;
//     }
//
//     return nullptr; // unreachable
// }
