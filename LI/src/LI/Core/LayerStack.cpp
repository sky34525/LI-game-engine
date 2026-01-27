#include "pch.h"
#include "LayerStack.h"

namespace LI {
	LayerStack::LayerStack()
	{
	}

	LayerStack::~LayerStack()
	{
	}

	void LayerStack::PushLayer(std::unique_ptr<Layer> layer)
	{
		layer->OnAttach();
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, std::move(layer));
		m_LayerInsertIndex++;
	}

	void LayerStack::PushOverlay(std::unique_ptr<Layer> overlay)
	{
		overlay->OnAttach();
		m_Layers.emplace_back(std::move(overlay));
	}

	std::unique_ptr<Layer> LayerStack::PopLayer(Layer& layer)
	{
		auto it = std::find_if(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex,
			[&layer](const std::unique_ptr<Layer>& p) {return p.get() == &layer; }
			);
		if (it != m_Layers.begin() + m_LayerInsertIndex)
		{
			(*it)->OnDetach();
			auto result = std::move(*it);
			m_Layers.erase(it);
			m_LayerInsertIndex--;
			return result;
		}
		return nullptr;
	}

	std::unique_ptr<Layer> LayerStack::PopOverlay(Layer& layer)
	{
		auto it = std::find_if(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(),
			[&layer](const std::unique_ptr<Layer>& p) {return p.get() == &layer; }
		);
		if (it != m_Layers.end())
		{
			(*it)->OnDetach();
			auto result = std::move(*it);
			m_Layers.erase(it);
			return result;
		}
		return nullptr;
	}
}