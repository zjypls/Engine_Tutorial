#ifndef KEYCODE_H
#define KEYCODE_H
#include <iostream>
namespace Z
{
	typedef enum class KeyCode : uint16_t
	{
		// From glfw3.h
		Space               = 32,
		Apostrophe          = 39, /* ' */
		Comma               = 44, /* , */
		Minus               = 45, /* - */
		Period              = 46, /* . */
		Slash               = 47, /* / */

		D0                  = 48, /* 0 */
		D1                  = 49, /* 1 */
		D2                  = 50, /* 2 */
		D3                  = 51, /* 3 */
		D4                  = 52, /* 4 */
		D5                  = 53, /* 5 */
		D6                  = 54, /* 6 */
		D7                  = 55, /* 7 */
		D8                  = 56, /* 8 */
		D9                  = 57, /* 9 */

		Semicolon           = 59, /* ; */
		Equal               = 61, /* = */

		A                   = 65,
		B                   = 66,
		C                   = 67,
		D                   = 68,
		E                   = 69,
		F                   = 70,
		G                   = 71,
		H                   = 72,
		I                   = 73,
		J                   = 74,
		K                   = 75,
		L                   = 76,
		M                   = 77,
		N                   = 78,
		O                   = 79,
		P                   = 80,
		Q                   = 81,
		R                   = 82,
		S                   = 83,
		T                   = 84,
		U                   = 85,
		V                   = 86,
		W                   = 87,
		X                   = 88,
		Y                   = 89,
		Z                   = 90,

		LeftBracket         = 91,  /* [ */
		Backslash           = 92,  /* \ */
		RightBracket        = 93,  /* ] */
		GraveAccent         = 96,  /* ` */

		World1              = 161, /* non-US #1 */
		World2              = 162, /* non-US #2 */

		/* Function keys */
		Escape              = 256,
		Enter               = 257,
		Tab                 = 258,
		Backspace           = 259,
		Insert              = 260,
		Delete              = 261,
		Right               = 262,
		Left                = 263,
		Down                = 264,
		Up                  = 265,
		PageUp              = 266,
		PageDown            = 267,
		Home                = 268,
		End                 = 269,
		CapsLock            = 280,
		ScrollLock          = 281,
		NumLock             = 282,
		PrintScreen         = 283,
		Pause               = 284,
		F1                  = 290,
		F2                  = 291,
		F3                  = 292,
		F4                  = 293,
		F5                  = 294,
		F6                  = 295,
		F7                  = 296,
		F8                  = 297,
		F9                  = 298,
		F10                 = 299,
		F11                 = 300,
		F12                 = 301,
		F13                 = 302,
		F14                 = 303,
		F15                 = 304,
		F16                 = 305,
		F17                 = 306,
		F18                 = 307,
		F19                 = 308,
		F20                 = 309,
		F21                 = 310,
		F22                 = 311,
		F23                 = 312,
		F24                 = 313,
		F25                 = 314,

		/* Keypad */
		KP0                 = 320,
		KP1                 = 321,
		KP2                 = 322,
		KP3                 = 323,
		KP4                 = 324,
		KP5                 = 325,
		KP6                 = 326,
		KP7                 = 327,
		KP8                 = 328,
		KP9                 = 329,
		KPDecimal           = 330,
		KPDivide            = 331,
		KPMultiply          = 332,
		KPSubtract          = 333,
		KPAdd               = 334,
		KPEnter             = 335,
		KPEqual             = 336,

		LeftShift           = 340,
		LeftControl         = 341,
		LeftAlt             = 342,
		LeftSuper           = 343,
		RightShift          = 344,
		RightControl        = 345,
		RightAlt            = 346,
		RightSuper          = 347,
		Menu                = 348
	} Key;

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
	inline bool operator==(KeyCode lhs, KeyCode rhs)
	{
		return static_cast<int32_t>(lhs) == static_cast<int32_t>(rhs);
	}
}

