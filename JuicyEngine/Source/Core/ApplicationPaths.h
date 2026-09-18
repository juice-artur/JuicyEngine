#pragma once

#include <string>

namespace JuicyEngine
{
// Resolves paths to bundled application assets.
class ApplicationPaths
{
public:
    // On Apple platforms assets live inside the application bundle, so the
    // returned path is resolved relative to SDL_GetBasePath(). On other
    // platforms the asset path is used relative to the working directory.
    static std::string GetAssetPath(const std::string& AssetRelativePath);
};
} // namespace JuicyEngine