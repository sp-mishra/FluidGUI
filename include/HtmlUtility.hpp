#pragma once

#ifndef HTMLUTILITY_HPP
#define HTMLUTILITY_HPP

#include <lexbor/html/html.h>
#include <lexbor/dom/interfaces/element.h>
#include <string>
#include <rfl.hpp>
#include "FileUtils.hpp"
#include "Log.hpp"
#include "StringUtils.hpp"

namespace groklab {

    enum class HtmlTag {
        A,
        ABBR,
        ACRONYM,
        ADDRESS,
        APPLET,
        AREA,
        ARTICLE,
        ASIDE,
        AUDIO,
        B,
        BASE,
        BASEFONT,
        BDI,
        BDO,
        BIG,
        BLOCKQUOTE,
        BODY,
        BR,
        BUTTON,
        CANVAS,
        CAPTION,
        CENTER,
        CITE,
        CODE,
        COL,
        COLGROUP,
        DATA,
        DATALIST,
        DD,
        DEL,
        DETAILS,
        DFN,
        DIALOG,
        DIR,
        DIV,
        DL,
        DT,
        EM,
        EMBED,
        FIELDSET,
        FIGCAPTION,
        FIGURE,
        FONT,
        FOOTER,
        FORM,
        FRAME,
        FRAMESET,
        H1,
        H2,
        H3,
        H4,
        H5,
        H6,
        HEAD,
        HEADER,
        HR,
        HTML,
        I,
        IFRAME,
        IMG,
        INPUT,
        INS,
        KBD,
        LABEL,
        LEGEND,
        LI,
        LINK,
        MAIN,
        MAP,
        MARK,
        META,
        METER,
        NAV,
        NOFRAMES,
        NOSCRIPT,
        OBJECT,
        OL,
        OPTGROUP,
        OPTION,
        OUTPUT,
        P,
        PARAM,
        PICTURE,
        PRE,
        PROGRESS,
        Q,
        RP,
        RT,
        RUBY,
        S,
        SAMP,
        SCRIPT,
        SECTION,
        SELECT,
        SMALL,
        SOURCE,
        SPAN,
        STRIKE,
        STRONG,
        STYLE,
        SUB,
        SUMMARY,
        SUP,
        SVG,
        TABLE,
        TBODY,
        TD,
        TEMPLATE,
        TEXTAREA,
        TFOOT,
        TH,
        THEAD,
        TIME,
        TITLE,
        TR,
        TRACK,
        TT,
        U,
        UL,
        VAR,
        VIDEO,
        WBR
    };

    inline std::string toString(const HtmlTag val) {
        return StringUtils::toLowerCase(rfl::enum_to_string(val));
    }

    class HtmlUtility {
    public:
        enum class AttributeMatchType {
            kEquals,
            kContains,
            kStartsWith,
            kEndsWith
        };
    private:
        lxb_status_t status_{};
        lxb_html_parser_t *parser_{};
        lxb_html_document_t *document_{};
        const std::string rootAppElementName_ = {"q-app"};
        lxb_dom_collection_t *rootElementCollection_{};
        std::map<std::string, lxb_tag_id_t> tagIdMap_{};

    public:
        explicit HtmlUtility(const std::filesystem::path& filePath) {
            const std::string htmlContent = FileUtils::readFileAsString(filePath);
            initialize(htmlContent);
        }

        explicit HtmlUtility(const std::string& htmlContent) {
            initialize(htmlContent);
        }

        ~HtmlUtility() {
            cleanDomCollection(rootElementCollection_);
            lxb_html_document_destroy(document_);
            lxb_html_parser_destroy(parser_);
        }

        [[nodiscard]] std::string getTitle() const {
            size_t titleLength{0};
            const lxb_char_t *title = lxb_html_document_title(document_, &titleLength);
            return title == nullptr ? "" : reinterpret_cast<const char*>(title);
        }

        void setTitle(const std::string &title) const {
            const lxb_status_t status = lxb_html_document_title_set(document_, fromStringToLxbChar(title), title.size());
            if (status != LXB_STATUS_OK) {
                error("Failed to change HTML title to {}", title);
            }
        }

        void initRootAppElement() {
            rootElementCollection_ = findElementsWithTagName(rootAppElementName_);
        }

