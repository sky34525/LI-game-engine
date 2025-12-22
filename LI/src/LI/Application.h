#pragma once

#include "Core.h"

#include "LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Window.h"


namespace LI
{
	class WindowCloseEvent;

	class LI_API Application {
	public:

		virtual ~Application();
		void Run();

		void OnEvent(Event& e);

		void PushLayer(std::unique_ptr<Layer> layer);
		void PushOverlay(std::unique_ptr<Layer> layer);

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
	protected:
		Application();
	private:
		static Application* s_Instance;
	};

	Application* CreateApplication();
}