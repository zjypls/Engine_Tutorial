//
// Created by 32725 on 2023/3/12.
//

#ifndef ENGINE_TUTORIAL_EVENT_H
#define ENGINE_TUTORIAL_EVENT_H
#include "Z/Core/Core.h"
#include <iostream>
#include <string>
#include <functional>

namespace Z{
	enum class EventType{
		None=0,
		WindowClose,WindowResize,WindowFocus,WindowLostFocus,WindowMoved,
		AppTick,AppUpdate,AppRender,
		KeyPressed,KeyReleased,KeyTyped,
		MouseButtonPressed,MouseButtonReleased,MouseMoved,MouseScrolled
	};
	enum EventCategory{
		None=0,
		EventApplication=BIT(1),
		EventInput=BIT(2),
		EventKeyBoard=BIT(3),
		EventMouse=BIT(4),
		EventMouseButton=BIT(5)
	};

	#define EVENT_CLASS_TYPE(_type) static EventType GetStaticType(){return EventType::_type;} \
									virtual EventType GetEventType()const override{return GetStaticType();}\
									virtual const char*GetName()const override{return #_type;}
	#define EVENT_CLASS_CATEGORY(_category) virtual int GetCategoryFlags()const override{return _category;}

	class Z_API Event{
		friend class EventDispatcher;
	public:
		bool Handled=false;
		virtual ~Event()=default;
		virtual EventType GetEventType()const=0;
		virtual const char*GetName()const=0;
		virtual int GetCategoryFlags()const=0;

		virtual std::string ToString()const{return GetName();}
		inline bool isCategory(EventCategory category){return GetCategoryFlags()&category;}
		//inline bool isHandled()const{return Handled;}
	};

	class Z_API EventDispatcher{
		template<class T>
		using EventProc=std::function<bool(T&)>;
	public:
		EventDispatcher(Event&_event):event(_event){}
		template<class T>
		bool Handle(EventProc<T> func)const{
			if(T::GetStaticType()==event.GetEventType()){
				event.Handled=func(*(T*)&event);
				return true;
			}
			return false;
		}
	private:
		Event& event;
	};

	inline std::ostream& operator<<(std::ostream&stream,const Event& event){
		return stream<<event.ToString();
	}

}



#endif //ENGINE_TUTORIAL_EVENT_H
