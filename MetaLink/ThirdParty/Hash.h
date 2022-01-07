#pragma once


// Use fnv.h, I stole it from osiris for good reason
constexpr unsigned int Hash(const char* str, const unsigned int value = 0x45C3370D)
{
    return *str ? Hash(str + 1, (value ^ *str) * static_cast<unsigned long long>(0x1000193)) : value;
}

__forceinline unsigned int HashRuntime(const char* str)
{
    unsigned int hash = 0x45C3370D;
    while (*str)
    {
        hash ^= *str++;
        hash *= 0x1000193;
    }

    return hash;
}

namespace CRC32
{
    __forceinline void Init(unsigned long* crc) { *crc = 0xFFFFFFFFUL; }
    __forceinline void Final(unsigned long* crc) { *crc ^= 0xFFFFFFFFUL; }
    __forceinline void ProcessBuffer(unsigned long* crc, const void* pBuffer, int nBuffer);

    __forceinline unsigned long GetTableEntry(unsigned int slot);
    __forceinline unsigned long ProcessSingleBuffer(const void* p, int len)
    {
        unsigned long crc;

        Init(&crc);
        ProcessBuffer(&crc, p, len);
        Final(&crc);

        return crc;
    }
}

namespace MD5
{
    struct MD5Context
    {
        unsigned int buf[4];
        unsigned int bits[2];
        unsigned char in[64];
    };

    __forceinline void Init(MD5Context* ctx)
    {
        ctx->buf[0] = 0x67452301;
        ctx->buf[1] = 0xefcdab89;
        ctx->buf[2] = 0x98badcfe;
        ctx->buf[3] = 0x10325476;

        ctx->bits[0] = 0;
        ctx->bits[1] = 0;
    }
    __forceinline void Final(unsigned char digest[16], MD5Context* context);
    __forceinline void Transform(unsigned int buf[4], unsigned int const in[16]);
    __forceinline void Update(MD5Context* ctx, unsigned char const* buf, unsigned int len);

    __forceinline unsigned int PseudoRandom(unsigned int seed);
}