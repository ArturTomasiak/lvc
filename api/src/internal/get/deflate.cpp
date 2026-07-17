#include <internal.hpp>

char* get::deflate(const char* in, uint64_t in_len, uint64_t& out_len) {
    if (!in || !in_len)
        return 0;
    out_len = compressBound_z(in_len);
    if (out_len == (uint64_t)-1)
        return 0; //overflow 
    char* out = (char*)malloc(out_len);
    if (!out)
        return 0;
    int32_t result = compress2_z((Bytef*)out, &out_len, (const Bytef*)in, in_len, Z_DEFAULT_COMPRESSION);
    if (result != Z_OK) {
        free(out);
        return 0;
    }
    return out;
}