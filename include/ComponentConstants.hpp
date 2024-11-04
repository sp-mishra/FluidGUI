#pragma once

#ifndef COMPONENTCONSTANTS_HPP
#define COMPONENTCONSTANTS_HPP
namespace groklab {

    enum class TextSize {
        h1,
        h2,
        h3,
        h4,
        h5,
        h6,
        small,
        medium,
        large,
        regular
    };

    enum class TextStyle {
        normal,
        italic,
        bold,
        underline,
        strikethrough,
        uppercase,
        lowercase,
        capitalize,
        overline,
        smallcaps,
        superscript,
        subscript
    };

    enum class TextColor {
        black,
        white,
        red,
        green,
        blue,
        yellow,
        orange,
        purple,
        pink,
        brown,
        gray,
        cyan,
        magenta,
        lime,
        maroon,
        navy,
        olive,
        teal,
        violet,
        indigo
    };

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
}
#endif //COMPONENTCONSTANTS_HPP
