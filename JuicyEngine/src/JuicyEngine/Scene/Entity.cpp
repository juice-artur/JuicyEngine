#include "jepch.h"
#include "Entity.h"
namespace JuicyEngine
{
Entity::Entity(entt::entity handle, Scene* scene) : m_EntityHandle(handle), m_Scene(scene) {}
}  // namespace JuicyEngine