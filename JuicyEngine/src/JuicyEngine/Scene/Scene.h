#pragma once
#include "entt.hpp"
#include "JuicyEngine/Core/Timestep.h"

namespace JuicyEngine
{
class Scene
{
public:
    Scene();
    ~Scene();
    entt::entity CreateEntity();
    // TEMP
    entt::registry& Reg() { return m_Registry; }
    void OnUpdate(Timestep ts);

private:
    entt::registry m_Registry;
};
}  // namespace JuicyEngine