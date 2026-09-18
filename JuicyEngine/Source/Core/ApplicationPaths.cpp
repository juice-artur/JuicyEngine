#include "jepch.h"

#include "Core/ApplicationPaths.h"

#include <SDL3/SDL_filesystem.h>

namespace JuicyEngine
{
std::string ApplicationPaths::GetAssetPath(const std::string& AssetRelativePath)
{
#if defined(__APPLE__)
    const char* BasePath = SDL_GetBasePath();
    if (BasePath)
    {
        return std::string(BasePath) + AssetRelativePath;
    }
#endif

    return AssetRelativePath;
}
} // namespace JuicyEngine