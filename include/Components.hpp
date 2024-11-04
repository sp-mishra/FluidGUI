#pragma once

#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <string>
#include <unordered_map>
#include <sstream>
#include <regex>
#include <fstream>
#include <functional>
#include <any>
#include <rfl.hpp>
#include <utility>

#include "HtmlUtility.hpp"
#include "ComponentConstants.hpp"


namespace groklab {
    template<class T>
    class InputValue {
        static_assert(std::is_integral_v<T> || std::is_same_v<T, std::string> ||
                      std::is_same_v<T, bool> || std::is_same_v<T, float> || std::is_same_v<T, double>,
                      "InputValue only accepts integral types, std::string, bool, or float");
        using Type = T;
        std::string name_{};
        T value_{};
        T defaultValue_{};

    public:
        static const std::string type;

        InputValue() = default;

        explicit InputValue(std::string name, T value, T defaultValue = T()) :
        name_(std::move(name)),
        value_(std::move(value)), defaultValue_(std::move(defaultValue)) {
        }

        void setName(const std::string &name) {
            this->name_ = name;
        }

        [[nodiscard]] std::string getName() const {
            return name_;
        }

        [[nodiscard]] T getValue() const {
            return value_;
        }

        void setValue(T value) {
            this->value_ = value;
        }

        void setDefaultValue(T defaultValue) {
            this->defaultValue_ = defaultValue;
        }

        [[nodiscard]] T getDefaultValue() const {
            return defaultValue_;
        }

        // Define comparison operators
        bool operator<(const InputValue &other) const {
            return std::tie(name_, value_) < std::tie(other.name_, other.value_);
        }

        bool operator==(const InputValue &other) const {
            return std::tie(name_, value_) == std::tie(other.name_, other.value_);
        }
    };

    template<typename BaseType>
    class Component {
    public:
        using State = std::unordered_map<std::string, std::string>;
        using FunctionMap = std::map<std::string, std::function<void()> >;
        using ComponentPtr = std::shared_ptr<Component>;
        using InputValueVariant = std::variant<
            InputValue<char>,
            InputValue<unsigned char>,
            InputValue<int>,
            InputValue<unsigned int>,
            InputValue<short>,
            InputValue<unsigned short>,
            InputValue<long>,
            InputValue<unsigned long>,
            InputValue<long long>,
            InputValue<unsigned long long>,
            InputValue<std::string>,
            InputValue<bool>,
            InputValue<float>,
            InputValue<double>,
            InputValue<size_t>
        >;
        using InputValueSet = std::set<InputValueVariant>;

    protected:
        State state_;
        std::string name_;
        std::string template_;
        std::string css_;
        std::string javascript_;
        FunctionMap functionMap_;
        std::vector<ComponentPtr> children_;
        InputValueSet inputValues_;
        std::unique_ptr<HtmlUtility> htmlUtility_;
        std::string scope_{};

    public:
        explicit Component(std::string componentName) : name_(std::move(componentName)) {
            const std::string componentNameVue = name_ + ".vue";
            const std::filesystem::path basePath("./web/vue/");
            const std::filesystem::path fullPath = basePath / componentNameVue;
            initFromVueFile(fullPath);
        }

        explicit Component(const std::filesystem::path &filePath) : name_(filePath.stem().string()) {
            initFromVueFile(filePath);
        }

        explicit Component(const std::string &content, std::string componentName) : name_(std::move(componentName)) {
            initFromVueString(content);
        }

        Component() = default;

        virtual ~Component() = default;

        template<typename ChildComponent, typename... Args>
        void addChild(Args &&... args) {
            auto child = std::make_shared<ChildComponent>(std::forward<Args>(args)...);
            children_.emplace_back(std::move(child));
        }

        [[nodiscard]] const std::vector<ComponentPtr> &getChildren() const {
            return children_;
        }

        void setState(const std::string &key, const std::string &value) {
            state_[key] = value;
        }

        [[nodiscard]] std::string getState(const std::string &key) const {
            auto it = state_.find(key);
            if (it != state_.end()) {
                return it->second;
            }
            return "";
        }

