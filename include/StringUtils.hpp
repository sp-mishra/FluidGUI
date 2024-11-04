#pragma once


#ifndef STRINGUTILS_HPP
#define STRINGUTILS_HPP
#include <algorithm>
#include <cctype>
#include <string>
#include <iostream>
namespace groklab {
    struct StringUtils {
        static std::string toLowerCase(const std::string &str) {
            std::string result = str;
            std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
            return result;
        }
    };
}
#endif //STRINGUTILS_HPP
