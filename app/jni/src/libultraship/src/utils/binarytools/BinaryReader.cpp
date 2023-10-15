#include "BinaryReader.h"
#include "MemoryStream.h"
#include <cmath>
#include <stdexcept>

LUS::BinaryReader::BinaryReader(char* nBuffer, size_t nBufferSize) {
    mStream = std::make_shared<MemoryStream>(nBuffer, nBufferSize);
}

LUS::BinaryReader::BinaryReader(Stream* nStream) {
    mStream.reset(nStream);
}

LUS::BinaryReader::BinaryReader(std::shared_ptr<Stream> nStream) {
    mStream = nStream;
}

void LUS::BinaryReader::Close() {
    mStream->Close();
}

void LUS::BinaryReader::SetEndianness(Endianness endianness) {
    this->mEndianness = endianness;
}

LUS::Endianness LUS::BinaryReader::GetEndianness() const {
    return mEndianness;
}

void LUS::BinaryReader::Seek(int32_t offset, SeekOffsetType seekType) {
    mStream->Seek(offset, seekType);
}

uint32_t LUS::BinaryReader::GetBaseAddress() {
    return mStream->GetBaseAddress();
}

void LUS::BinaryReader::Read(int32_t length) {
    mStream->Read(length);
}

void LUS::BinaryReader::Read(char* buffer, int32_t length) {
    mStream->Read(buffer, length);
}

char LUS::BinaryReader::ReadChar() {
    return (char)mStream->ReadByte();
}

int8_t LUS::BinaryReader::ReadInt8() {
    return mStream->ReadByte();
}

int16_t LUS::BinaryReader::ReadInt16() {
    int16_t result = 0;
    mStream->Read((char*)&result, sizeof(int16_t));
    if (mEndianness != Endianness::Native) {
        result = BSWAP16(result);
    }

    return result;
}

int32_t LUS::BinaryReader::ReadInt32() {
    int32_t result = 0;

    mStream->Read((char*)&result, sizeof(int32_t));

    if (mEndianness != Endianness::Native) {
        result = BSWAP32(result);
    }

    return result;
}

uint8_t LUS::BinaryReader::ReadUByte() {
    return (uint8_t)mStream->ReadByte();
}

uint16_t LUS::BinaryReader::ReadUInt16() {
    uint16_t result = 0;

    mStream->Read((char*)&result, sizeof(uint16_t));

    if (mEndianness != Endianness::Native) {
        result = BSWAP16(result);
    }

    return result;
}

uint32_t LUS::BinaryReader::ReadUInt32() {
    uint32_t result = 0;

    mStream->Read((char*)&result, sizeof(uint32_t));

    if (mEndianness != Endianness::Native) {
        result = BSWAP32(result);
    }

    return result;
}

uint64_t LUS::BinaryReader::ReadUInt64() {
    uint64_t result = 0;

    mStream->Read((char*)&result, sizeof(uint64_t));

    if (mEndianness != Endianness::Native) {
        result = BSWAP64(result);
    }

    return result;
}

float LUS::BinaryReader::ReadFloat() {
    float result = NAN;

    mStream->Read((char*)&result, sizeof(float));

    if (mEndianness != Endianness::Native) {
        float tmp;
        char* dst = (char*)&tmp;
        char* src = (char*)&result;
        dst[3] = src[0];
        dst[2] = src[1];
        dst[1] = src[2];
        dst[0] = src[3];
        result = tmp;
    }

    if (std::isnan(result)) {
        throw std::runtime_error("BinaryReader::ReadFloat(): Error reading stream");
    }

    return result;
}

double LUS::BinaryReader::ReadDouble() {
    double result = NAN;

    mStream->Read((char*)&result, sizeof(double));

    if (mEndianness != Endianness::Native) {
        double tmp;
        char* dst = (char*)&tmp;
        char* src = (char*)&result;
        dst[7] = src[0];
        dst[6] = src[1];
        dst[5] = src[2];
        dst[4] = src[3];
        dst[3] = src[4];
        dst[2] = src[5];
        dst[1] = src[6];
        dst[0] = src[7];
        result = tmp;
    }

    if (std::isnan(result)) {
        throw std::runtime_error("BinaryReader::ReadDouble(): Error reading stream");
    }

    return result;
}

ZAPDUtils::Vec3f LUS::BinaryReader::ReadVec3f() {
    return ZAPDUtils::Vec3f();
}

ZAPDUtils::Vec3s LUS::BinaryReader::ReadVec3s() {
    return ZAPDUtils::Vec3s(0, 0, 0);
}

ZAPDUtils::Vec3s LUS::BinaryReader::ReadVec3b() {
    return ZAPDUtils::Vec3s(0, 0, 0);
}

ZAPDUtils::Vec2f LUS::BinaryReader::ReadVec2f() {
    return ZAPDUtils::Vec2f();
}

Color3b LUS::BinaryReader::ReadColor3b() {
    return Color3b();
}

std::string LUS::BinaryReader::ReadString() {
    std::string res;
    int numChars = ReadInt32();
    for (int i = 0; i < numChars; i++) {
        res += ReadChar();
    }
    return res;
}

std::string LUS::BinaryReader::ReadCString() {
    std::string res;

    unsigned char c = 0;
    do {
        if (mStream->GetBaseAddress() >= mStream->GetLength()) {
            break;
        }

        c = ReadChar();
        res += c;
    } while (c != '\0');

    return res;
}

std::vector<char> LUS::BinaryReader::ToVector() {
    return mStream->ToVector();
}
