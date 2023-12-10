#pragma once

#include <string>
#include <vector>

namespace advent {
namespace strings {

std::vector<std::string> split(const std::string& s, char delim, bool trim = false);

std::string& trim(std::string& s);

} // namespce string
} // namesapce advent