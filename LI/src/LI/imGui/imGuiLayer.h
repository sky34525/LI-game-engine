#pragma once

#include "LI/Layer.h"

namespace LI {
	class LI_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& evnet);
	private:
		float m_Time;
	};
}
