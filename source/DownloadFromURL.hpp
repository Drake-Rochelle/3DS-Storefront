#pragma once
#include <3ds.h>
#include <string>
#include <vector>

bool downloadToBuffer(const std::string& url, std::vector<u8>& out);
void ensureDir(FS_Archive archive, const std::string& path);
bool downloadToSD(const std::string& url, const std::string& outPath);
bool downloadURL(const std::string& url, const std::string& dir, const std::string& fileName);