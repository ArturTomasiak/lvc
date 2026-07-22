#pragma once
#include <filesystem>
#include <cstring>
#include <vector>

void sha256(const char* in, uint64_t in_len, char out[65]);
void insert_pattern(std::string& content, const std::string& type);