#pragma once

#ifndef SCREENUTILS_HPP
#define SCREENUTILS_HPP

#ifdef __APPLE__
#include <CoreGraphics/CGDisplayConfiguration.h>
#endif

namespace groklab {
    struct ScreenSize {
        int width;
        int height;
        ScreenSize() = default;
        ScreenSize(int width, int height) : width(width), height(height) {}
    };

    struct ScreenPosition {
        int x;
        int y;
    };

    struct ScreenRect {
        ScreenPosition position;
        ScreenSize size;
    };

    inline ScreenSize getScreenSize();

#ifdef __APPLE__
    inline ScreenSize getScreenSize() {
        const CGDirectDisplayID displayID = CGMainDisplayID();
        const ScreenSize screenSize = {static_cast<int>(CGDisplayPixelsWide(displayID)),
            static_cast<int>(CGDisplayPixelsHigh(displayID))};
        return screenSize;
    }
#endif
}

#endif //SCREENUTILS_HPP