// From glfw3.h
#define Z_KEY_SPACE           ::Z::Key::Space
#define Z_KEY_APOSTROPHE      ::Z::Key::Apostrophe    /* ' */
#define Z_KEY_COMMA           ::Z::Key::Comma         /* , */
#define Z_KEY_MINUS           ::Z::Key::Minus         /* - */
#define Z_KEY_PERIOD          ::Z::Key::Period        /* . */
#define Z_KEY_SLASH           ::Z::Key::Slash         /* / */
#define Z_KEY_0               ::Z::Key::D0
#define Z_KEY_1               ::Z::Key::D1
#define Z_KEY_2               ::Z::Key::D2
#define Z_KEY_3               ::Z::Key::D3
#define Z_KEY_4               ::Z::Key::D4
#define Z_KEY_5               ::Z::Key::D5
#define Z_KEY_6               ::Z::Key::D6
#define Z_KEY_7               ::Z::Key::D7
#define Z_KEY_8               ::Z::Key::D8
#define Z_KEY_9               ::Z::Key::D9
#define Z_KEY_SEMICOLON       ::Z::Key::Semicolon     /* ; */
#define Z_KEY_EQUAL           ::Z::Key::Equal         /* = */
#define Z_KEY_A               ::Z::Key::A
#define Z_KEY_B               ::Z::Key::B
#define Z_KEY_C               ::Z::Key::C
#define Z_KEY_D               ::Z::Key::D
#define Z_KEY_E               ::Z::Key::E
#define Z_KEY_F               ::Z::Key::F
#define Z_KEY_G               ::Z::Key::G
#define Z_KEY_H               ::Z::Key::H
#define Z_KEY_I               ::Z::Key::I
#define Z_KEY_J               ::Z::Key::J
#define Z_KEY_K               ::Z::Key::K
#define Z_KEY_L               ::Z::Key::L
#define Z_KEY_M               ::Z::Key::M
#define Z_KEY_N               ::Z::Key::N
#define Z_KEY_O               ::Z::Key::O
#define Z_KEY_P               ::Z::Key::P
#define Z_KEY_Q               ::Z::Key::Q
#define Z_KEY_R               ::Z::Key::R
#define Z_KEY_S               ::Z::Key::S
#define Z_KEY_T               ::Z::Key::T
#define Z_KEY_U               ::Z::Key::U
#define Z_KEY_V               ::Z::Key::V
#define Z_KEY_W               ::Z::Key::W
#define Z_KEY_X               ::Z::Key::X
#define Z_KEY_Y               ::Z::Key::Y
#define Z_KEY_Z               ::Z::Key::Z
#define Z_KEY_LEFT_BRACKET    ::Z::Key::LeftBracket   /* [ */
#define Z_KEY_BACKSLASH       ::Z::Key::Backslash     /* \ */
#define Z_KEY_RIGHT_BRACKET   ::Z::Key::RightBracket  /* ] */
#define Z_KEY_GRAVE_ACCENT    ::Z::Key::GraveAccent   /* ` */
#define Z_KEY_WORLD_1         ::Z::Key::World1        /* non-US #1 */
#define Z_KEY_WORLD_2         ::Z::Key::World2        /* non-US #2 */

