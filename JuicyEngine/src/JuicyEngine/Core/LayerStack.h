
#include "JuicyEngine/Core/Layer.h"
#include <vector>
namespace JuicyEngine
{
class  LayerStack
{
public:
    LayerStack();
    ~LayerStack();
    /**
     * @brief Adds a layer to the stack.
     *
     * Adds the layer at the position before any overlays in the stack.
     * This layer will be rendered or processed before overlays but after previously pushed layers.
     *
     * @param layer A pointer to the layer to be added.
     */
    void PushLayer(Layer* layer);
    /**
     * @brief Adds an overlay to the stack.
     *
     * Overlays are always rendered or processed after all regular layers.
     * This method adds the overlay at the end of the stack, ensuring it stays on top.
     *
     * @param overlay A pointer to the overlay to be added.
     */
    void PushOverlay(Layer* overlay);
    /**
     * @brief Removes a layer from the stack.
     *
     * on the overlay before removing it.
     *
     * @param overlay A pointer to the overlay to be removed.
     */
    void PopLayer(Layer* layer);
    /**
     * @brief Removes an overlay from the stack.
     *
     * on the overlay before removing it.
     *
     * @param overlay A pointer to the overlay to be removed.
     */
    void PopOverlay(Layer* overlay);
    std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
    std::vector<Layer*>::iterator end() { return m_Layers.end(); }
    std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
    std::vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }
    std::vector<Layer*>::const_iterator begin() const { return m_Layers.begin(); }
    std::vector<Layer*>::const_iterator end() const { return m_Layers.end(); }
    std::vector<Layer*>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
    std::vector<Layer*>::const_reverse_iterator rend() const { return m_Layers.rend(); }

private:
    std::vector<Layer*> m_Layers;
    unsigned int m_LayerInsertIndex = 0;
};
}  // namespace JuicyEngine