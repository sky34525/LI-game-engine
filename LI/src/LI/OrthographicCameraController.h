#pragma once

#include "LI/Renderer/OrthographicCamera.h"
#include "LI/Core/Timestep.h"

#include "LI/Events/ApplicationEvent.h"
#include "LI/Events/MouseEvent.h"

namespace LI {
		class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotation = false);
		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);
		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }
		//void Resize(float width, float height);
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthographicCamera m_Camera;
		bool m_Rotation;
		
		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f; // In degrees, in the anti-clockwise direction
		float m_CameraMoveSpeed = 1.0f;
		float m_CameraRotationSpeed = 180.0f;
	};

}