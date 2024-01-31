//
// Created by 32725 on 2023/3/12.
//

#ifndef ENGINE_TUTORIAL_ZWINDOW_H
#define ENGINE_TUTORIAL_ZWINDOW_H
#include <utility>

#include "Z/Core/Core.h"
#include "Z/Events/Event.h"


namespace Z{
	struct WindowProps{
		uint32 width;
		uint32 height;
		std::string title;
		WindowProps(std::string _title="Z Engine",uint32 _width=1200,uint32 _height=800)
			:width(_width),height(_height),title(std::move(_title)){}
	};

	class Z_API zWindow{
	public:
		using EventCallFunc=std::function<void(Event&)>;
		virtual ~zWindow(){}

		[[nodiscard]] virtual uint32 GetWidth()const=0;
		[[nodiscard]] virtual uint32 GetHeight()const=0;

		virtual void SetEventCallFunc(const EventCallFunc&)=0;
		virtual void Update()=0;
		virtual void* GetNativeWindow()=0;
		virtual void Shutdown()=0;

		static zWindow* Create(const WindowProps&props=WindowProps{});
	};
}

#endif //ENGINE_TUTORIAL_ZWINDOW_H
