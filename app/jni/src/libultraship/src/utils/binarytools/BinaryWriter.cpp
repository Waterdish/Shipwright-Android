#include "BinaryWriter.h"
#include "MemoryStream.h"

LUS::BinaryWriter::BinaryWriter() {
    mStream = std::make_shared<MemoryStream>();
}

LUS::BinaryWriter::BinaryWriter(Stream* nStream) {
    mStream.reset(nStream);
}

LUS::BinaryWriter::BinaryWriter(std::shared_ptr<Stream> nStream) {
    mStream = nStream;
}

void LUS::BinaryWriter::SetEndianness(Endianness endianness) {
    this->mEndianness = endianness;
}

void LUS::BinaryWriter::Close() {
    mStream->Close();
}

std::shared_ptr<LUS::Stream> LUS::BinaryWriter::GetStream() {
    return mStream;
}

uint64_t LUS::BinaryWriter::GetBaseAddress() {
    return mStream->GetBaseAddress();
}

uint64_t LUS::BinaryWriter::GetLength() {
    return mStream->GetLength();
}

void LUS::BinaryWriter::Seek(int32_t offset, SeekOffsetType seekType) {
    mStream->Seek(offset, seekType);
}

void LUS::BinaryWriter::Write(int8_t value) {
    mStream->Write((char*)&value, sizeof(int8_t));
}

void LUS::BinaryWriter::Write(uint8_t value) {
    mStream->Write((char*)&value, sizeof(uint8_t));
}

void LUS::BinaryWriter::Write(int16_t value) {
    if (mEndianness != Endianness::Native) {
        value = BSWAP16(value);
    }

    mStream->Write((char*)&value, sizeof(int16_t));
}

void LUS::BinaryWriter::Write(uint16_t value) {
    if (mEndianness != Endianness::Native) {
        value = BSWAP16(value);
    }

    mStream->Write((char*)&value, sizeof(uint16_t));
}

void LUS::BinaryWriter::Write(int32_t value) {
    if (mEndianness != Endianness::Native) {
        value = BSWAP32(value);
    }

    mStream->Write((char*)&value, sizeof(int32_t));
}

void LUS::BinaryWriter::Write(int32_t valueA, int32_t valueB) {
    Write(valueA);
    Write(valueB);
}

void LUS::BinaryWriter::Write(uint32_t value) {
    if (mEndianness != Endianness::Native) {
        value = BSWAP32(value);
    }

    mStream->Write((char*)&value, sizeof(uint32_t));
}

void LUS::BinaryWriter::Write(int64_t value) {
    if (mEndianness != Endianness::Native) {
        value = BSWAP64(value);
    }

    mStream->Write((char*)&value, sizeof(int64_t));
}

void LUS::BinaryWriter::Write(uint64_t value) {
    if (mEndianness != Endianness::Native) {
        value = BSWAP64(value);
    }

    mStream->Write((char*)&value, sizeof(uint64_t));
}

void LUS::BinaryWriter::Write(float value) {
    if (mEndianness != Endianness::Native) {
        float tmp;
        char* dst = (char*)&tmp;
        char* src = (char*)&value;
        dst[3] = src[0];
        dst[2] = src[1];
        dst[1] = src[2];
        dst[0] = src[3];
        value = tmp;
    }

    mStream->Write((char*)&value, sizeof(float));
}

void LUS::BinaryWriter::Write(double value) {
    if (mEndianness != Endianness::Native) {
        double tmp;
        char* dst = (char*)&tmp;
        char* src = (char*)&value;
        dst[7] = src[0];
        dst[6] = src[1];
        dst[5] = src[2];
        dst[4] = src[3];
        dst[3] = src[4];
        dst[2] = src[5];
        dst[1] = src[6];
        dst[0] = src[7];
        value = tmp;
    }

    mStream->Write((char*)&value, sizeof(double));
}

void LUS::BinaryWriter::Write(const std::string& str) {
    int strLen = str.size();
    Write(strLen);

    for (char c : str) {
        mStream->WriteByte(c);
    }
}

void LUS::BinaryWriter::Write(char* srcBuffer, size_t length) {
    mStream->Write(srcBuffer, length);
}

std::vector<char> LUS::BinaryWriter::ToVector() {
    return mStream->ToVector();
}
