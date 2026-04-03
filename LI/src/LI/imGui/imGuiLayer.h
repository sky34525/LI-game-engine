#pragma once

#include "LI/Core/Layer.h"


#include "LI/Events/ApplicationEvent.h"
#include "LI/Events/KeyEvent.h"
#include "LI/Events/MouseEvent.h"

namespace LI {
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void Begin();
		void End();

	private:
		float m_Time;
		bool m_BlockEvents = true;
	};
}
