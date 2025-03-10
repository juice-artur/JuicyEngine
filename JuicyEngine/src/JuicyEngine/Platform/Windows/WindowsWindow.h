#pragma once

#include "JuicyEngine/Core/Window.h"
#include "JuicyEngine/Events/ApplicationEvent.h"
#include "JuicyEngine/Events/MouseEvent.h"
#include "JuicyEngine/Events/KeyEvent.h"
#include <JuicyEngine/Renderer/GraphicsContext.h>

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
    // Window attributes
    inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
    virtual void* GetNativeWindow() const;
    void SetVSync(bool enabled) override;
    bool IsVSync() const override;

private:
    virtual void Init(const WindowProps& props);
    virtual void Shutdown();
    void ProcessMessages();
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    HWND m_Window;
    HINSTANCE m_HInstance;

    struct WindowData
    {
        std::string Title;
        unsigned int Width, Height;
        bool VSync;
        EventCallbackFn EventCallback;
    };
    WindowData m_Data;
};
};  // namespace JuicyEngine
