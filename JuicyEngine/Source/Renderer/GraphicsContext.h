#pragma once

namespace JuicyEngine
{
	class GraphicsContext
	{
	public:
		virtual void Init(void* Window) = 0;
		virtual void SwapBuffers() = 0;
		virtual void Shutdown() = 0;
		virtual void Draw() = 0;
		virtual ~GraphicsContext() {};
	};
} // namespace JuicyEngine