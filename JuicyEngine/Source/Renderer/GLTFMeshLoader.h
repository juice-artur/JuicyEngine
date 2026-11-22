#pragma once

#include "Renderer/Mesh.h"

#include <filesystem>
#include <optional>

namespace JuicyEngine
{
class GLTFMeshLoader
{
public:
    static std::optional<Mesh> Load(const std::filesystem::path& Path);
};
} // namespace JuicyEngine