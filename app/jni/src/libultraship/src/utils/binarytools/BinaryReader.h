#pragma once

#include <string>
#include <memory>
#include <vector>
#include "endianness.h"
#include "Vec2f.h"
#include "Vec3f.h"
#include "Vec3s.h"
#include "Color3b.h"
#include "Stream.h"

class BinaryReader;

namespace LUS {

class BinaryReader {
  public:
    BinaryReader(char* nBuffer, size_t nBufferSize);
    BinaryReader(Stream* nStream);
    BinaryReader(std::shared_ptr<Stream> nStream);

    void Close();

    void SetEndianness(Endianness endianness);
    Endianness GetEndianness() const;

    void Seek(int32_t offset, SeekOffsetType seekType);
    uint32_t GetBaseAddress();

    void Read(int32_t length);
    void Read(char* buffer, int32_t length);
    char ReadChar();
    int8_t ReadInt8();
    int16_t ReadInt16();
    int32_t ReadInt32();
    uint8_t ReadUByte();
    uint16_t ReadUInt16();
    uint32_t ReadUInt32();
    uint64_t ReadUInt64();
    float ReadFloat();
    double ReadDouble();
    ZAPDUtils::Vec3f ReadVec3f();
    ZAPDUtils::Vec3s ReadVec3s();
    ZAPDUtils::Vec3s ReadVec3b();
    ZAPDUtils::Vec2f ReadVec2f();
    Color3b ReadColor3b();
    std::string ReadString();
    std::string ReadCString();

    std::vector<char> ToVector();

  protected:
    std::shared_ptr<Stream> mStream;
    Endianness mEndianness = Endianness::Native;
};
} // namespace LUS
