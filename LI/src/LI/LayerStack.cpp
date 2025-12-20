#include "pch.h"
#include "LayerStack.h"

namespace LI {
	LayerStack::LayerStack()
		:m_LayerInsert(m_Layers.begin())
	{
	}

	LayerStack::~LayerStack()
	{
	}

	void LayerStack::PushLayer(std::unique_ptr<Layer> layer)
	{
		m_LayerInsert = m_Layers.emplace(m_LayerInsert, std::move(layer));
	}

	void LayerStack::PushOverlay(std::unique_ptr<Layer> overlay)
	{
		m_Layers.emplace_back(std::move(overlay));
	}

	std::unique_ptr<Layer> LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find_if(m_Layers.begin(), m_Layers.end(),
			[layer](const std::unique_ptr<Layer>& p) {return p.get() == layer; }
			);
		if (it != m_Layers.end())
		{
			auto result = std::move(*it);
			m_Layers.erase(it);
			m_LayerInsert--;
			return result;
		}
		return nullptr;
	}

	std::unique_ptr<Layer> LayerStack::PopOverlay(Layer* layer)
	{
		auto it = std::find_if(m_Layers.begin(), m_Layers.end(),
			[layer](const std::unique_ptr<Layer>& p) {return p.get() == layer; }
		);
		if (it != m_Layers.end())
		{
			auto result = std::move(*it);
			m_Layers.erase(it);
			return result;
		}
		return nullptr;
	}
}