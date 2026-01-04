#pragma once

#include "LI/Core.h"

#include <string>
#include <functional>
#include <spdlog/fmt/fmt.h>


namespace LI {
	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() {return EventType::##type;}\
                               virtual EventType GetEventType() const override {return GetStaticType();}\
                               virtual const char* GetName() const override {return #type;}

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override {return category;}

	class LI_API Event
	{
		friend class EventDispatcher;
	public:
		bool Handled = false;
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }
		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	};
	

	/*
	1. Type checking and conversion: Checks the actual type of the event at runtime; 
	if a match is found, the base class reference is safely cast to the derived class type.

	2. Event handling delegation: Passes the matching event to the appropriate handler function.

	3. Handling status tracking: Marks whether an event has been handled (using the m_Handled flag).
	*/
	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}
		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}

template<>
struct fmt::formatter<LI::Event>
{
	constexpr auto parse(fmt::format_parse_context& ctx) -> decltype(ctx.begin())
	{
		return ctx.begin();
	}

	template<typename FormatContext>
	auto format(const LI::Event& e, FormatContext& ctx) const -> decltype(ctx.out())
	{
		return fmt::format_to(ctx.out(), "{}", e.ToString());
	}

};

// Formatter for all Event-derived types
template<typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_base_of_v<LI::Event, T>, char>>
{
	constexpr auto parse(fmt::format_parse_context& ctx) -> decltype(ctx.begin())
	{
		return ctx.begin();
	}

	template<typename FormatContext>
	auto format(const T& e, FormatContext& ctx) const -> decltype(ctx.out())
	{
		return fmt::format_to(ctx.out(), "{}", e.ToString());
	}
};