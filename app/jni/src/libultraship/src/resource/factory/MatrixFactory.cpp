#include "resource/factory/MatrixFactory.h"
#include "resource/type/Matrix.h"
#include "spdlog/spdlog.h"

namespace LUS {
std::shared_ptr<IResource> MatrixFactory::ReadResource(std::shared_ptr<ResourceInitData> initData,
                                                       std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<Matrix>(initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion) {
        case 0:
            factory = std::make_shared<MatrixFactoryV0>();
            break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load Matrix with version {}", resource->GetInitData()->ResourceVersion);
        return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void MatrixFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<IResource> resource) {
    std::shared_ptr<Matrix> mtx = static_pointer_cast<Matrix>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, resource);

    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            mtx->Matrx.m[i][j] = reader->ReadInt32();
        }
    }
}
} // namespace LUS
