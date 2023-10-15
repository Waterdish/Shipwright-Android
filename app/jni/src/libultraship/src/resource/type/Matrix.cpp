#include "resource/type/Matrix.h"

namespace LUS {
Matrix::Matrix() : Resource(std::shared_ptr<ResourceInitData>()) {
}

Mtx* Matrix::GetPointer() {
    return &Matrx;
}

size_t Matrix::GetPointerSize() {
    return sizeof(Mtx);
}
} // namespace LUS