/* Function keys */
#define Z_KEY_ESCAPE          ::Z::Key::Escape
#define Z_KEY_ENTER           ::Z::Key::Enter
#define Z_KEY_TAB             ::Z::Key::Tab
#define Z_KEY_BACKSPACE       ::Z::Key::Backspace
#define Z_KEY_INSERT          ::Z::Key::Insert
#define Z_KEY_DELETE          ::Z::Key::Delete
#define Z_KEY_RIGHT           ::Z::Key::Right
#define Z_KEY_LEFT            ::Z::Key::Left
#define Z_KEY_DOWN            ::Z::Key::Down
#define Z_KEY_UP              ::Z::Key::Up
#define Z_KEY_PAGE_UP         ::Z::Key::PageUp
#define Z_KEY_PAGE_DOWN       ::Z::Key::PageDown
#define Z_KEY_HOME            ::Z::Key::Home
#define Z_KEY_END             ::Z::Key::End
#define Z_KEY_CAPS_LOCK       ::Z::Key::CapsLock
#define Z_KEY_SCROLL_LOCK     ::Z::Key::ScrollLock
#define Z_KEY_NUM_LOCK        ::Z::Key::NumLock
#define Z_KEY_PRINT_SCREEN    ::Z::Key::PrintScreen
#define Z_KEY_PAUSE           ::Z::Key::Pause
#define Z_KEY_F1              ::Z::Key::F1
#define Z_KEY_F2              ::Z::Key::F2
#define Z_KEY_F3              ::Z::Key::F3
#define Z_KEY_F4              ::Z::Key::F4
#define Z_KEY_F5              ::Z::Key::F5
#define Z_KEY_F6              ::Z::Key::F6
#define Z_KEY_F7              ::Z::Key::F7
#define Z_KEY_F8              ::Z::Key::F8
#define Z_KEY_F9              ::Z::Key::F9
#define Z_KEY_F10             ::Z::Key::F10
#define Z_KEY_F11             ::Z::Key::F11
#define Z_KEY_F12             ::Z::Key::F12
#define Z_KEY_F13             ::Z::Key::F13
#define Z_KEY_F14             ::Z::Key::F14
#define Z_KEY_F15             ::Z::Key::F15
#define Z_KEY_F16             ::Z::Key::F16
#define Z_KEY_F17             ::Z::Key::F17
#define Z_KEY_F18             ::Z::Key::F18
#define Z_KEY_F19             ::Z::Key::F19
#define Z_KEY_F20             ::Z::Key::F20
#define Z_KEY_F21             ::Z::Key::F21
#define Z_KEY_F22             ::Z::Key::F22
#define Z_KEY_F23             ::Z::Key::F23
#define Z_KEY_F24             ::Z::Key::F24
#define Z_KEY_F25             ::Z::Key::F25

/* Keypad */
#define Z_KEY_KP_0            ::Z::Key::KP0
#define Z_KEY_KP_1            ::Z::Key::KP1
#define Z_KEY_KP_2            ::Z::Key::KP2
#define Z_KEY_KP_3            ::Z::Key::KP3
#define Z_KEY_KP_4            ::Z::Key::KP4
#define Z_KEY_KP_5            ::Z::Key::KP5
#define Z_KEY_KP_6            ::Z::Key::KP6
#define Z_KEY_KP_7            ::Z::Key::KP7
#define Z_KEY_KP_8            ::Z::Key::KP8
#define Z_KEY_KP_9            ::Z::Key::KP9
#define Z_KEY_KP_DECIMAL      ::Z::Key::KPDecimal
#define Z_KEY_KP_DIVIDE       ::Z::Key::KPDivide
#define Z_KEY_KP_MULTIPLY     ::Z::Key::KPMultiply
#define Z_KEY_KP_SUBTRACT     ::Z::Key::KPSubtract
#define Z_KEY_KP_ADD          ::Z::Key::KPAdd
#define Z_KEY_KP_ENTER        ::Z::Key::KPEnter
#define Z_KEY_KP_EQUAL        ::Z::Key::KPEqual

#define Z_KEY_LEFT_SHIFT      ::Z::Key::LeftShift
#define Z_KEY_LEFT_CONTROL    ::Z::Key::LeftControl
#define Z_KEY_LEFT_ALT        ::Z::Key::LeftAlt
#define Z_KEY_LEFT_SUPER      ::Z::Key::LeftSuper
#define Z_KEY_RIGHT_SHIFT     ::Z::Key::RightShift
#define Z_KEY_RIGHT_CONTROL   ::Z::Key::RightControl
#define Z_KEY_RIGHT_ALT       ::Z::Key::RightAlt
#define Z_KEY_RIGHT_SUPER     ::Z::Key::RightSuper
#define Z_KEY_MENU            ::Z::Key::Menu

#endif