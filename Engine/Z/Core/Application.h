//
// Created by 32725 on 2023/3/11.
//

#ifndef ENGINE_TUTORIAL_APPLICATION_H
#define ENGINE_TUTORIAL_APPLICATION_H
#include "Core.h"
#include<memory>
#include "Z/Events/Event.h"
#include "zWindow.h"
#include "LayerStacks.h"
#include "Z/ImGui/ImGuiLayer.h"
#include "Z/Renderer/Shader.h"
#include "Z/Renderer/Buffer.h"
#include "Z/Renderer/VertexArray.h"
#include"Z/Renderer/OrithGraphicCamera.h"
namespace Z {
    class Z_API Application {
		Z::Scope<zWindow> window;
		bool Running=true;
		bool MinSize=false;
		LayerStacks LayerStack;
		ImGuiLayer* imguiLayer;

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

    public:
        Application(const std::string&name="Z App");
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		inline ImGuiLayer* GetImGuiLayer(){return imguiLayer;}
		void EventCall(Event&);
	    void Run();
        virtual ~Application();
		inline void Close(){Running=false;}
		static Application& Get();
		inline zWindow& GetWindow(){return *application->window;}
	private:
		static Application* application;

    };
	Application* GetApplication();

}


#endif //ENGINE_TUTORIAL_APPLICATION_H
