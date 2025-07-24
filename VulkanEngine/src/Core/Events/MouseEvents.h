#pragma once
#include "Event.h"
#include "VulkanEngine/Input/Mousecode.h"

namespace CHIKU
{
	class MouseButtonEvent : public Event
	{
	public:
		MouseButtonEvent(MouseCode button) : m_Button(button) {}
		MouseCode GetMouseCode() const { return m_Button; }

		int GetCategoryFlags() const override { return EventCategoryMouse | EventCategoryInput; }

	protected:
		MouseCode m_Button;
	};

	class MousePressedEvent : public MouseButtonEvent
	{
	public:
		MousePressedEvent(MouseCode button) : MouseButtonEvent(button) {}

		EventType GetEventType() const override { return EventType::MouseButtonPressed; }
		const char* GetName() const override { return "MousePressed"; }
	};

	class MouseReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseReleasedEvent(MouseCode button) : MouseButtonEvent(button) {}

		EventType GetEventType() const override { return EventType::MouseButtonReleased; }
		const char* GetName() const override { return "MouseReleased"; }
	};

	class MouseMovedEvent : public MouseButtonEvent
	{
	public:
		MouseMovedEvent(MouseCode button) : MouseButtonEvent(button) {}

		EventType GetEventType() const override { return EventType::MouseMoved; }
		const char* GetName() const override { return "MouseMoved"; }
	};

	class MouseScrolledEvent : public MouseButtonEvent
	{
	public:
		MouseScrolledEvent(MouseCode button) : MouseButtonEvent(button) {}

		EventType GetEventType() const override { return EventType::MouseScrolled; }
		const char* GetName() const override { return "MouseScrolled"; }
	};
}