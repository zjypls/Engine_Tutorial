#ifndef MOUSEBUTTON
#define MOUSEBUTTON
#include <iostream>
namespace Z
{
	typedef enum class MouseCode : uint16_t
	{
		// From glfw3.h
		Button1                = 0,
		Button2                = 1,
		Button3                = 2,
		Button4                = 3,
		Button5                = 4,
		Button6                = 5,
		Button7                = 6,
		Button8                = 7,

		ButtonLast             = Button7,
		ButtonLeft             = Button1,
		ButtonRight            = Button2,
		ButtonMiddle           = Button3
	} Mouse;

	inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode)
	{
		os << static_cast<int32_t>(mouseCode);
		return os;
	}
	inline bool operator==(MouseCode lhs, MouseCode rhs)
	{
		return static_cast<int32_t>(lhs) == static_cast<int32_t>(rhs);
	}

}

#define Z_MOUSE_BUTTON_0      ::Z::Mouse::Button0
#define Z_MOUSE_BUTTON_1      ::Z::Mouse::Button1
#define Z_MOUSE_BUTTON_2      ::Z::Mouse::Button2
#define Z_MOUSE_BUTTON_3      ::Z::Mouse::Button3
#define Z_MOUSE_BUTTON_4      ::Z::Mouse::Button4
#define Z_MOUSE_BUTTON_5      ::Z::Mouse::Button5
#define Z_MOUSE_BUTTON_6      ::Z::Mouse::Button6
#define Z_MOUSE_BUTTON_7      ::Z::Mouse::Button7
#define Z_MOUSE_BUTTON_LAST   ::Z::Mouse::ButtonLast
#define Z_MOUSE_BUTTON_LEFT   ::Z::Mouse::ButtonLeft
#define Z_MOUSE_BUTTON_RIGHT  ::Z::Mouse::ButtonRight
#define Z_MOUSE_BUTTON_MIDDLE ::Z::Mouse::ButtonMiddle

#endif // MOUSEBUTTON