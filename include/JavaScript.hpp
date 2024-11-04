// include/JavaScript.hpp
#pragma once

#ifndef JAVASCRIPT_HPP
#define JAVASCRIPT_HPP

#include <duktape.h>
#include <string>
#include <functional>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <vector>

namespace groklab {
    class JavaScript {
        duk_context *ctx_ = nullptr;

    public:
        JavaScript() {
            ctx_ = duk_create_heap_default();
            if (!ctx_) {
                throw std::runtime_error("Failed to create a Duktape heap.");
            }
        }

        ~JavaScript() {
            if (ctx_) {
                duk_destroy_heap(ctx_);
            }
        }

        // Evaluate JavaScript code
        [[nodiscard]] std::string eval(const std::string &code) const {
            if (duk_peval_string(ctx_, code.c_str()) != 0) {
                std::string error = duk_safe_to_string(ctx_, -1);
                duk_pop(ctx_);  // Pop the error from the stack
                throw std::runtime_error("JavaScript error: " + error);
            }
            std::string result = duk_safe_to_string(ctx_, -1);
            duk_pop(ctx_);  // Pop the result from the stack
            return result;
        }

        // Bind a C++ lambda to JavaScript
        void bind(const std::string &name, std::function<duk_ret_t(duk_context*)> func) const {
            duk_push_global_object(ctx_);
            duk_push_string(ctx_, name.c_str());
            duk_push_c_function(ctx_, [](duk_context *ctx) -> duk_ret_t {
                auto func = static_cast<std::function<duk_ret_t(duk_context*)>*>(duk_get_pointer(ctx, -1));
                return (*func)(ctx);
            }, DUK_VARARGS);
            duk_push_pointer(ctx_, &func);
            duk_put_prop_string(ctx_, -2, "\xff""\xff""data");
            duk_put_prop(ctx_, -3);
            duk_pop(ctx_);
        }

