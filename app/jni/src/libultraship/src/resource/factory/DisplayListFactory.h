#pragma once

#include "resource/Resource.h"
#include "resource/ResourceFactory.h"

namespace LUS {
class DisplayListFactory : public ResourceFactory {
  public:
    std::shared_ptr<IResource> ReadResource(std::shared_ptr<ResourceInitData> initData,
                                            std::shared_ptr<BinaryReader> reader) override;
    std::shared_ptr<IResource> ReadResourceXML(std::shared_ptr<ResourceInitData> initData,
                                               tinyxml2::XMLElement* reader) override;
};

class DisplayListFactoryV0 : public ResourceVersionFactory {
  public:
    void ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<IResource> resource) override;
    void ParseFileXML(tinyxml2::XMLElement* reader, std::shared_ptr<IResource> resource) override;

    uint32_t GetCombineLERPValue(std::string valStr);
};
} // namespace LUS
