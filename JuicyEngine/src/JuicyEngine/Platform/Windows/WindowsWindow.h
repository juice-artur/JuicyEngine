#pragma once

#include "JuicyEngine/Core/Window.h"

namespace JuicyEngine
{
class WindowsWindow : public Window
{
public:
    WindowsWindow(const WindowProps& props);
    virtual ~WindowsWindow();
    void OnUpdate() override;
    inline unsigned int GetWidth() const override { return m_Data.Width; }
    inline unsigned int GetHeight() const override { return m_Data.Height; }
    inline virtual void* GetNativeWindow() const { return m_Window; }

    // Window attributes
    inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }

private:
    virtual void Init(const WindowProps& props);
    virtual void Shutdown();

    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    HWND m_Window;
    HINSTANCE m_instance;
    struct WindowData
    {
        std::string Title;
        unsigned int Width, Height;
        EventCallbackFn EventCallback;
    };
    WindowData m_Data;
};

};  // namespace JuicyEngine