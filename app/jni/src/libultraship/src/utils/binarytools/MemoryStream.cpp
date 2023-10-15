#include "MemoryStream.h"
#include <cstring>

#ifndef _MSC_VER
#define memcpy_s(dest, destSize, source, sourceSize) memcpy(dest, source, destSize)
#endif

LUS::MemoryStream::MemoryStream() {
    mBuffer = std::vector<char>();
    // mBuffer.reserve(1024 * 16);
    mBufferSize = 0;
    mBaseAddress = 0;
}

LUS::MemoryStream::MemoryStream(char* nBuffer, size_t nBufferSize) : MemoryStream() {
    mBuffer = std::vector<char>(nBuffer, nBuffer + nBufferSize);
    mBufferSize = nBufferSize;
    mBaseAddress = 0;
}

LUS::MemoryStream::~MemoryStream() {
}

uint64_t LUS::MemoryStream::GetLength() {
    return mBuffer.size();
}

void LUS::MemoryStream::Seek(int32_t offset, SeekOffsetType seekType) {
    if (seekType == SeekOffsetType::Start) {
        mBaseAddress = offset;
    } else if (seekType == SeekOffsetType::Current) {
        mBaseAddress += offset;
    } else if (seekType == SeekOffsetType::End) {
        mBaseAddress = mBufferSize - 1 - offset;
    }
}

std::unique_ptr<char[]> LUS::MemoryStream::Read(size_t length) {
    std::unique_ptr<char[]> result = std::make_unique<char[]>(length);

    memcpy_s(result.get(), length, &mBuffer[mBaseAddress], length);
    mBaseAddress += length;

    return result;
}

void LUS::MemoryStream::Read(const char* dest, size_t length) {
    memcpy_s((void*)dest, length, &mBuffer[mBaseAddress], length);
    mBaseAddress += length;
}

int8_t LUS::MemoryStream::ReadByte() {
    return mBuffer[mBaseAddress++];
}

void LUS::MemoryStream::Write(char* srcBuffer, size_t length) {
    if (mBaseAddress + length >= mBuffer.size()) {
        mBuffer.resize(mBaseAddress + length);
        mBufferSize += length;
    }

    memcpy_s(&mBuffer[mBaseAddress], length, srcBuffer, length);
    mBaseAddress += length;
}

void LUS::MemoryStream::WriteByte(int8_t value) {
    if (mBaseAddress >= mBuffer.size()) {
        mBuffer.resize(mBaseAddress + 1);
        mBufferSize = mBaseAddress;
    }

    mBuffer[mBaseAddress++] = value;
}

std::vector<char> LUS::MemoryStream::ToVector() {
    return mBuffer;
}

void LUS::MemoryStream::Flush() {
}

void LUS::MemoryStream::Close() {
}
