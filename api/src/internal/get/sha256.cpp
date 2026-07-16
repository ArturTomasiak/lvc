#include <internal.hpp>

#define RIGHT_ROTATE(x, n) (((uint32_t)(x) >> (n)) | ((uint32_t)(x) << (32U - (n))))

static constexpr uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static void process_chunk(const uint8_t* chunk, uint32_t& h0, uint32_t& h1, uint32_t& h2, uint32_t& h3, uint32_t& h4, uint32_t& h5, uint32_t& h6, uint32_t& h7) {
    uint32_t w[64];
    for (uint8_t i = 0; i < 16; i++) {
        uint8_t offset = i * 4;
        w[i] =
            ((uint32_t)chunk[offset]     << 24) |
            ((uint32_t)chunk[offset + 1] << 16) |
            ((uint32_t)chunk[offset + 2] << 8)  |
            ((uint32_t)chunk[offset + 3]);
    }
    for (uint8_t i = 16; i < 64; i++) {
        uint32_t s0 =
            RIGHT_ROTATE(w[i - 15], 7) ^
            RIGHT_ROTATE(w[i - 15], 18) ^
            (w[i - 15] >> 3);

        uint32_t s1 =
            RIGHT_ROTATE(w[i - 2], 17) ^
            RIGHT_ROTATE(w[i - 2], 19) ^
            (w[i - 2] >> 10);

        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

    uint32_t a = h0;
    uint32_t b = h1;
    uint32_t c = h2;
    uint32_t d = h3;
    uint32_t e = h4;
    uint32_t f = h5;
    uint32_t g = h6;
    uint32_t h = h7;

    for (uint8_t i = 0; i < 64; i++) {
        const uint32_t S1    = RIGHT_ROTATE(e, 6) ^ RIGHT_ROTATE(e, 11) ^ RIGHT_ROTATE(e, 25);
        const uint32_t ch    = g ^ (e & (f ^ g));
        const uint32_t temp1 = h + S1 + ch + k[i] + w[i];
        const uint32_t S0    = RIGHT_ROTATE(a, 2) ^ RIGHT_ROTATE(a, 13) ^ RIGHT_ROTATE(a, 22);
        const uint32_t maj   = (a & b) ^ (c & (a ^ b));
        const uint32_t temp2 = S0 + maj;
        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    h0 += a;
    h1 += b;
    h2 += c;
    h3 += d;
    h4 += e;
    h5 += f;
    h6 += g;
    h7 += h;
}

bool get::sha256(const char* in, uint64_t in_len, char out[65]) {
    if (!in || !out)
        return false;

    uint64_t bit_len   = in_len << 3;
    uint64_t remainder = in_len % 64;
    uint8_t padding[128] = {0};
    memcpy((void*)padding, in + in_len - remainder, remainder);
    padding[remainder] = 0x80;
    
    uint64_t in_chunks      = in_len >> 6;
    uint64_t padding_chunks = remainder >= 56 ? 2 : 1;

    for (int i = 0; i < 8; ++i)
        padding[((64 * padding_chunks) - 8) + i] = bit_len >> (56 - i * 8);

    uint32_t h0 = 0x6a09e667;
    uint32_t h1 = 0xbb67ae85;
    uint32_t h2 = 0x3c6ef372;
    uint32_t h3 = 0xa54ff53a;
    uint32_t h4 = 0x510e527f;
    uint32_t h5 = 0x9b05688c;
    uint32_t h6 = 0x1f83d9ab;
    uint32_t h7 = 0x5be0cd19;
        
    for (uint64_t i = 0; i < in_chunks; i++)
        process_chunk((const uint8_t*)in + (i * 64), h0, h1, h2, h3, h4, h5, h6, h7);
    for (uint64_t i = 0; i < padding_chunks; i++)
        process_chunk(padding + (i * 64), h0, h1, h2, h3, h4, h5, h6, h7);

    static constexpr char hex[] = "0123456789abcdef";
    const uint32_t state[8] = {h0, h1, h2, h3, h4, h5, h6, h7};

    size_t output_index = 0;

    for (uint32_t word : state)
        for (int shift = 28; shift >= 0; shift -= 4)
            out[output_index++] = hex[(word >> shift) & 0x0f];

    out[64] = '\0';

    return true;
}