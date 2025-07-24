#pragma once
#include <cstdint>

namespace CHIKU
{
    enum class MouseCode : uint8_t
    {
        Button0 = 0,
        Button1 = 1,
        Button2 = 2,
        Button3 = 3,
        Button4 = 4,
        Button5 = 5,
        Button6 = 6,
        Button7 = 7,

        // Aliases
        Left = Button0,
        Right = Button1,
        Middle = Button2,
        Back = Button3,
        Forward = Button4
    };
}