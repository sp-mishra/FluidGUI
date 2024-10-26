#pragma once

#ifndef HTMLUTILITY_HPP
#define HTMLUTILITY_HPP

#include <lexbor/html/html.h>
#include <lexbor/dom/interfaces/document.h>
#include <lexbor/dom/interfaces/element.h>
#include <lexbor/core/str.h>
#include <unordered_map>
#include <string>
#include <utility>
#include <rfl.hpp>
#include "FileUtils.hpp"
#include "Log.hpp"

namespace groklab {
    enum class JavaScriptEvent {
        click,
        dblclick,
        mousedown,
        mouseup,
        mousemove,
        mouseover,
        mouseout,
        keydown,
        keyup,
        keypress,
        load,
        unload,
        abort,
        error,
        resize,
        scroll,
        select,
        change,
        submit,
        reset,
        focus,
        blur,
        input,
        contextmenu,
        drag,
        dragend,
        dragenter,
        dragleave,
        dragover,
        dragstart,
        drop,
        copy,
        cut,
        paste,
        wheel,
        touchstart,
        touchmove,
        touchend,
        touchcancel
    };

    inline std::string toString(const JavaScriptEvent event) {
        return rfl::enum_to_string(event);
    }

    class HtmlElement {
    protected:
        std::string tagName_;
        std::unordered_map<std::string, std::string> attributes_;
        std::string innerHTML_;

    public:
        virtual ~HtmlElement() = default;

        explicit HtmlElement(std::string  tagName) : tagName_(std::move(tagName)) {}

        void setAttribute(const std::string& name, const std::string& value) {
            attributes_[name] = value;
        }

        void setInnerHTML(const std::string& html) {
            innerHTML_ = html;
        }

        [[nodiscard]] std::string getAttribute(const std::string& name) const {
            auto it = attributes_.find(name);
            if (it != attributes_.end()) {
                return it->second;
            }
            return "";
        }

        [[nodiscard]] virtual std::string render() const {
            std::stringstream ss;
            ss << "<" << tagName_;
            for (const auto& attr : attributes_) {
                ss << " " << attr.first << "=\"" << attr.second << "\"";
            }
            ss << ">" << innerHTML_ << "</" << tagName_ << ">";
            return ss.str();
        }
    };

    class HtmlUtility {
        lxb_status_t status_;
        lxb_html_parser_t *parser_;
        lxb_html_document_t *document_;

    public:
        explicit HtmlUtility(const std::string& filePath) {
            /* Initialization */
            parser_ = lxb_html_parser_create();
            status_ = lxb_html_parser_init(parser_);
            if (status_ != LXB_STATUS_OK) {
                critical("Failed to create parser!");
            }

            const std::string htmlContent = FileUtils::readFileAsString(filePath);

            document_ = lxb_html_parse(parser_, reinterpret_cast<const lxb_char_t *>(htmlContent.c_str()), htmlContent.length());
            if (document_ == nullptr) {
                critical("Failed to create Document object");
            }
        }

        ~HtmlUtility() {
            lxb_html_document_destroy(document_);
            lxb_html_parser_destroy(parser_);
        }

        [[nodiscard]] std::string toString() const {
            std::string result;
            const lxb_status_t status = lxb_html_serialize_pretty_tree_cb(lxb_dom_interface_node(document_),
                                                       LXB_HTML_SERIALIZE_OPT_UNDEF,
                                                       0, serializer_callback, &result);
            if (status != LXB_STATUS_OK) {
                critical("Failed to serialize HTML tree");
            }
            return result;
        }

    private:
        static lxb_status_t serializer_callback(const lxb_char_t *data, const size_t len, void *ctx) {
            if (ctx == nullptr) {
                return LXB_STATUS_ERROR;
            }
            const auto str = reinterpret_cast<const char*>(data);
            auto *result = static_cast<std::string*>(ctx);
            result->append(str, len);

            return LXB_STATUS_OK;
        }

    };
}

#endif //HTMLUTILITY_HPP