        [[nodiscard]] std::string getScope() const {
            if(scope_.empty()) {
                scope_ = typeid(BaseType).name();
            }
            return scope_;
        }

        void addInputValue(const InputValueVariant &inputValue) {
            inputValues_.insert(inputValue);
        }

        [[nodiscard]] const InputValueSet &getInputValues() const {
            return inputValues_;
        }

        [[nodiscard]] std::optional<InputValueVariant> getInputValues(const std::string &name) const {
            for (const auto &inputValue: inputValues_) {
                if (std::visit([&](const auto &val) { return val.getName() == name; }, inputValue)) {
                    return inputValue;
                }
            }
            return std::nullopt;
        }

        virtual void render() {
            compose();

        }

        [[nodiscard]] std::string getName() const {
            return name_;
        }

        [[nodiscard]] std::string getTemplate() const {
            return template_;
        }

        [[nodiscard]] std::string getCss() const {
            return css_;
        }

        [[nodiscard]] std::string getJavascript() const {
            return javascript_;
        }

        template<typename Func>
        void addFunction(const std::string &name, Func &&func) {
            functionMap_[name] = [func = std::forward<Func>(func)]() mutable {
                func(); // Calls the stored function
            };
        }

        template<typename... Args>
        void callFunction(const std::string &name, Args &&... args) {
            // If the function exists, cast it to a callable with matching arguments
            if (auto it = functionMap_.find(name); it != functionMap_.end()) {
                // Cast and call the function with forwarded arguments
                std::invoke(it->second, std::forward<Args>(args)...);
            } else {
                std::cerr << "Function '" << name << "' not found.\n";
            }
        }

        virtual void mounted() {
            for (const auto &child: children_) {
                child->mounted();
            }
        }

        virtual void updated() {
            for (const auto &child: children_) {
                child->updated();
            }
        }

    protected:
        std::string generateProps() {
            std::ostringstream propsStream;
            propsStream << "props: {\n";

            for (const auto &inputValue : inputValues_) {
                std::visit([&propsStream, this](const auto &val) {
                    using T = std::decay_t<decltype(val)>;
                    propsStream << "  " << this->scope_ + "_" + val.getName() << ": {\n";
                    if constexpr (std::is_same_v<T, InputValue<std::string>>) {
                        propsStream << "    type: String,\n";
                    } else if constexpr (std::is_same_v<T, InputValue<int>> || std::is_same_v<T, InputValue<long>> || std::is_same_v<T, InputValue<short>>) {
                        propsStream << "    type: Number,\n";
                    } else if constexpr (std::is_same_v<T, InputValue<bool>>) {
                        propsStream << "    type: Boolean,\n";
                    } else if constexpr (std::is_same_v<T, InputValue<float>> || std::is_same_v<T, InputValue<double>>) {
                        propsStream << "    type: Number,\n";
                    }
                    propsStream << "    default: " << (val.getDefaultValue().empty() ? "''" : val.getDefaultValue()) << "\n";
                    propsStream << "  },\n";
                }, inputValue);

                return propsStream.str();
            }

            propsStream << "}\n";
            return propsStream.str();
        }

        void parseHtml() const {
            HtmlUtility htmlUtility(template_);
        }

        virtual void compose() = 0;

