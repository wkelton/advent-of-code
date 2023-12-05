#include "strings.h"

#include <sstream>

namespace advent {
namespace strings {

std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string elem;
    while(std::getline(ss, elem, delim)) {
        if (elem.size() > 1 || elem[0] != delim) {
            elems.push_back(elem);
        }
    }
    return elems;
}

} // namespce string
} // namesapce advent