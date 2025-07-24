#pragma once

namespace CHIKU
{
    enum class EventType
    {
        None = 0,
        KeyPressed,
        KeyReleased,
        KeyTyped,

        MouseButtonPressed,
        MouseButtonReleased,
        MouseMoved,
        MouseScrolled,

        WindowResize,
        WindowClose
    };

    enum EventCategory
    {
        None = 0,
        EventCategoryInput = 1 << 0,
        EventCategoryKeyboard = 1 << 1,
        EventCategoryMouse = 1 << 2,
        EventCategoryApplication = 1 << 3
    };

    class Event
    {
    public:
        virtual EventType GetEventType() const = 0;
        virtual int GetCategoryFlags() const = 0;
        virtual const char* GetName() const = 0;

        bool Handled = false;

        inline bool IsInCategory(EventCategory category)
        {
            return GetCategoryFlags() & category;
        }
    };
}