        void writeToFile(const std::filesystem::path& filePath) const {
            const std::string htmlContent = toString();
            FileUtils::writeToFile(filePath, htmlContent);
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

        static std::string nodeToString(lxb_dom_node_t *node) {
            std::string result;
            lxb_status_t status = lxb_html_serialize_pretty_cb(node, LXB_HTML_SERIALIZE_OPT_UNDEF,
                                                  0, serializer_callback, &result);
            if (status != LXB_STATUS_OK) {
                error("Failed to serialization HTML tree");
            }
            return result;
        }

        static void cleanDomCollection(lxb_dom_collection_t *collection) {
            lxb_dom_collection_clean(collection);
        }

        [[nodiscard]] lxb_dom_collection_t* findElementsWithTagName(const std::string& elementName) const {
            lxb_dom_collection_t *collection = lxb_dom_collection_make(&document_->dom_document, 128);
            if(nullptr == collection) {
                error("Failed to create collection object");
            }

            const lxb_status_t status = lxb_dom_elements_by_tag_name(lxb_dom_interface_element(document_->body),
                                                                     collection,
                                                                     fromStringToLxbChar(elementName),
                                                                     elementName.size());
            if (status != LXB_STATUS_OK) {
                error("Failed to get elements by name");
            }
            return collection;
        }

        static std::vector<std::string> lxbCollectionToString(lxb_dom_collection_t *collection) {
            const auto collectionSize = lxb_dom_collection_length(collection);
            std::vector<std::string> result(collectionSize);
            for (size_t i = 0; i < collectionSize; i++) {
                lxb_dom_element_t *element = lxb_dom_collection_element(collection, i);
                result.emplace_back(nodeToString(lxb_dom_interface_node(element)));
            }
            return result;
        }

        [[nodiscard]] lxb_dom_collection_t* findElementsByAttribute(const std::string& attributeName,
                                                      const std::string& attributeValue,
                                                      const AttributeMatchType matchType,
                                                      const bool ignoreCase = true) const {
            lxb_dom_collection_t *collection = lxb_dom_collection_make(&document_->dom_document, 128);
            if(nullptr == collection) {
                error("Failed to create collection object");
            }

            lxb_status_t status{};

            switch (matchType) {
                case AttributeMatchType::kEquals:
                    status = lxb_dom_elements_by_attr(lxb_dom_interface_element(document_->body),
                                                                         collection,
                                                                         fromStringToLxbChar(attributeName),
                                                                         attributeName.size(),
                                                                         fromStringToLxbChar(attributeValue),
                                                                         attributeValue.size(), ignoreCase);
                    break;
                case AttributeMatchType::kStartsWith:
                    status = lxb_dom_elements_by_attr_begin(lxb_dom_interface_element(document_->body),
                                                                         collection,
                                                                         fromStringToLxbChar(attributeName),
                                                                         attributeName.size(),
                                                                         fromStringToLxbChar(attributeValue),
                                                                         attributeValue.size(), ignoreCase);
                    break;
                case AttributeMatchType::kEndsWith:
                    status = lxb_dom_elements_by_attr_end(lxb_dom_interface_element(document_->body),
                                                                         collection,
                                                                         fromStringToLxbChar(attributeName),
                                                                         attributeName.size(),
                                                                         fromStringToLxbChar(attributeValue),
                                                                         attributeValue.size(), ignoreCase);
                break;
                default:
                    status = lxb_dom_elements_by_attr_contain(lxb_dom_interface_element(document_->body),
                                                     collection,
                                                     fromStringToLxbChar(attributeName),
                                                     attributeName.size(),
                                                     fromStringToLxbChar(attributeValue),
                                                     attributeValue.size(), ignoreCase);
            }

            if (status != LXB_STATUS_OK) {
                error("Failed to get elements by attribute");
            }
            return collection;
        }

        [[nodiscard]] lxb_tag_id_t tagNameToId(const std::string& tagName) const {
            if (const auto it = tagIdMap_.find(tagName); it != tagIdMap_.end()) {
                return it->second;
            }
            return LXB_TAG__UNDEF;
        }

    private:
        void initiateAllDocumentTags() {
            for (lxb_tag_id_t tag_id = LXB_TAG_A; tag_id < LXB_TAG__LAST_ENTRY; tag_id++) {
                size_t tag_name_len{};
                const lxb_char_t *tag_name = lxb_tag_name_by_id(tag_id, &tag_name_len);
                tagIdMap_[reinterpret_cast<const char*>(tag_name)] = tag_id;
            }
        }

        void initialize(const std::string& htmlContent) {
            /* Initialization */
            parser_ = lxb_html_parser_create();
            status_ = lxb_html_parser_init(parser_);
            if (status_ != LXB_STATUS_OK) {
                critical("Failed to create parser!");
            }

            document_ = lxb_html_parse(parser_, reinterpret_cast<const lxb_char_t *>(htmlContent.c_str()), htmlContent.length());
            if (document_ == nullptr) {
                critical("Failed to create Document object");
            }
            initiateAllDocumentTags();
        }

        static lxb_status_t serializer_callback(const lxb_char_t *data, const size_t len, void *ctx) {
            if (ctx == nullptr) {
                return LXB_STATUS_ERROR;
            }
            const auto str = reinterpret_cast<const char*>(data);
            auto *result = static_cast<std::string*>(ctx);
            result->append(str, len);

            return LXB_STATUS_OK;
        }

        static lxb_char_t* fromStringToLxbChar(const std::string& str) {
            return reinterpret_cast<lxb_char_t*>(const_cast<char*>(str.c_str()));
        }

    };
}

#endif //HTMLUTILITY_HPP
