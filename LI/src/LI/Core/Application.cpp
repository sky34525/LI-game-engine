#include "pch.h"
#include "Application.h"
#include "log.h"
#include "Input.h"
#include "LI/Renderer/Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace LI{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		LI_CORE_ASSERT(!s_Instance, "Application alreade exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		Renderer::Init();

		auto imGuiLayer = std::make_unique<ImGuiLayer>();
		m_ImGuiLayer = imGuiLayer.get();
		PushOverlay(std::move(imGuiLayer));

	}

	void Application::PushLayer(std::unique_ptr<Layer> layer)
	{
		m_LayerStack.PushLayer(move(layer));

	}
	void Application::PushOverlay(std::unique_ptr<Layer> layer)
	{
		m_LayerStack.PushOverlay(move(layer));
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));


		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.m_Handled)
				break;
		}
	}


	void Application::Run()
	{
		while (m_Running)
		{
			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;
			if (!m_Minimized)
			{
				for (auto& layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}
			m_ImGuiLayer->Begin();
			for (auto& layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();
			m_Window->OnUpdate();
		};
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}


	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}
} // namespace LI