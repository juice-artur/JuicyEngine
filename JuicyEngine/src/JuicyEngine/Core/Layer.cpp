#include "jepch.h"
#include "Layer.h"

namespace JuicyEngine
{
Layer::Layer(const std::string& debugName) : m_DebugName(debugName) {}
Layer::~Layer() {}
}  // namespace JuicyEngine