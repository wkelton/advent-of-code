#include "strings.h"

#include <algorithm>
#include <sstream>

namespace advent {
namespace strings {

std::vector<std::string> split(const std::string& s, char delim, bool trim) {
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string elem;
    while(std::getline(ss, elem, delim)) {
        if (elem.size() > 1 || elem[0] != delim) {
            if (trim) {
                elems.push_back(advent::strings::trim(elem));
            } else {
                elems.push_back(elem);
            }
        }
    }
    return elems;
}

std::string& trim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
    return s;
}

} // namespce string
} // namesapce advent