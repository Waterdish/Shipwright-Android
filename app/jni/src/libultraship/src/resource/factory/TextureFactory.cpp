#include "resource/factory/TextureFactory.h"
#include "resource/type/Texture.h"
#include "spdlog/spdlog.h"

namespace LUS {

std::shared_ptr<IResource> TextureFactory::ReadResource(std::shared_ptr<ResourceInitData> initData,
                                                        std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<Texture>(initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion) {
        case 0:
            factory = std::make_shared<TextureFactoryV0>();
            break;
        case 1:
            factory = std::make_shared<TextureFactoryV1>();
            break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load Texture with version {}", resource->GetInitData()->ResourceVersion);
        return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void TextureFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<IResource> resource) {
    std::shared_ptr<Texture> texture = std::static_pointer_cast<Texture>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, texture);

    texture->Type = (TextureType)reader->ReadUInt32();
    texture->Width = reader->ReadUInt32();
    texture->Height = reader->ReadUInt32();

    uint32_t dataSize = reader->ReadUInt32();

    texture->ImageDataSize = dataSize;
    texture->ImageData = new uint8_t[dataSize];

    reader->Read((char*)texture->ImageData, dataSize);
}

void TextureFactoryV1::ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<IResource> resource) {
    std::shared_ptr<Texture> texture = std::static_pointer_cast<Texture>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, texture);

    texture->Type = (TextureType)reader->ReadUInt32();
    texture->Width = reader->ReadUInt32();
    texture->Height = reader->ReadUInt32();
    texture->Flags = reader->ReadUInt32();
    texture->HByteScale = reader->ReadFloat();
    texture->VPixelScale = reader->ReadFloat();

    uint32_t dataSize = reader->ReadUInt32();

    texture->ImageDataSize = dataSize;
    texture->ImageData = new uint8_t[dataSize];

    reader->Read((char*)texture->ImageData, dataSize);
}
} // namespace LUS
