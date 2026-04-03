#pragma once

#include "Core.h"

#include "LI/Core/LayerStack.h"
#include "LI/Events/Event.h"
#include "LI/Events/ApplicationEvent.h"
#include "Window.h"
#include "LI/ImGui/ImGuiLayer.h"
#include "LI/Core/Timestep.h"

//Application通过将构造函数设为protected，和一个static Application*实现单例。但是和常规的懒汉和饿汉式单例都有区别
//和饿汉式单例的区别：
//不在静态初始化阶段构造，静态初始化阶段将static Application* = nullptr
//不使用懒汉式的原因：
//EntryPoint.h负责Application的生命周期管理。

namespace LI
{
	class WindowCloseEvent;

	class Application {
	public:

		virtual ~Application() = default;
		void Run();

		void OnEvent(Event& e);

		void PushLayer(std::unique_ptr<Layer> layer);
		void PushOverlay(std::unique_ptr<Layer> layer);

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e); //Viewport Transformation

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;


	protected:
		Application();
	private:
		static Application* s_Instance;
	};

	Application* CreateApplication();
}