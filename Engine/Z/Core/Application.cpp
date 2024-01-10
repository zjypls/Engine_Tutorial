//
// Created by 32725 on 2023/3/11.
//
#include <filesystem>

#include "Z/Core/Application.h"
#include "Z/Core/Log.h"
#include "Z/Core/Input.h"
#include "Z/Core/Time.h"
#include "Z/Core/KeyCodes.h"
#include "Z/Core/AssetsSystem.h"
#include "Z/Events/ApplicationEvent.h"
#include "Z/Renderer/Renderer.h"
#include "Z/Script/ScriptEngine.h"

namespace Z {
	Application *Application::application = nullptr;

	Application::Application(const ApplicationSpec &spec) : Spec(spec) {
		Z_CORE_ASSERT(!application, "Application already exists!");
		application = this;
		Z_CORE_WARN("Current path: {0}!", Spec.commandArgs.Args[0]);
		if (!Spec.RootPath.empty())std::filesystem::current_path(Spec.RootPath);

		window = Z::Scope<zWindow>(zWindow::Create(WindowProps(Spec.Name)));
		window->SetEventCallFunc(Z_BIND_EVENT_FUNC(Application::EventCall));
		AssetsSystem::PreInit();

		ScriptEngine::Init();
		Renderer::Init();

		window->SetVSync(false);
		imguiLayer = new ImGuiLayer();
		PushLayer(imguiLayer);
	}


	Application::~Application() {
		window->Shutdown();
		Renderer::ShutDown();
		if(ScriptEngine::GetContext()!=nullptr){
			Z_CORE_WARN("Close Application in Running!!!");
		}
		ScriptEngine::ShutDown();

		Z_CORE_INFO("Application closed!");
	}

	void Application::Run() {
		while (Running) {
			Time::Update();
			imguiLayer->Begin();
			if (!MinSize) {
				for (Layer *layer: LayerStack) {
					layer->OnUpdate();
				}
			}
			for (Layer *layer: LayerStack) {
				layer->OnImGuiRender();
			}
			imguiLayer->End();

			for (auto &func: FuncQueue) {
				func();
			}
			FuncQueue.clear();

			window->Update();
		}
		window->GetContext()->DeviceSynchronize();
		LayerStack.PopAllLayer();
		AssetsSystem::Destroy();
	}

	void Application::EventCall(Event &e) {
		EventDispatcher dispatcher(e);
		dispatcher.Handle<WindowCloseEvent>(Z_BIND_EVENT_FUNC(Application::OnWindowClose));
		dispatcher.Handle<WindowResizeEvent>(Z_BIND_EVENT_FUNC(Application::OnWindowResize));
		for (auto it = LayerStack.end(); it != LayerStack.begin();) {
			(*--it)->OnEvent(e);
			if (e.Handled) {
				break;
			}
		}
	}

	void Application::PushLayer(Layer *layer) {
		LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer *overlay) {
		LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	Application &Application::Get() {
		return *application;
	}

	bool Application::OnWindowClose(WindowCloseEvent &e) {
		Running = false;
		Z_CORE_INFO("{0}", e);
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent &e) {
		if (e.GetWidth() == 0 || e.GetHeight() == 0) {
			MinSize = true;
			return false;
		}
		MinSize = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		return false;
	}

}