        // Load and evaluate an external JavaScript file
        [[nodiscard]] std::string load(const std::string &filePath) const {
            std::ifstream file(filePath);
            if (!file.is_open()) {
                throw std::runtime_error("Failed to open JavaScript file: " + filePath);
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            return eval(buffer.str());
        }

        // Call a JavaScript function from C++
        [[nodiscard]] std::string call(const std::string &funcName, const std::vector<std::string> &args) const {
            duk_push_global_object(ctx_);
            duk_get_prop_string(ctx_, -1, funcName.c_str());
            for (const auto &arg : args) {
                duk_push_string(ctx_, arg.c_str());
            }
            if (duk_pcall(ctx_, args.size()) != 0) {
                std::string error = duk_safe_to_string(ctx_, -1);
                duk_pop(ctx_);  // Pop the error from the stack
                throw std::runtime_error("JavaScript error: " + error);
            }
            std::string result = duk_safe_to_string(ctx_, -1);
            duk_pop_2(ctx_);  // Pop the result and global object from the stack
            return result;
        }

        // Register a C++ function to be called from JavaScript
        void registerFunction(const std::string &name, duk_c_function func, duk_idx_t nargs) const {
            duk_push_global_object(ctx_);
            duk_push_c_function(ctx_, func, nargs);
            duk_put_prop_string(ctx_, -2, name.c_str());
            duk_pop(ctx_);  // Pop the global object from the stack
        }

        // Create a JavaScript object
        void createObject(const std::string &name) const {
            duk_push_global_object(ctx_);
            duk_push_object(ctx_);
            duk_put_prop_string(ctx_, -2, name.c_str());
            duk_pop(ctx_);  // Pop the global object from the stack
        }

        // Create a JavaScript array
        void createArray(const std::string &name, const std::vector<std::string> &elements) const {
            duk_push_global_object(ctx_);
            duk_push_array(ctx_);
            for (duk_uarridx_t i = 0; i < elements.size(); ++i) {
                duk_push_string(ctx_, elements[i].c_str());
                duk_put_prop_index(ctx_, -2, i);
            }
            duk_put_prop_string(ctx_, -2, name.c_str());
            duk_pop(ctx_);  // Pop the global object from the stack
        }

        // Set a global JavaScript variable
        void setGlobal(const std::string &name, const std::string &value) const {
            duk_push_global_object(ctx_);
            duk_push_string(ctx_, value.c_str());
            duk_put_prop_string(ctx_, -2, name.c_str());
            duk_pop(ctx_);  // Pop the global object from the stack
        }

        // Get a global JavaScript variable
        [[nodiscard]] std::string getGlobal(const std::string &name) const {
            duk_push_global_object(ctx_);
            duk_get_prop_string(ctx_, -1, name.c_str());
            std::string result = duk_safe_to_string(ctx_, -1);
            duk_pop_2(ctx_);  // Pop the result and global object from the stack
            return result;
        }

        // Handle JavaScript exceptions
        void handleException() const {
            if (duk_is_error(ctx_, -1)) {
                duk_get_prop_string(ctx_, -1, "stack");
                std::string stack = duk_safe_to_string(ctx_, -1);
                duk_pop(ctx_);  // Pop the stack trace
                throw std::runtime_error("JavaScript error: " + stack);
            } else {
                std::string error = duk_safe_to_string(ctx_, -1);
                duk_pop(ctx_);  // Pop the error
                throw std::runtime_error("JavaScript error: " + error);
            }
        }

        // Push a value onto the Duktape stack
        void push(const std::string &value) const {
            duk_push_string(ctx_, value.c_str());
        }

        // Pop a value from the Duktape stack
        [[nodiscard]] std::string pop() const {
            std::string result = duk_safe_to_string(ctx_, -1);
            duk_pop(ctx_);
            return result;
        }

        // Serialize a JavaScript object to JSON
        [[nodiscard]] std::string toJson(const std::string &objName) const {
            duk_push_global_object(ctx_);
            duk_get_prop_string(ctx_, -1, objName.c_str());
            duk_json_encode(ctx_, -1);
            std::string result = duk_safe_to_string(ctx_, -1);
            duk_pop_2(ctx_);  // Pop the result and global object from the stack
            return result;
        }

        // Deserialize a JSON string to a JavaScript object
        void fromJson(const std::string &jsonStr, const std::string &objName) const {
            duk_push_global_object(ctx_);
            duk_push_string(ctx_, jsonStr.c_str());
            duk_json_decode(ctx_, -1);
            duk_put_prop_string(ctx_, -2, objName.c_str());
            duk_pop(ctx_);  // Pop the global object from the stack
        }

        // Set a timeout in JavaScript
        [[nodiscard]] std::string setTimeout(const std::string &funcName, int delay) const {
            const std::string code = "setTimeout(" + funcName + ", " + std::to_string(delay) + ");";
            return eval(code);
        }

        // Set an interval in JavaScript
        [[nodiscard]] std::string setInterval(const std::string &funcName, int interval) const {
            const std::string code = "setInterval(" + funcName + ", " + std::to_string(interval) + ");";
            return eval(code);
        }

        // Handle JavaScript promises
        [[nodiscard]] std::string handlePromise(const std::string &promiseCode) const {
            const std::string code = "new Promise(" + promiseCode + ").then(function(result) { return result; }).catch(function(error) { throw error; });";
            return eval(code);
        }

        // Load and evaluate a JavaScript module
        [[nodiscard]] std::string loadModule(const std::string &modulePath) const {
            std::ifstream file(modulePath);
            if (!file.is_open()) {
                throw std::runtime_error("Failed to open JavaScript module: " + modulePath);
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string code = "import * as module from '" + modulePath + "';\n" + buffer.str();
            return eval(code);
        }

        // Create a new JavaScript context
        void createContext() {
            if (ctx_) {
                duk_destroy_heap(ctx_);
            }
            ctx_ = duk_create_heap_default();
            if (!ctx_) {
                throw std::runtime_error("Failed to create a Duktape heap.");
            }
        }

        // Handle JavaScript events
        [[nodiscard]] std::string handleEvent(const std::string &eventCode) const {
            const std::string code = "document.addEventListener('event', function() {" + eventCode + "});";
            return eval(code);
        }

        // Enable debugging support
        void enableDebugging() const {
            duk_push_global_object(ctx_);
            duk_eval_string_noresult(ctx_, "if (typeof Duktape !== 'undefined') { Duktape.act = function() {}; }");
            duk_pop(ctx_);
        }

        // Memory management: push and pop values
        void pushInt(int value) const {
            duk_push_int(ctx_, value);
        }

        void pushDouble(double value) const {
            duk_push_number(ctx_, value);
        }

        void pushBoolean(bool value) const {
            duk_push_boolean(ctx_, value);
        }

        [[nodiscard]] int popInt() const {
            const int result = duk_get_int(ctx_, -1);
            duk_pop(ctx_);
            return result;
        }

        [[nodiscard]] double popDouble() const {
            const double result = duk_get_number(ctx_, -1);
            duk_pop(ctx_);
            return result;
        }

        [[nodiscard]] bool popBoolean() const {
            const bool result = duk_get_boolean(ctx_, -1);
            duk_pop(ctx_);
            return result;
        }

        // Advanced stack operations
        void dup(int index) const {
            duk_dup(ctx_, index);
        }

        void insert(int index) const {
            duk_insert(ctx_, index);
        }

        void remove(int index) const {
            duk_remove(ctx_, index);
        }

        void replace(int index) const {
            duk_replace(ctx_, index);
        }

        // Error handling and debugging
        void throwError(const std::string &message) const {
            duk_error(ctx_, DUK_ERR_ERROR, "%s", message.c_str());
        }

        void throwTypeError(const std::string &message) const {
            duk_error(ctx_, DUK_ERR_TYPE_ERROR, "%s", message.c_str());
        }

        void throwRangeError(const std::string &message) const {
            duk_error(ctx_, DUK_ERR_RANGE_ERROR, "%s", message.c_str());
        }

        // Timers and intervals
        [[nodiscard]] std::string clearTimeout(int id) const {
            const std::string code = "clearTimeout(" + std::to_string(id) + ");";
            return eval(code);
        }

        [[nodiscard]] std::string clearInterval(int id) const {
            const std::string code = "clearInterval(" + std::to_string(id) + ");";
            return eval(code);
        }

        // Modules and require
        [[nodiscard]] std::string require(const std::string &moduleName) const {
            const std::string code = "require('" + moduleName + "');";
            return eval(code);
        }

        // Event handling
        [[nodiscard]] std::string addEventListener(const std::string &event, const std::string &handler) const {
            const std::string code = "document.addEventListener('" + event + "', " + handler + ");";
            return eval(code);
        }

        [[nodiscard]] std::string removeEventListener(const std::string &event, const std::string &handler) const {
            const std::string code = "document.removeEventListener('" + event + "', " + handler + ");";
            return eval(code);
        }

        // JSON operations
        [[nodiscard]] std::string stringify(const std::string &objName) const {
            const std::string code = "JSON.stringify(" + objName + ");";
            return eval(code);
        }

        [[nodiscard]] std::string parse(const std::string &jsonStr) const {
            const std::string code = "JSON.parse('" + jsonStr + "');";
            return eval(code);
        }

        // Context management
        void resetContext() {
            if (ctx_) {
                duk_destroy_heap(ctx_);
            }
            ctx_ = duk_create_heap_default();
            if (!ctx_) {
                throw std::runtime_error("Failed to create a Duktape heap.");
            }
        }

        // Handling JavaScript classes
        [[nodiscard]] std::string defineClass(const std::string &className, const std::string &classBody) const {
            const std::string code = "class " + className + " {" + classBody + "};";
            return eval(code);
        }

        // Handling JavaScript symbols
        [[nodiscard]] std::string createSymbol(const std::string &symbolName) const {
            const std::string code = "Symbol('" + symbolName + "');";
            return eval(code);
        }

        // Handling JavaScript proxies
        [[nodiscard]] std::string createProxy(const std::string &target, const std::string &handler) const {
            const std::string code = "new Proxy(" + target + ", " + handler + ");";
            return eval(code);
        }

        // Handling JavaScript iterators and generators
        [[nodiscard]] std::string createIterator(const std::string &iteratorBody) const {
            const std::string code = "function* iterator() {" + iteratorBody + "};";
            return eval(code);
        }

        // Handling JavaScript typed arrays
        [[nodiscard]] std::string createTypedArray(const std::string &type, int length) const {
            const std::string code = "new " + type + "Array(" + std::to_string(length) + ");";
            return eval(code);
        }

        // Handling JavaScript maps and sets
        [[nodiscard]] std::string createMap() const {
            const std::string code = "new Map();";
            return eval(code);
        }

        [[nodiscard]] std::string createSet() const {
            const std::string code = "new Set();";
            return eval(code);
        }

        // Handling JavaScript weak maps and weak sets
        [[nodiscard]] std::string createWeakMap() const {
            const std::string code = "new WeakMap();";
            return eval(code);
        }

        [[nodiscard]] std::string createWeakSet() const {
            const std::string code = "new WeakSet();";
            return eval(code);
        }

        // Handling JavaScript async/await
        [[nodiscard]] std::string createAsyncFunction(const std::string &funcBody) const {
            const std::string code = "async function asyncFunc() {" + funcBody + "};";
            return eval(code);
        }

        // Handling JavaScript regular expressions
        [[nodiscard]] std::string createRegExp(const std::string &pattern, const std::string &flags) const {
            const std::string code = "new RegExp('" + pattern + "', '" + flags + "');";
            return eval(code);
        }

        // Handling JavaScript dates
        [[nodiscard]] std::string createDate(const std::string &dateString) const {
            const std::string code = "new Date('" + dateString + "');";
            return eval(code);
        }

        // Handling JavaScript buffers
        [[nodiscard]] std::string createBuffer(int size) const {
            const std::string code = "new ArrayBuffer(" + std::to_string(size) + ");";
            return eval(code);
        }

        [[nodiscard]] std::string createDataView(const std::string &buffer) const {
            const std::string code = "new DataView(" + buffer + ");";
            return eval(code);
        }
    };
}

#endif // JAVASCRIPT_HPP