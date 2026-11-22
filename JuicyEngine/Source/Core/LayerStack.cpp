#include "jepch.h"
#include "LayerStack.h"

namespace JuicyEngine
{

LayerStack::LayerStack()
{
    m_LayerInsert = 0;
}

LayerStack::~LayerStack()
{
    for (Layer* layer : m_Layers)
    {
        delete layer;
    }
}

void LayerStack::PushLayer(Layer* layer)
{
    m_Layers.emplace(layers_end(), layer);
    m_LayerInsert++;
    layer->OnAttach();
}

void LayerStack::PushOverlay(Layer* overlay)
{
    m_Layers.emplace_back(overlay);
    overlay->OnAttach();
}

void LayerStack::PopLayer(Layer* layer)
{
    auto layersEnd = layers_end();
    auto it = std::find(m_Layers.begin(), layersEnd, layer);
    if (it != layersEnd)
    {
        m_Layers.erase(it);
        m_LayerInsert--;
        layer->OnDetach();
    }
}

void LayerStack::PopOverlay(Layer* overlay)
{
    auto it = std::find(layers_end(), m_Layers.end(), overlay);
    if (it != m_Layers.end())
    {
        m_Layers.erase(it);
        overlay->OnDetach();
    }
}

void LayerStack::Cleanup()
{
    for (auto it = m_Layers.rbegin(); it != m_Layers.rend(); ++it)
    {
        (*it)->OnDetach();
        delete *it;
    }
    m_Layers.clear();
    m_LayerInsert = 0;
}

} // namespace JuicyEngine