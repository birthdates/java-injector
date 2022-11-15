#include <string>

std::string get_dir(char* full_path) {
    std::string path = full_path;
    return path.substr(0, path.find_last_of("\\"));
}