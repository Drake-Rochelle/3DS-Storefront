#pragma once
#include <3ds.h>
#include <stdio.h>
#include <string>
std::string readFile(const std::string& path);
bool writeFile(const std::string& path, const std::string& data);
