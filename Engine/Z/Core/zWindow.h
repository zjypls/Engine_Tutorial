//
// Created by 32725 on 2023/3/12.
//

#ifndef ENGINE_TUTORIAL_ZWINDOW_H
#define ENGINE_TUTORIAL_ZWINDOW_H
#include "Core.h"
#include "Z/Events/Event.h"

namespace Z{
	struct WindowProps{
		unsigned int width;
		unsigned int height;
		std::string title;
		WindowProps(const std::string&_title="Z Engine",unsigned int _width=1200,unsigned int _height=800)
			:width(_width),height(_height),title(_title){}
	};

	class Z_API zWindow{
	public:
		using EventCallFunc=std::function<void(Event&)>;
		virtual ~zWindow(){};

		inline virtual unsigned int GetWidth()const=0;
		inline virtual unsigned int GetHeight()const=0;

		inline virtual bool IsVSync()=0;
		inline virtual void SetVSync(bool)=0;
		inline virtual void SetEventCallFunc(const EventCallFunc&)=0;
		virtual void Update()=0;
		inline virtual void* GetNativeWindow()=0;

		static zWindow* Create(const WindowProps&props=WindowProps{});
	};
}

#endif //ENGINE_TUTORIAL_ZWINDOW_H
