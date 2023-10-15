#include "ResourceLoader.h"
#include "ResourceFactory.h"
#include "ResourceManager.h"
#include "Resource.h"
#include "File.h"
#include "Context.h"
#include "utils/binarytools/MemoryStream.h"
#include "utils/binarytools/BinaryReader.h"
#include "factory/TextureFactory.h"
#include "factory/VertexFactory.h"
#include "factory/ArrayFactory.h"
#include "factory/BlobFactory.h"
#include "factory/DisplayListFactory.h"
#include "factory/MatrixFactory.h"

namespace LUS {
ResourceLoader::ResourceLoader() {
    RegisterGlobalResourceFactories();
}

ResourceLoader::~ResourceLoader() {
    SPDLOG_TRACE("destruct resource loader");
}

void ResourceLoader::RegisterGlobalResourceFactories() {
    RegisterResourceFactory(ResourceType::Texture, "Texture", std::make_shared<TextureFactory>());
    RegisterResourceFactory(ResourceType::Vertex, "Vertex", std::make_shared<VertexFactory>());
    RegisterResourceFactory(ResourceType::DisplayList, "DisplayList", std::make_shared<DisplayListFactory>());
    RegisterResourceFactory(ResourceType::Matrix, "Matrix", std::make_shared<MatrixFactory>());
    RegisterResourceFactory(ResourceType::Array, "Array", std::make_shared<ArrayFactory>());
    RegisterResourceFactory(ResourceType::Blob, "Blob", std::make_shared<BlobFactory>());
}

bool ResourceLoader::RegisterResourceFactory(ResourceType resourceType, std::string resourceTypeXML,
                                             std::shared_ptr<ResourceFactory> factory) {
    if (mFactories.contains(resourceType)) {
        return false;
    }

    mFactories[resourceType] = factory;
    mFactoriesStr[resourceTypeXML] = factory;
    mFactoriesTypes[resourceTypeXML] = resourceType;
    return true;
}

std::shared_ptr<IResource> ResourceLoader::LoadResource(std::shared_ptr<File> fileToLoad) {
    std::shared_ptr<IResource> result = nullptr;

    if (fileToLoad != nullptr) {
        auto stream = std::make_shared<MemoryStream>(fileToLoad->Buffer.data(), fileToLoad->Buffer.size());
        auto reader = std::make_shared<BinaryReader>(stream);

        // Determine if file is binary or XML...
        uint8_t firstByte = reader->ReadInt8();

        auto resourceInitData = std::make_shared<ResourceInitData>();
        resourceInitData->Path = fileToLoad->Path;
        resourceInitData->Id = 0xDEADBEEFDEADBEEF;
        resourceInitData->Type = ResourceType::None;
        resourceInitData->ResourceVersion = -1;
        resourceInitData->IsCustom = false;
        resourceInitData->ByteOrder = Endianness::Native;

        // If first byte is '<' then we are loading XML, else we are loading OTR binary.
        if (firstByte == '<') {
            // XML
            resourceInitData->IsCustom = true;
            reader->Seek(-1, SeekOffsetType::Current);

            std::string xmlStr = reader->ReadCString();

            tinyxml2::XMLDocument doc;
            tinyxml2::XMLError eResult = doc.Parse(xmlStr.data());

            // OTRTODO: Error checking

            auto root = doc.FirstChildElement();

            std::string nodeName = root->Name();
            resourceInitData->ResourceVersion = root->IntAttribute("Version");

            auto factory = mFactoriesStr[nodeName];
            resourceInitData->Type = mFactoriesTypes[nodeName];

            if (factory != nullptr) {
                result = factory->ReadResourceXML(resourceInitData, root);
            }
        } else {
            // OTR HEADER BEGIN
            // Byte Order
            resourceInitData->ByteOrder = (Endianness)firstByte;
            reader->SetEndianness(resourceInitData->ByteOrder);
            // Is this asset custom?
            resourceInitData->IsCustom = (bool)reader->ReadInt8();
            // Unused two bytes
            for (int i = 0; i < 2; i++) {
                reader->ReadInt8();
            }
            // The type of the resource
            resourceInitData->Type = (ResourceType)reader->ReadUInt32();
            // Resource version
            resourceInitData->ResourceVersion = reader->ReadUInt32();
            // Unique asset ID
            resourceInitData->Id = reader->ReadUInt64();
            // ????
            reader->ReadUInt32();
            // ROM CRC
            reader->ReadUInt64();
            // ROM Enum
            reader->ReadUInt32();
            // Reserved for future file format versions...
            reader->Seek(64, SeekOffsetType::Start);
            // OTR HEADER END

            auto factory = mFactories[resourceInitData->Type];

            if (factory != nullptr) {
                result = factory->ReadResource(resourceInitData, reader);
            }
        }

        if (result == nullptr) {
            SPDLOG_ERROR("Failed to load resource of type {} \"{}\"", (uint32_t)resourceInitData->Type,
                         resourceInitData->Path);
        }
    }

    return result;
}
} // namespace LUS
