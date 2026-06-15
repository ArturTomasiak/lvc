#pragma once
#include <string>

void create_folder(const char* path);
void create_file(const char* path, const char* content);
void delete_folder(const char* path);
void delete_file(const char* path);
std::string read_file();
std::string lvc_path();