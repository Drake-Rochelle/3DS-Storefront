#include <3ds.h>
#include <string>
#include <vector>
std::vector<std::string> split(const std::string& s, char delim) 
{
    std::vector<std::string> out;
    std::string current;

    for (char c : s) {
        if (c == delim) {
            out.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    out.push_back(current);
    return out;
}