#pragma once


namespace JuicyEngine
{
class Window final
{
public:
    Window();
    ~Window();
    void* GetNativeWindow() const;
private:
    void* WindowPtr;
};
}