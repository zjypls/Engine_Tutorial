//
// Created by 32725 on 2023/3/12.
//

#ifndef ENGINE_TUTORIAL_KEYEVENT_H
#define ENGINE_TUTORIAL_KEYEVENT_H
#include <sstream>

#include "Z/Core/KeyCodes.h"
#include "Z/Events/Event.h"
namespace Z{
	class KeyEvent:public Event{
	public:
		inline KeyCode GetKey()const{return m_KeyCode;}
		EVENT_CLASS_CATEGORY(EventKeyBoard|EventInput);
	protected:
		KeyEvent(KeyCode _code):m_KeyCode(_code){};
		KeyCode m_KeyCode;
	};

	class KeyPressEvent:public KeyEvent{
		int repeatNum;
	public:
		KeyPressEvent(KeyCode _code,int _repeatCount): KeyEvent(_code),repeatNum(_repeatCount){}
		inline int GetRepeatCount()const{return repeatNum;}
		std::string ToString()const {
			std::stringstream ss;
			ss<<"KeyPressEvent:"<<m_KeyCode<<"("<<repeatNum<<" repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed);
	};

	class KeyReleasedEvent:public KeyEvent{
	public:
		KeyReleasedEvent(KeyCode _code): KeyEvent(_code){}
		std::string ToString()const{
			std::stringstream ss;
			ss<<"KeyReleasedEvent";
			return ss.str();
		}
		EVENT_CLASS_TYPE(KeyReleased);
	};
	class KeyTypedEvent:public KeyEvent{
	public:
		KeyTypedEvent(KeyCode _code): KeyEvent(_code){}
		std::string ToString()const{
			std::stringstream ss;
			ss<<"KeyTypedEvent:"<<m_KeyCode;
			return ss.str();
		}
		EVENT_CLASS_TYPE(KeyTyped);
	};
}

#endif //ENGINE_TUTORIAL_KEYEVENT_H
