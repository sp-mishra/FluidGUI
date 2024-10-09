#pragma once

#ifndef LOG_HPP
#define LOG_HPP
#include <spdlog/spdlog.h>

namespace groklab {
    template <typename... Args>
    void info(spdlog::format_string_t<Args...> fmt, Args &&...args) {
        spdlog::info(fmt, args...);
    }

    template <typename... Args>
    void warn(spdlog::format_string_t<Args...> fmt, Args &&...args) {
        spdlog::warn(fmt, args...);
    }

    template <typename... Args>
    void error(spdlog::format_string_t<Args...> fmt, Args &&...args) {
        spdlog::error(fmt, args...);
    }

    template <typename... Args>
    void debug(spdlog::format_string_t<Args...> fmt, Args &&...args) {
        spdlog::debug(fmt, args...);
    }

    template <typename... Args>
    void critical(spdlog::format_string_t<Args...> fmt, Args &&...args) {
        spdlog::critical(fmt, args...);
    }
}

#endif //LOG_HPP