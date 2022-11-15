#define WIN32_LEAN_AND_MEAN
// Uncomment to get console with data from DLL
#define VERBOSE
#include <string>

/**
* Returns directory from full path
* 
* @param full_path Full path (i.e C:\folder\test.exe)
* @return Directory of (i.e C:\folder)
*/
std::string get_dir(char* full_path);