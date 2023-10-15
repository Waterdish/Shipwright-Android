#include "ResourceFactory.h"

namespace LUS {
void ResourceVersionFactory::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                             std::shared_ptr<IResource> resource) {
}

void ResourceVersionFactory::ParseFileXML(tinyxml2::XMLElement* reader, std::shared_ptr<IResource> resource) {
}

void ResourceVersionFactory::WriteFileBinary(std::shared_ptr<BinaryWriter> writer,
                                             std::shared_ptr<IResource> resource) {
}

void ResourceVersionFactory::WriteFileXML(tinyxml2::XMLElement* writer, std::shared_ptr<IResource> resource) {
}

std::shared_ptr<IResource> ResourceFactory::ReadResourceXML(std::shared_ptr<ResourceInitData> initData,
                                                            tinyxml2::XMLElement* reader) {
    return nullptr;
}
} // namespace LUS
