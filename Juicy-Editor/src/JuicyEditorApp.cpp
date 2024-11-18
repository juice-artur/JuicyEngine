#include <JuicyEngine.h>
#include <JuicyEngine/Core/EntryPoint.h>
#include "EditorLayer.h"
namespace JuicyEngine
{
class JuicyEditor : public Application
{
public:
    JuicyEditor() : Application("Juicy Editor") { PushLayer(new EditorLayer()); }
    ~JuicyEditor() {}
};
Application* CreateApplication()
{
    return new JuicyEditor();
}
}  // namespace JuicyEngine