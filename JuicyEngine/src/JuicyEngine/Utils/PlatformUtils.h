#pragma once
#include <string>
#include <optional>

namespace JuicyEngine
{
class FileDialogs
{
public:
    // These return empty strings if cancelled
    static std::string OpenFile(const char* filter);
    static std::string SaveFile(const char* filter);
};
}  // namespace JuicyEngine