#pragma once

#ifndef UIDOM_HPP
#define UIDOM_HPP

#include <webview/webview.h>
#include <rfl/json.hpp>
#include <rfl.hpp>
#include <memory>
#include "ScreenUtils.hpp"
#include "Log.hpp"

namespace groklab {
    struct Widget {
        enum class WidgetType {
            Undefined,
            Layout,
            // Text elements
            TextElement,
            Label,
            Paragraph,
            Span,
            Link,
            Markdown,
            Html,
        };

        enum class LayoutType {
            Horizontal,
            Vertical,
        };

        Widget() = default;

        std::string id{};
        graaf::vertex_id_t nodeId{0};
        WidgetType type{WidgetType::Undefined};
        bool visible{true};
        std::map<std::string, std::string> attributes;

        [[nodiscard]] std::string_view getAttribute(const std::string &key) const {
            const auto it = attributes.find(key);
            if (it != attributes.end()) {
                return it->second;
            }
            return "";
        }

        void setAttribute(const std::string &key, const std::string &value) {
            attributes[key] = value;
        }

        [[nodiscard]] bool hasAttribute(const std::string &key) const {
            return attributes.contains(key);
        }
    };

    struct WidgetEdgeProperties {
        bool visible;
    };

    class ParentLayout : public Widget {
    };

    enum class HtmlTag {
        HTML,
        HEAD,
        TITLE,
        BODY,
        H1,
        H2,
        H3,
        H4,
        H5,
        H6,
        P,
        DIV,
        SPAN,
        A,
        IMG,
        UL,
        OL,
        LI,
        TABLE,
        TR,
        TH,
        TD,
        FORM,
        INPUT,
        BUTTON,
        SELECT,
        OPTION,
        TEXTAREA,
        IFRAME,
        CANVAS,
        SVG,
        PATH,
        RECT,
        CIRCLE,
        ELLIPSE,
        LINE,
        POLYGON,
        POLYLINE,
        G,
        SCRIPT,
        STYLE,
        META,
        LINK,
        BR,
        HR,
        COMMENT
    };

    struct HtmlNode {
        HtmlTag htmlTag;
        std::string content;
        std::map<std::string, std::string> attributes;
    };

    class HtmlGenerator {
    public:
        using WidgetGraphType = graaf::directed_graph<Widget, WidgetEdgeProperties>;

        HtmlGenerator() = default;

        virtual ~HtmlGenerator() = default;

        [[nodiscard]] virtual std::string generateHtml(const WidgetGraphType &widgetGraph) const = 0;
    };

    class FluidUI {
        using WidgetGraphType = HtmlGenerator::WidgetGraphType;
        std::unique_ptr<webview::webview> webview_;
        WidgetGraphType widgetGraph_{};
        graaf::vertex_id_t parentVertexId_{};
        graaf::vertex_id_t currentVertexId_{};
        std::unique_ptr<HtmlGenerator> htmlGenerator_;

    public:
        explicit FluidUI(const std::string &title, std::unique_ptr<HtmlGenerator> htmlGenerator)
            : htmlGenerator_(std::move(htmlGenerator)) {
            const ScreenSize screenSize = getScreenSize();
            initialize(title, screenSize.width, screenSize.height);
        }

        explicit FluidUI(const std::string &title, int width, int height, std::unique_ptr<HtmlGenerator> htmlGenerator)
            : htmlGenerator_(std::move(htmlGenerator)) {
            initialize(title, width, height);
        }

        void generate() const {
            struct Res {
                std::string method;
                std::string id;
            };
            if (htmlGenerator_ == nullptr) {
                critical("HtmlGenerator is not initialized");
                return;
            }
            std::string html = htmlGenerator_->generateHtml(widgetGraph_);
            webview_->bind("count",
                [&](const std::string &req) -> std::string {
                info("Request from:  {}", req);
                    const Res res = {req, "123"};
                    const std::string result = rfl::json::write(res);
                return result;
            });
            webview_->set_html(html);
        }

        void run() const {
            if (webview_ == nullptr) {
                critical("FluidUI is not initialized");
                return;
            }
            webview_->run();
        }

    private:
        void initialize(const std::string &title, int width, int height) {
            try {
                webview_ = std::make_unique<webview::webview>(true, nullptr);
                webview_->set_title(title);
                webview_->set_size(width, height, WEBVIEW_HINT_NONE);
                parentVertexId_ = widgetGraph_.add_vertex(ParentLayout{});
            } catch (const webview::exception &e) {
                critical("Failed to initialize FluidUI with error {}", e.what());
                throw;
            }
        }
    };
}

#endif //UIDOM_HPP
