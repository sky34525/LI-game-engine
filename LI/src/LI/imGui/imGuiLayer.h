#pragma once

#include "LI/Layer.h"


#include "LI/Events/ApplicationEvent.h"
#include "LI/Events/KeyEvent.h"
#include "LI/Events/MouseEvent.h"

namespace LI {
	class LI_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();

	private:
		float m_Time;
		bool m_BlockEvents = true;
	};
}
