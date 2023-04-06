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
	struct CommandArgs{
		int Count;
		char** Args=nullptr;
		std::string operator[](int i){
			Z_CORE_ASSERT(i<Count,"Index out of range");
			return Args[i];
		}
	};
	struct ApplicationSpec{
		std::string Name;
		CommandArgs commandArgs;
		std::string RootPath;
	};
    class Z_API Application {

		ApplicationSpec Spec;
		Z::Scope<zWindow> window;
		bool Running=true;
		bool MinSize=false;
		LayerStacks LayerStack;
		ImGuiLayer* imguiLayer;

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

    public:
        Application(const ApplicationSpec&spec);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		inline ImGuiLayer* GetImGuiLayer(){return imguiLayer;}
		void EventCall(Event&);
	    void Run();
        virtual ~Application();
		inline void Close(){Running=false;}
		inline ApplicationSpec& GetSpec(){return Spec;}
		static Application& Get();
		inline zWindow& GetWindow(){return *application->window;}
	private:
		static Application* application;

    };
	Application* GetApplication(const CommandArgs&);

}


#endif //ENGINE_TUTORIAL_APPLICATION_H
