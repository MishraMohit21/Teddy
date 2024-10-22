#pragma once

namespace Teddy
{
	typedef enum class KeyCode : uint16_t
	{
		// From glfw3.h
		Space = 32,
		Apostrophe = 39, /* ' */
		Comma = 44, /* , */
		Minus = 45, /* - */
		Period = 46, /* . */
		Slash = 47, /* / */

		D0 = 48, /* 0 */
		D1 = 49, /* 1 */
		D2 = 50, /* 2 */
		D3 = 51, /* 3 */
		D4 = 52, /* 4 */
		D5 = 53, /* 5 */
		D6 = 54, /* 6 */
		D7 = 55, /* 7 */
		D8 = 56, /* 8 */
		D9 = 57, /* 9 */

		Semicolon = 59, /* ; */
		Equal = 61, /* = */

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

		LeftBracket = 91,  /* [ */
		Backslash = 92,  /* \ */
		RightBracket = 93,  /* ] */
		GraveAccent = 96,  /* ` */

		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

		/* Function keys */
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
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
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

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	} Key;

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}

// From glfw3.h
#define TD_KEY_SPACE           ::Teddy::Key::Space
#define TD_KEY_APOSTROPHE      ::Teddy::Key::Apostrophe    /* ' */
#define TD_KEY_COMMA           ::Teddy::Key::Comma         /* , */
#define TD_KEY_MINUS           ::Teddy::Key::Minus         /* - */
#define TD_KEY_PERIOD          ::Teddy::Key::Period        /* . */
#define TD_KEY_SLASH           ::Teddy::Key::Slash         /* / */
#define TD_KEY_0               ::Teddy::Key::D0
#define TD_KEY_1               ::Teddy::Key::D1
#define TD_KEY_2               ::Teddy::Key::D2
#define TD_KEY_3               ::Teddy::Key::D3
#define TD_KEY_4               ::Teddy::Key::D4
#define TD_KEY_5               ::Teddy::Key::D5
#define TD_KEY_6               ::Teddy::Key::D6
#define TD_KEY_7               ::Teddy::Key::D7
#define TD_KEY_8               ::Teddy::Key::D8
#define TD_KEY_9               ::Teddy::Key::D9
#define TD_KEY_SEMICOLON       ::Teddy::Key::Semicolon     /* ; */
#define TD_KEY_EQUAL           ::Teddy::Key::Equal         /* = */
#define TD_KEY_A               ::Teddy::Key::A
#define TD_KEY_B               ::Teddy::Key::B
#define TD_KEY_C               ::Teddy::Key::C
#define TD_KEY_D               ::Teddy::Key::D
#define TD_KEY_E               ::Teddy::Key::E
#define TD_KEY_F               ::Teddy::Key::F
#define TD_KEY_G               ::Teddy::Key::G
#define TD_KEY_H               ::Teddy::Key::H
#define TD_KEY_I               ::Teddy::Key::I
#define TD_KEY_J               ::Teddy::Key::J
#define TD_KEY_K               ::Teddy::Key::K
#define TD_KEY_L               ::Teddy::Key::L
#define TD_KEY_M               ::Teddy::Key::M
#define TD_KEY_N               ::Teddy::Key::N
#define TD_KEY_O               ::Teddy::Key::O
#define TD_KEY_P               ::Teddy::Key::P
#define TD_KEY_Q               ::Teddy::Key::Q
#define TD_KEY_R               ::Teddy::Key::R
#define TD_KEY_S               ::Teddy::Key::S
#define TD_KEY_T               ::Teddy::Key::T
#define TD_KEY_U               ::Teddy::Key::U
#define TD_KEY_V               ::Teddy::Key::V
#define TD_KEY_W               ::Teddy::Key::W
#define TD_KEY_X               ::Teddy::Key::X
#define TD_KEY_Y               ::Teddy::Key::Y
#define TD_KEY_Z               ::Teddy::Key::Z
#define TD_KEY_LEFT_BRACKET    ::Teddy::Key::LeftBracket   /* [ */
#define TD_KEY_BACKSLASH       ::Teddy::Key::Backslash     /* \ */
#define TD_KEY_RIGHT_BRACKET   ::Teddy::Key::RightBracket  /* ] */
#define TD_KEY_GRAVE_ACCENT    ::Teddy::Key::GraveAccent   /* ` */
#define TD_KEY_WORLD_1         ::Teddy::Key::World1        /* non-US #1 */
#define TD_KEY_WORLD_2         ::Teddy::Key::World2        /* non-US #2 */

