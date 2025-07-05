#include "Index.hpp"

using namespace app;

Index::Index(GUID_t index)
    : m_IndexFile(AppMetaData::msc_szAssetsPath / ms_IndexDescs[std::to_string(index)].value_or<std::string>(""),
          std::ios::in | std::ios::binary)
{
    if (!m_IndexFile) throw std::invalid_argument("Invalid index: " + std::to_string(index));

    ResDesc desc {};
    do {
        m_IndexFile.read(reinterpret_cast<char*>(&desc), sizeof desc);
    } while (m_IndexFile && (m_ResDescs[desc.m_Guid] = desc, true));
}

const ResourceDescription& Index::GetResourceDescription(GUID_t resource) const
{
    return m_ResDescs.at(resource);
}
