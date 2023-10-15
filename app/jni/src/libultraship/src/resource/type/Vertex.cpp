#include "resource/type/Vertex.h"

namespace LUS {
Vertex::Vertex() : Resource(std::shared_ptr<ResourceInitData>()) {
}

Vtx* Vertex::GetPointer() {
    return VertexList.data();
}

size_t Vertex::GetPointerSize() {
    return VertexList.size() * sizeof(Vtx);
}
} // namespace LUS
