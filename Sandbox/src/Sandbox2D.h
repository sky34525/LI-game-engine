#pragma once

#include "LI.h"

class Sandbox2D : public LI::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(LI::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(LI::Event& e) override;
private:
	LI::OrthographicCameraController m_CameraController;

	// Temp
	LI::Ref<LI::VertexArray> m_SquareVA;
	LI::Ref<LI::Shader> m_FlatColorShader;
	LI::Ref<LI::Texture2D> m_CheckerboardTexture;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};