#pragma once

#ifndef HTMLGENERATOR_HPP
#define HTMLGENERATOR_HPP
#include <string>
#include <graaflib/graph.h>

#include "UIDom.hpp"

namespace groklab {
    class W2UIHtmlGenerator : public HtmlGenerator {
    public:
        W2UIHtmlGenerator() = default;
        ~W2UIHtmlGenerator() override = default;

        [[nodiscard]] std::string generateHtml(const WidgetGraphType &widgetGraph) const override {
            return FileUtils::readFileAsString("./web/vue/index-gen.html");
        }
    };
}
#endif //HTMLGENERATOR_HPP
