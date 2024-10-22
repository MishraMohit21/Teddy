#pragma once
#include <Teddy/Core/Core.h>

namespace Teddy
{
    typedef enum class MouseCode : uint16_t
    {
        // From glfw3.h
        Button0 = 0,
        Button1 = 1,
        Button2 = 2,
        Button3 = 3,
        Button4 = 4,
        Button5 = 5,
        Button6 = 6,
        Button7 = 7,

        ButtonLast = Button7,
        ButtonLeft = Button0,
        ButtonRight = Button1,
        ButtonMiddle = Button2
    } Mouse;

    inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode)
    {
        os << static_cast<int>(static_cast<uint16_t>(mouseCode)); // Cast to int
        return os;
    }
}

#define TD_MOUSE_BUTTON_0      ::Teddy::Mouse::Button0
#define TD_MOUSE_BUTTON_1      ::Teddy::Mouse::Button1
#define TD_MOUSE_BUTTON_2      ::Teddy::Mouse::Button2
#define TD_MOUSE_BUTTON_3      ::Teddy::Mouse::Button3
#define TD_MOUSE_BUTTON_4      ::Teddy::Mouse::Button4
#define TD_MOUSE_BUTTON_5      ::Teddy::Mouse::Button5
#define TD_MOUSE_BUTTON_6      ::Teddy::Mouse::Button6
#define TD_MOUSE_BUTTON_7      ::Teddy::Mouse::Button7
#define TD_MOUSE_BUTTON_LAST   ::Teddy::Mouse::ButtonLast
#define TD_MOUSE_BUTTON_LEFT   ::Teddy::Mouse::ButtonLeft
#define TD_MOUSE_BUTTON_RIGHT  ::Teddy::Mouse::ButtonRight
#define TD_MOUSE_BUTTON_MIDDLE ::Teddy::Mouse::ButtonMiddle