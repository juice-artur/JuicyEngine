#pragma once
#include "JuicyEngine/Core/Core.h"
#include "JuicyEngine/Scene/Scene.h"
#include "JuicyEngine/Scene/Entity.h"
namespace JuicyEngine
{
class SceneHierarchyPanel
{
public:
    SceneHierarchyPanel() = default;
    SceneHierarchyPanel(const Ref<Scene>& scene);
    void SetContext(const Ref<Scene>& scene);
    void OnImGuiRender();
    Entity GetSelectedEntity() const { return m_SelectionContext; }
    void SetSelectedEntity(Entity entity);

private:
    void DrawEntityNode(Entity entity);
    void DrawComponents(Entity entity);

private:
    Ref<Scene> m_Context;
    Entity m_SelectionContext;
};
}  // namespace JuicyEngine