        void initFromVueFile(const std::filesystem::path &filePath) {
            std::ifstream file(filePath);
            if (!file) {
                auto msg = std::format("Could not open the file! {}", filePath.string());
                throw std::runtime_error(msg);
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();
            name_ = filePath.stem().string();
            initFromVueString(content);
        }

        void initFromVueString(const std::string &content) {
            htmlUtility_ =  std::make_unique<HtmlUtility>(content);
            template_ = getTemplateContent(content);
            css_ = getStyleContent(content);
            javascript_ = getScriptContent(content);
        }

        static std::string getTagContent(const std::string &content, const std::string &tagName) {
            // Construct the regex pattern for the specified tag and account for multiline content
            const std::string pattern = "<" + tagName + R"(\b[^>]*>([\s\S]*?)</)" + tagName + ">";

            // Compile the regex with our pattern
            const std::regex tagRegex(pattern, std::regex::icase);

            // Search for the first match of the tag content
            if (std::smatch matches; std::regex_search(content, matches, tagRegex)) {
                return matches[1].str(); // Return the content inside the tag
            }
            return ""; // Return empty if no match found
        }

        static std::string getScriptContent(const std::string &content) {
            // Construct the regex pattern to match content inside `export default { ... }`
            const std::string pattern = R"(export\s+default\s+\{([\s\S]*?)\})";

            // Compile the regex pattern
            const std::regex scriptRegex(pattern, std::regex::icase); // Case insensitive

            // Search for the match of `export default { ... }`
            if (std::smatch matches; std::regex_search(content, matches, scriptRegex)) {
                return matches[1].str(); // Return the content inside the `export default { ... }`
            }
            return ""; // Return empty if no match found
        }

        [[nodiscard]] std::string generateScopedName(const std::string& name) const {
            return scope_ + "-" + name;
        }

        static std::string getTemplateContent(const std::string &filePath) {
            return getTagContent(filePath, "template");
        }

        static std::string getStyleContent(const std::string &filePath) {
            return getTagContent(filePath, "style");
        }
    };

    // App Component
    class AppComponent final : public Component<AppComponent> {
    public:
        explicit AppComponent(const std::filesystem::path &filePath)
            : Component() {
            initialize(filePath);
        }

        AppComponent(const std::string &content, const std::string &componentName)
            : Component(content, componentName) {
        }

        ~AppComponent() override = default;

        void render() override {
        };

        void mounted() override {
        };

        void updated() override {
        };

    private:
        // Example functions with various arguments
        void exampleFunction(int x, double y) {
            info("Called example_function with: x: {}, y: {}", x, y);
        }

        void anotherFunction(std::string message) {
            info("Called another_function with message: {}", message);
        }

        void initialize(const std::filesystem::path &filePath) {
            HtmlUtility htmlUtils(filePath);
            // htmlUtils.getAttribute();
            info("Title: {}", htmlUtils.getTitle());
            htmlUtils.setTitle("New Title");
            info("Title: {}", htmlUtils.getTitle());
            htmlUtils.initRootAppElement();
            htmlUtils.writeToFile("./index-tmp.html");

            addFunction("lambda_func", []() {
                std::cout << "Called lambda_func with no arguments.\n";
            });
            // Add example_function and another_function
            addFunction("example_function", [this]() { exampleFunction(42, 3.14); });
            addFunction("another_function", [this]() { anotherFunction("Hello from map!"); });
        }
    };

    // Text component
    class TextDisplay final : public Component<TextDisplay> {
    public:
        static const std::string TEXT_INPUT;
        static const std::string SIZE_INPUT;
        static const std::string STYLE_INPUT;
        static const std::string COLOR_INPUT;
        TextDisplay(std::string text, TextSize textSize, TextStyle textStyle, TextColor textColor)
        : Component(std::string{"TextDisplay"}) {
            addInputValue(InputValue(TEXT_INPUT, std::move(text)));
            addInputValue(InputValue(SIZE_INPUT, rfl::enum_to_string(textSize)));
            addInputValue(InputValue(STYLE_INPUT, rfl::enum_to_string(textStyle)));
            addInputValue(InputValue(COLOR_INPUT, rfl::enum_to_string(textColor)));
        }

        [[nodiscard]] std::string getText() const {
            if (const auto value = getInputValues(TEXT_INPUT); value.has_value()) {
                return std::get<InputValue<std::string>>(value.value()).getValue();
            }
            return "";
        }

        void render() override{ };
        void compose() {}
    };
    // Definition of the static member
    const std::string TextDisplay::TEXT_INPUT = "text";
    const std::string TextDisplay::SIZE_INPUT = "size";
    const std::string TextDisplay::STYLE_INPUT = "style";
    const std::string TextDisplay::COLOR_INPUT = "color";
}

#endif // COMPONENT_HPP
