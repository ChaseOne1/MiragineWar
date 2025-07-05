#pragma once
#include "ResourceDescription.hpp"

namespace app {
class Index
{
private:
    //TODO: change ".meta" to ".toml"
    inline static toml::table ms_IndexDescs = 
        toml::parse_file((AppMetaData::msc_szAssetsPath / "index.meta").generic_u8string());

    std::ifstream m_IndexFile;
    std::unordered_map<GUID_t, ResDesc> m_ResDescs;

public:
    Index(GUID_t index);
    const ResourceDescription& GetResourceDescription(GUID_t resource) const;
};
}
