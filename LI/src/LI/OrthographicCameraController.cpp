#include "pch.h"
#include "OrthographicCameraController.h"

#include "LI/Input.h"
#include "LI/KeyCodes.h"

namespace LI {
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		:m_AspectRatio(aspectRatio), 
		m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
		m_Rotation(rotation)
	{
	}

	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		if (LI::Input::IsKeyPressed(LI_KEY_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		else if (LI::Input::IsKeyPressed(LI_KEY_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed * ts;

		if (LI::Input::IsKeyPressed(LI_KEY_UP))
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		else if (LI::Input::IsKeyPressed(LI_KEY_DOWN))
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;

		if(m_Rotation)
		{
			if (LI::Input::IsKeyPressed(LI_KEY_A))
				m_CameraRotation += m_CameraRotationSpeed * ts;
			if (LI::Input::IsKeyPressed(LI_KEY_D))
				m_CameraRotation -= m_CameraRotationSpeed * ts;
			m_Camera.SetRotation(m_CameraRotation);
		}

		m_Camera.SetPosition(m_CameraPosition);
		m_CameraMoveSpeed = m_ZoomLevel;
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(LI_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(LI_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	//void OrthographicCameraController::Resize(float width, float height)
	//{
	//}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}
}
