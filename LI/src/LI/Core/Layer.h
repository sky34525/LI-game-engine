#pragma once

#include "LI/Core/Core.h"
#include "LI/Events/Event.h"
#include "LI/Core/Timestep.h"


namespace LI {

	class LI_API Layer
	{
	public:
		Layer(const std::string& name = "layer");
		virtual ~Layer() = default;

		virtual void OnAttach(){}
		virtual void OnDetach(){}
		virtual void OnUpdate(Timestep ts){}
		virtual void OnEvent(Event& event){}
		virtual void OnImGuiRender() {}

		inline const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};

}
