//
// Created by 32725 on 2023/3/12.
//

#ifndef ENGINE_TUTORIAL_APPLICATIONEVENT_H
#define ENGINE_TUTORIAL_APPLICATIONEVENT_H
#include "Event.h"
#include <sstream>
namespace Z{
	class WindowResizeEvent:public Event{
		int width,height;
	public:
		inline int GetWidth()const{return width;}
		inline int GetHeight()const{return height;}
		WindowResizeEvent(int _width,int _height):width(_width),height(_height){}
		std::string ToString()const override{
			std::stringstream ss;
			ss<<"WindowResizeEvent:"<<width<<":"<<height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)

		EVENT_CLASS_CATEGORY(EventApplication)
	};
	class WindowCloseEvent:public Event{
	public:
		WindowCloseEvent(){}
		EVENT_CLASS_TYPE(WindowClose)

		EVENT_CLASS_CATEGORY(EventApplication)
	};
	class AppTickEvent:public Event{
	public:
		AppTickEvent(){}

		EVENT_CLASS_TYPE(AppTick)

		EVENT_CLASS_CATEGORY(EventApplication)
	};
	class AppUpdateEvent:public Event{
	public:
		AppUpdateEvent(){}

		EVENT_CLASS_TYPE(AppUpdate)

		EVENT_CLASS_CATEGORY(EventApplication)
	};
	class AppRenderEvent:public Event{
	public:
		AppRenderEvent(){}

		EVENT_CLASS_TYPE(AppRender)

		EVENT_CLASS_CATEGORY(EventApplication)
	};
}


#endif //ENGINE_TUTORIAL_APPLICATIONEVENT_H
