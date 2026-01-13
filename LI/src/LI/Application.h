#pragma once

#include "Core.h"

#include "LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Window.h"
#include "LI/ImGui/ImGuiLayer.h"
#include "LI/Renderer/Shader.h"
#include "LI/Renderer/Buffer.h"
#include "LI/Renderer/VertexArray.h"
#include "LI/Renderer/Renderer.h"

namespace LI
{
	class WindowCloseEvent;

	class LI_API Application {
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

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;


	protected:
		Application();
	private:
		static Application* s_Instance;
	};

	Application* CreateApplication();
}