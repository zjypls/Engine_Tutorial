//
// Created by 32725 on 2023/3/11.
//

#ifndef ENGINE_TUTORIAL_APPLICATION_H
#define ENGINE_TUTORIAL_APPLICATION_H
#include "Z/Core/zWindow.h"
#include "Z/Core/LayerStacks.h"
#include "Z/Events/Event.h"
#include "Z/Renderer/OrithGraphicCamera.h"
#include "Z/Scene/Scene.h"

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
    public:
        Application(const ApplicationSpec&spec);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void EventCall(Event&);
	    void Run();
        virtual ~Application();
		inline void Close(){Running=false;}
		inline ApplicationSpec& GetSpec(){return Spec;}
		static Application& Get();
		inline zWindow& GetWindow(){return *application->window;}
		// push a function to the queue to execute at the end of the frame
		inline void SubmitFunc(const std::function<void()>& func){std::scoped_lock<std::mutex> lock(QueueMutex);FuncQueue.push_back(func);}
	private:
        static Application* application;

        // an events queue execute at the end per frame
		std::vector<std::function<void()>> FuncQueue;
		std::mutex QueueMutex;

        ApplicationSpec Spec;
        Scope<zWindow> window;
        bool Running=true;
        bool MinSize=false;
        LayerStacks LayerStack;
        Ref<Scene> scene;

        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);

    };
	Application* GetApplication(const CommandArgs&);

}


#endif //ENGINE_TUTORIAL_APPLICATION_H
