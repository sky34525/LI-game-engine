#pragma once

#include "Core.h"

#include "LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Window.h"
#include "LI/ImGui/ImGuiLayer.h"
#include "LI/Renderer/Shader.h"
#include "LI/Renderer/Buffer.h"

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
		std::unique_ptr<Layer> PopLayer();

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }

		//TODO
		unsigned int m_VertexArray;
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;

	protected:
		Application();
	private:
		static Application* s_Instance;
	};

	Application* CreateApplication();
}