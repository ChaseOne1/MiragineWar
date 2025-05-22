#pragma once
#include "app/resources/Index.hpp"
#include "app/resources/Pak.hpp"
#include "utility/LRU.hpp"

#define RESOURCE_CACHE_NUM 100u
#define INDEX_CACHE_NUM 10u
#define PAK_CACHE_NUM 5u

namespace app {

class Resources final : public utility::Singleton<Resources>
{
    friend class utility::Singleton<Resources>;

    template <typename T>
    struct LoadFromMemImpl;

private:
    utility::LRUContainer<GUID_t, std::shared_ptr<void>, GUIDHash> m_Resources { RESOURCE_CACHE_NUM };
    utility::LRUContainer<GUID_t, Index, GUIDHash> m_Indexes { INDEX_CACHE_NUM };
    utility::LRUContainer<GUID_t, Pak, GUIDHash> m_Paks { PAK_CACHE_NUM };

private:
    Resources() = default;
    ~Resources() = default;

    // TODO: map file instade of stream
    template <typename T>
    std::shared_ptr<T> LoadFromMem(const ResDesc& desc, std::unique_ptr<std::byte[]> data)
    {
        return LoadFromMemImpl<T>::Load(desc, std::move(data));
    }

public:
    template <typename T>
    std::shared_ptr<T> Require(GUID_t resource, GUID_t index)
    {
        // if the res is in cache, just return
        if (auto res = m_Resources.Get(resource)) return std::static_pointer_cast<T>(*res);

        // if not, load from the pak
        try {
            // but firstly, find out which pak we need
            auto idx = m_Indexes.Get(index);
            if (!idx) idx = m_Indexes.Put(index, Index(index));
            const ResDesc& desc = idx->GetResourceDescription(resource);

            // finally our dear pak
            auto pak = m_Paks.Get(desc.m_FromPak);
            if (!pak) pak = m_Paks.Put(desc.m_FromPak, Pak(desc.m_FromPak));

            // WARNING: we dont know if a image is surface or texture anymore
            m_Resources.Put(resource, LoadFromMem<T>(desc, pak->Load(desc)));
        } catch (std::invalid_argument e) {
            SDL_Log("%s\n", e.what());
        }

        // give them what they want, maybe someting unexpected :D
        return std::static_pointer_cast<T>(*m_Resources.Get(resource));
    }
};

template <>
struct Resources::LoadFromMemImpl<SDL_Surface>
{
    static std::shared_ptr<SDL_Surface> Load(const ResDesc& desc, std::unique_ptr<std::byte[]> data)
    {
        SDL_IOStream* stream = SDL_IOFromConstMem(data.get(), desc.m_nSize);
        return std::shared_ptr<SDL_Surface>(IMG_Load_IO(stream, true),
            [](SDL_Surface* surface) { SDL_DestroySurface(surface); });
    }
};

template <>
struct Resources::LoadFromMemImpl<SDL_Texture>
{
    static std::shared_ptr<SDL_Texture> Load(const ResDesc& desc, std::unique_ptr<std::byte[]> data)
    {
        SDL_IOStream* stream = SDL_IOFromConstMem(data.get(), desc.m_nSize);
        return std::shared_ptr<SDL_Texture>(IMG_LoadTexture_IO(app::Renderer::GetInstance().GetRenderer(), stream, true),
            [](SDL_Texture* texture) { SDL_DestroyTexture(texture); });
    }
};

template <>
struct Resources::LoadFromMemImpl<toml::table>
{
    static std::shared_ptr<toml::table> Load(const ResDesc& desc, std::unique_ptr<std::byte[]> data)
    {
        //TODO: no tested
        //NOTE: just support ascii code
        return std::make_shared<toml::table>(toml::parse({ reinterpret_cast<const char*>(data.get()), desc.m_nSize }));
    }
};
}
