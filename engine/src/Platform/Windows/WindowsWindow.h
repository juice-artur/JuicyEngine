#pragma once

#include "Platform\Common\BaseWindow.h"
#include "Platform\Common\WindowEvent.h"
#include "CoreDefines.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_system.h"
class WindowsWindow : public BaseWindow
{
public:
	virtual WindowHandle_t GetHandle() const override;
	~WindowsWindow();
	WindowsWindow();

	// Inherited via CBaseWindow
	bool Create(const char* InTitle, unsigned int InWidth, unsigned int InHeight, unsigned int InFlags = SW_Default) override;
	void Close() override;
	void ShowCursor(bool InIsShowCursor) override;
	void ShowWindow(bool InIsShowWindow) override;
	void Maximize() override;
	void Minimize() override;
	bool PollEvent(WindowEvent& OutWindowEvent) const override;
	void SetTitle(const char* InTitle) override;
	void SetSize(unsigned int InWidth, unsigned int InHeight) override;
	void SetFullscreen(bool InIsFullscreen) override;
	bool IsOpen() const override;
	bool IsShowingCursor() const override;
	bool IsFullscreen() const override;
	void GetSize(unsigned int& OutWidth, unsigned int& OutHeight) const override;

private:
	bool bIsShowCursor;	   /**< Is showed cursor */
	bool bIsFullscreen;	   /**< Is enabled fullscreen mode */
	SDL_Window* sdlWindow; /**< SDL window */
	HANDLE handle;		   /**< OS handle on window */
};