/* Function keys */
#define TD_KEY_ESCAPE          ::Teddy::Key::Escape
#define TD_KEY_ENTER           ::Teddy::Key::Enter
#define TD_KEY_TAB             ::Teddy::Key::Tab
#define TD_KEY_BACKSPACE       ::Teddy::Key::Backspace
#define TD_KEY_INSERT          ::Teddy::Key::Insert
#define TD_KEY_DELETE          ::Teddy::Key::Delete
#define TD_KEY_RIGHT           ::Teddy::Key::Right
#define TD_KEY_LEFT            ::Teddy::Key::Left
#define TD_KEY_DOWN            ::Teddy::Key::Down
#define TD_KEY_UP              ::Teddy::Key::Up
#define TD_KEY_PAGE_UP         ::Teddy::Key::PageUp
#define TD_KEY_PAGE_DOWN       ::Teddy::Key::PageDown
#define TD_KEY_HOME            ::Teddy::Key::Home
#define TD_KEY_END             ::Teddy::Key::End
#define TD_KEY_CAPS_LOCK       ::Teddy::Key::CapsLock
#define TD_KEY_SCROLL_LOCK     ::Teddy::Key::ScrollLock
#define TD_KEY_NUM_LOCK        ::Teddy::Key::NumLock
#define TD_KEY_PRINT_SCREEN    ::Teddy::Key::PrintScreen
#define TD_KEY_PAUSE           ::Teddy::Key::Pause
#define TD_KEY_F1              ::Teddy::Key::F1
#define TD_KEY_F2              ::Teddy::Key::F2
#define TD_KEY_F3              ::Teddy::Key::F3
#define TD_KEY_F4              ::Teddy::Key::F4
#define TD_KEY_F5              ::Teddy::Key::F5
#define TD_KEY_F6              ::Teddy::Key::F6
#define TD_KEY_F7              ::Teddy::Key::F7
#define TD_KEY_F8              ::Teddy::Key::F8
#define TD_KEY_F9              ::Teddy::Key::F9
#define TD_KEY_F10             ::Teddy::Key::F10
#define TD_KEY_F11             ::Teddy::Key::F11
#define TD_KEY_F12             ::Teddy::Key::F12
#define TD_KEY_F13             ::Teddy::Key::F13
#define TD_KEY_F14             ::Teddy::Key::F14
#define TD_KEY_F15             ::Teddy::Key::F15
#define TD_KEY_F16             ::Teddy::Key::F16
#define TD_KEY_F17             ::Teddy::Key::F17
#define TD_KEY_F18             ::Teddy::Key::F18
#define TD_KEY_F19             ::Teddy::Key::F19
#define TD_KEY_F20             ::Teddy::Key::F20
#define TD_KEY_F21             ::Teddy::Key::F21
#define TD_KEY_F22             ::Teddy::Key::F22
#define TD_KEY_F23             ::Teddy::Key::F23
#define TD_KEY_F24             ::Teddy::Key::F24
#define TD_KEY_F25             ::Teddy::Key::F25

/* Keypad */
#define TD_KEY_KP_0            ::Teddy::Key::KP0
#define TD_KEY_KP_1            ::Teddy::Key::KP1
#define TD_KEY_KP_2            ::Teddy::Key::KP2
#define TD_KEY_KP_3            ::Teddy::Key::KP3
#define TD_KEY_KP_4            ::Teddy::Key::KP4
#define TD_KEY_KP_5            ::Teddy::Key::KP5
#define TD_KEY_KP_6            ::Teddy::Key::KP6
#define TD_KEY_KP_7            ::Teddy::Key::KP7
#define TD_KEY_KP_8            ::Teddy::Key::KP8
#define TD_KEY_KP_9            ::Teddy::Key::KP9
#define TD_KEY_KP_DECIMAL      ::Teddy::Key::KPDecimal
#define TD_KEY_KP_DIVIDE       ::Teddy::Key::KPDivide
#define TD_KEY_KP_MULTIPLY     ::Teddy::Key::KPMultiply
#define TD_KEY_KP_SUBTRACT     ::Teddy::Key::KPSubtract
#define TD_KEY_KP_ADD          ::Teddy::Key::KPAdd
#define TD_KEY_KP_ENTER        ::Teddy::Key::KPEnter
#define TD_KEY_KP_EQUAL        ::Teddy::Key::KPEqual

#define TD_KEY_LEFT_SHIFT      ::Teddy::Key::LeftShift
#define TD_KEY_LEFT_CONTROL    ::Teddy::Key::LeftControl
#define TD_KEY_LEFT_ALT        ::Teddy::Key::LeftAlt
#define TD_KEY_LEFT_SUPER      ::Teddy::Key::LeftSuper
#define TD_KEY_RIGHT_SHIFT     ::Teddy::Key::RightShift
#define TD_KEY_RIGHT_CONTROL   ::Teddy::Key::RightControl
#define TD_KEY_RIGHT_ALT       ::Teddy::Key::RightAlt
#define TD_KEY_RIGHT_SUPER     ::Teddy::Key::RightSuper
#define TD_KEY_MENU            ::Teddy::Key::Menu