//
// Created by 32725 on 2023/3/12.
//

#ifndef ENGINE_TUTORIAL_KEYEVENT_H
#define ENGINE_TUTORIAL_KEYEVENT_H

#include "Event.h"
#include <sstream>
namespace Z{
	class KeyEvent:public Event{
	public:
		inline int GetKey()const{return m_KeyCode;}
		EVENT_CLASS_CATEGORY(EventKeyBoard|EventInput);
	protected:
		KeyEvent(int _code):m_KeyCode(_code){};
		int m_KeyCode;
	};

	class KeyPressEvent:public KeyEvent{
		int repeatNum;
	public:
		KeyPressEvent(int _code,int _repeatCount): KeyEvent(_code),repeatNum(_repeatCount){}
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
		KeyReleasedEvent(int _code): KeyEvent(_code){}
		std::string ToString()const{
			std::stringstream ss;
			ss<<"KeyReleasedEvent";
			return ss.str();
		}
		EVENT_CLASS_TYPE(KeyReleased);
	};
	class KeyTypedEvent:public KeyEvent{
	public:
		KeyTypedEvent(int _code): KeyEvent(_code){}
		std::string ToString()const{
			std::stringstream ss;
			ss<<"KeyTypedEvent:"<<m_KeyCode;
			return ss.str();
		}
		EVENT_CLASS_TYPE(KeyTyped);
	};
}

#endif //ENGINE_TUTORIAL_KEYEVENT_H
