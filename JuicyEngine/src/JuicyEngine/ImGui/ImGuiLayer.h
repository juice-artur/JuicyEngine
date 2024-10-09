#pragma once
#include "JuicyEngine/Layer.h"
#include "JuicyEngine/Events/ApplicationEvent.h"
#include <JuicyEngine/Events/KeyEvent.h>
#include <JuicyEngine/Events/MouseEvent.h>
namespace JuicyEngine {
	class JE_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();
		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);
	private:
		float m_Time = 0.0f;
	private:
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);
	};
}