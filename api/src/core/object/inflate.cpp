#include <core.hpp>

char* object::inflate(const std::string& in, uint64_t& out_len) {
    const char* in_c = in.data();
    uint64_t in_len  = in.size();

    // probe is just to extract the pattern
    constexpr uint8_t probe_capacity = 40;
    char              probe[probe_capacity];
    uint64_t          probe_bytes_written  = probe_capacity;
    uint64_t          probe_bytes_consumed = in_len;

    int result = uncompress2_z(
        (Bytef*)probe,
        &probe_bytes_written,
        (Bytef*)in_c,
        &probe_bytes_consumed
    );

    if (result != Z_OK && result != Z_BUF_ERROR)
        return 0;
    
    const char* newline = (const char*)std::memchr(probe, '\n', probe_bytes_written);
    const char* space   = (const char*)std::memchr(probe, ' ', probe_bytes_written);
    if (!space || !newline)
        return 0;
    uint8_t  pattern_len      = (uint8_t)(newline - probe);
    out_len = 0;
    space++;
    while (space != newline) {
        out_len *= 10;
        out_len += (uint64_t)(*space - '0');
        space++;
    }

    out_len += pattern_len;
    out_len++; // \n
    char* out = (char*)malloc(out_len);

    if (!out)
        return 0;

    result = uncompress2_z(
        (Bytef*)out,
        &out_len,
        (Bytef*)in_c,
        &in_len
    );
    
    if (result != Z_OK && result != Z_BUF_ERROR)
        return 0;

    return out;
}