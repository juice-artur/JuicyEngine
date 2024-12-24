#include <JuicyEngine.h>
#include <JuicyEngine/Core/EntryPoint.h>
#include "EditorLayer.h"
namespace JuicyEngine
{
class JuicyEditor : public Application
{
public:
    JuicyEditor(ApplicationCommandLineArgs args) : Application("Juicy Editor") { PushLayer(new EditorLayer()); }
    ~JuicyEditor() {}
};
Application* CreateApplication(ApplicationCommandLineArgs args)
{
    return new JuicyEditor(args);
}
}  // namespace JuicyEngine