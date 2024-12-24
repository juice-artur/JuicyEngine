#pragma once
#include <filesystem>
namespace JuicyEngine
{
class ContentBrowserPanel
{
public:
    ContentBrowserPanel();
    void OnImGuiRender();

private:
    std::filesystem::path m_CurrentDirectory;
};
}  // namespace JuicyEngine