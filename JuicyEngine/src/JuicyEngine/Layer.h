#pragma once
#include "JuicyEngine/Core.h"
#include "JuicyEngine/Core/Timestep.h"
#include "JuicyEngine/Events/Event.h"
namespace JuicyEngine
{
class JE_API Layer
{
public:
    Layer(const std::string& name = "Layer");
    virtual ~Layer();
    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(Timestep ts) {}
    virtual void OnImGuiRender() {}
    virtual void OnEvent(Event& event) {}
    inline const std::string& GetName() const { return m_DebugName; }

protected:
    std::string m_DebugName;
};
}  // namespace JuicyEngine