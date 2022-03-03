#ifndef _UTILS_H
#define _UTILS_H

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

// return the root directory
fs::path root_dir(void);

// execute a command
std::string exec(const char* cmd);

#endif
