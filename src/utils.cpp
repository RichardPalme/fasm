#include "utils.h"
#include <limits.h> // for PATH_MAX
#include <unistd.h> // for readlink
#include <string>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <array>

// return the root directory
fs::path root_dir() {
  char result[PATH_MAX];
  // write the path to the executable into result
  ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  // convert result to std::string
  auto tmp = std::string(result, (count > 0) ? count : 0);
  return fs::path(tmp).parent_path().parent_path();
}

// execute a command
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

