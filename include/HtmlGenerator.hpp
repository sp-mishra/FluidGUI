#pragma once

#ifndef HTMLGENERATOR_HPP
#define HTMLGENERATOR_HPP
#include <string>
#include <map>
#include <graaflib/graph.h>

namespace groklab {
    struct HtmlNode {
        std::string tag;
        std::string content;
        std::map<std::string, std::string> attributes;
    };

    class HtmlGenerator {
    private:
    };
}
#endif //HTMLGENERATOR_HPP
