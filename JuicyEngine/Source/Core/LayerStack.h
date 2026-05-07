#pragma once

#include "Layer.h"

#include <vector>

namespace JuicyEngine
{

class LayerStack
{
public:
    LayerStack();
    ~LayerStack();

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* overlay);
    void PopLayer(Layer* layer);
    void PopOverlay(Layer* overlay);
    void Cleanup();

    std::vector<Layer*>::iterator begin()
    {
        return m_Layers.begin();
    }
    std::vector<Layer*>::iterator end()
    {
        return m_Layers.begin() + m_LayerInsert;
    }

private:
    std::vector<Layer*> m_Layers;
    size_t m_LayerInsert = 0;
};

} // namespace JuicyEngine