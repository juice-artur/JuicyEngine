#pragma once
#include <filesystem>
#include "JuicyEngine/Renderer/Texture.h"

namespace JuicyEngine
{
class ContentBrowserPanel
{
public:
    ContentBrowserPanel();
    void OnImGuiRender();

private:
    std::filesystem::path m_CurrentDirectory;

    Ref<Texture2D> m_DirectoryIcon;
    Ref<Texture2D> m_FileIcon;
};
}  // namespace JuicyEngine