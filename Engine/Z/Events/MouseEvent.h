//
// Created by 32725 on 2023/3/12.
//

#ifndef ENGINE_TUTORIAL_MOUSEEVENT_H
#define ENGINE_TUTORIAL_MOUSEEVENT_H
#include <sstream>

#include "Z/Core/MouseButtonCodes.h"
#include "Z/Events/Event.h"

namespace Z {
	class MouseMovedEvent : public Event {
		float m_X, m_Y;
	public:
		MouseMovedEvent(float x,float y):m_X(x),m_Y(y){}
		inline float GetX()const{return m_X;}
		inline float GetY()const{return m_Y;}
		std::string ToString()const override{
			std::stringstream ss;
			ss<<"MouseMovedEvent:"<<m_X<<":"<<m_Y;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)

		EVENT_CLASS_CATEGORY(EventInput|EventMouse)
	};

	class MouseScrollEvent:public Event{
		float offset_x,offset_y;
	public:
		MouseScrollEvent(float x_offset,float y_offset):offset_x(x_offset),offset_y(y_offset){}
		std::string ToString()const override{
			std::stringstream ss;
			ss<<"MouseScrollEvent:"<<offset_x<<":"<<offset_y;
			return ss.str();
		}
		inline float GetXOffset()const{return offset_x;}
		inline float GetYOffset()const{return offset_y;}

		EVENT_CLASS_TYPE(MouseScrolled)

		EVENT_CLASS_CATEGORY(EventMouse|EventInput)
	};

	class MouseButtonEvent:public Event{
	protected:
		MouseButtonEvent(MouseCode _button):button(_button){}
		MouseCode button;

	public:
		inline MouseCode GetButton()const{return button;}
		EVENT_CLASS_CATEGORY(EventMouseButton|EventInput)
	};

	class MouseButtonPressedEvent:public MouseButtonEvent{
	public:
		MouseButtonPressedEvent(MouseCode _code): MouseButtonEvent(_code){}
		std::string ToString()const override{
			std::stringstream ss;
			ss<<"MouseButtonPressed:"<<button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent:public MouseButtonEvent{
	public:
		MouseButtonReleasedEvent(MouseCode _button): MouseButtonEvent(_button){}
		std::string ToString()const override{
			std::stringstream ss;
			ss<<"MouseButtonReleased:"<<button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};

}


#endif //ENGINE_TUTORIAL_MOUSEEVENT_H
