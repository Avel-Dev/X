#pragma once
#include <cstdint>

namespace CHIKU
{
    enum class KeyCode : uint16_t
    {
        // Printable keys
        Space = 32,
        Apostrophe = 39,  // '
        Comma = 44,
        Minus = 45,
        Period = 46,
        Slash = 47,

        D0 = 48,
        D1 = 49,
        D2 = 50,
        D3 = 51,
        D4 = 52,
        D5 = 53,
        D6 = 54,
        D7 = 55,
        D8 = 56,
        D9 = 57,

        Semicolon = 59,
        Equal = 61,

        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,

        LeftBracket = 91,
        Backslash = 92,
        RightBracket = 93,
        GraveAccent = 96,

        // Function keys
        Escape = 256,
        Enter = 257,
        Tab = 258,
        Backspace = 259,
        Insert = 260,
        Delete = 261,
        Right = 262,
        Left = 263,
        Down = 264,
        Up = 265,
        PageUp = 266,
        PageDown = 267,
        Home = 268,
        End = 269,

        CapsLock = 280,
        ScrollLock = 281,
        NumLock = 282,
        PrintScreen = 283,
        Pause = 284,

        F1 = 290,
        F2 = 291,
        F3 = 292,
        F4 = 293,
        F5 = 294,
        F6 = 295,
        F7 = 296,
        F8 = 297,
        F9 = 298,
        F10 = 299,
        F11 = 300,
        F12 = 301,

        // Keypad
        KP0 = 320,
        KP1 = 321,
        KP2 = 322,
        KP3 = 323,
        KP4 = 324,
        KP5 = 325,
        KP6 = 326,
        KP7 = 327,
        KP8 = 328,
        KP9 = 329,
        KPDecimal = 330,
        KPDivide = 331,
        KPMultiply = 332,
        KPSubtract = 333,
        KPAdd = 334,
        KPEnter = 335,
        KPEqual = 336,

        // Modifier keys
        LeftShift = 340,
        LeftControl = 341,
        LeftAlt = 342,
        LeftSuper = 343,
        RightShift = 344,
        RightControl = 345,
        RightAlt = 346,
        RightSuper = 347,
        Menu = 348
    };

    static const char* KeyCodeToString(KeyCode code) {
        switch (code) {
        case KeyCode::Space: return "Space";
        case KeyCode::Apostrophe: return "'";
        case KeyCode::Comma: return ",";
        case KeyCode::Minus: return "-";
        case KeyCode::Period: return ".";
        case KeyCode::Slash: return "/";

        case KeyCode::D0: return "0";
        case KeyCode::D1: return "1";
        case KeyCode::D2: return "2";
        case KeyCode::D3: return "3";
        case KeyCode::D4: return "4";
        case KeyCode::D5: return "5";
        case KeyCode::D6: return "6";
        case KeyCode::D7: return "7";
        case KeyCode::D8: return "8";
        case KeyCode::D9: return "9";

        case KeyCode::Semicolon: return ";";
        case KeyCode::Equal: return "=";

        case KeyCode::A: return "A";
        case KeyCode::B: return "B";
        case KeyCode::C: return "C";
        case KeyCode::D: return "D";
        case KeyCode::E: return "E";
        case KeyCode::F: return "F";
        case KeyCode::G: return "G";
        case KeyCode::H: return "H";
        case KeyCode::I: return "I";
        case KeyCode::J: return "J";
        case KeyCode::K: return "K";
        case KeyCode::L: return "L";
        case KeyCode::M: return "M";
        case KeyCode::N: return "N";
        case KeyCode::O: return "O";
        case KeyCode::P: return "P";
        case KeyCode::Q: return "Q";
        case KeyCode::R: return "R";
        case KeyCode::S: return "S";
        case KeyCode::T: return "T";
        case KeyCode::U: return "U";
        case KeyCode::V: return "V";
        case KeyCode::W: return "W";
        case KeyCode::X: return "X";
        case KeyCode::Y: return "Y";
        case KeyCode::Z: return "Z";

        case KeyCode::LeftBracket: return "[";
        case KeyCode::Backslash: return "\\";
        case KeyCode::RightBracket: return "]";
        case KeyCode::GraveAccent: return "`";

        case KeyCode::Escape: return "Escape";
        case KeyCode::Enter: return "Enter";
        case KeyCode::Tab: return "Tab";
        case KeyCode::Backspace: return "Backspace";
        case KeyCode::Insert: return "Insert";
        case KeyCode::Delete: return "Delete";
        case KeyCode::Right: return "Right";
        case KeyCode::Left: return "Left";
        case KeyCode::Down: return "Down";
        case KeyCode::Up: return "Up";
        case KeyCode::PageUp: return "PageUp";
        case KeyCode::PageDown: return "PageDown";
        case KeyCode::Home: return "Home";
        case KeyCode::End: return "End";

        case KeyCode::CapsLock: return "CapsLock";
        case KeyCode::ScrollLock: return "ScrollLock";
        case KeyCode::NumLock: return "NumLock";
        case KeyCode::PrintScreen: return "PrintScreen";
        case KeyCode::Pause: return "Pause";

        case KeyCode::F1: return "F1";
        case KeyCode::F2: return "F2";
        case KeyCode::F3: return "F3";
        case KeyCode::F4: return "F4";
        case KeyCode::F5: return "F5";
        case KeyCode::F6: return "F6";
        case KeyCode::F7: return "F7";
        case KeyCode::F8: return "F8";
        case KeyCode::F9: return "F9";
        case KeyCode::F10: return "F10";
        case KeyCode::F11: return "F11";
        case KeyCode::F12: return "F12";

        case KeyCode::KP0: return "Keypad 0";
        case KeyCode::KP1: return "Keypad 1";
        case KeyCode::KP2: return "Keypad 2";
        case KeyCode::KP3: return "Keypad 3";
        case KeyCode::KP4: return "Keypad 4";
        case KeyCode::KP5: return "Keypad 5";
        case KeyCode::KP6: return "Keypad 6";
        case KeyCode::KP7: return "Keypad 7";
        case KeyCode::KP8: return "Keypad 8";
        case KeyCode::KP9: return "Keypad 9";
        case KeyCode::KPDecimal: return "Keypad .";
        case KeyCode::KPDivide: return "Keypad /";
        case KeyCode::KPMultiply: return "Keypad *";
        case KeyCode::KPSubtract: return "Keypad -";
        case KeyCode::KPAdd: return "Keypad +";
        case KeyCode::KPEnter: return "Keypad Enter";
        case KeyCode::KPEqual: return "Keypad =";

        case KeyCode::LeftShift: return "Left Shift";
        case KeyCode::LeftControl: return "Left Ctrl";
        case KeyCode::LeftAlt: return "Left Alt";
        case KeyCode::LeftSuper: return "Left Super";
        case KeyCode::RightShift: return "Right Shift";
        case KeyCode::RightControl: return "Right Ctrl";
        case KeyCode::RightAlt: return "Right Alt";
        case KeyCode::RightSuper: return "Right Super";
        case KeyCode::Menu: return "Menu";

        default: return "Unknown";
        }
    }

}