#pragma once
#include "Macros.h"
#include <queue>
#include <bitset>


namespace Cashew
{
	class CASHEW_API Keyboard
	{
		friend class Window;
	public:
		class Event
		{
		public:
			enum class Type
			{
				Press,
				Release,
				Invalid
			};
		private:
			Type m_type;
			unsigned char m_code;
		public:
			Event() noexcept
				:
				m_type(Type::Invalid),
				m_code(0u)
			{}
			Event(Type type, unsigned char code) noexcept
				:
				m_type(type),
				m_code(code)
			{}
			bool IsPress() const noexcept
			{
				return m_type == Type::Press;
			}
			bool IsRelease() const noexcept
			{
				return m_type == Type::Release;
			}
			bool IsValid() const noexcept
			{
				return m_type != Type::Invalid;
			}
			unsigned char GetCode() const noexcept
			{
				return m_code;
			}
		};
	public:
		Keyboard() = default;
		Keyboard(const Keyboard&) = delete;
		Keyboard& operator=(const Keyboard&) = delete;
		// key events
		bool KeyIsPressed(unsigned char keycode) const noexcept;
		Event ReadKey() noexcept;
		bool KeyIsEmpty() const noexcept;
		void FlushKey() noexcept;
		// char events
		char ReadChar() noexcept;
		bool CharIsEmpty() const noexcept;
		void FlushChar() noexcept;
		void Flush() noexcept;
		// autorepeat control
		void EnableAutorepeat() noexcept;
		void DisableAutorepeat() noexcept;
		bool AutorepeatIsEnabled() const noexcept;
	private:
		void OnKeyPressed(unsigned char keycode) noexcept;
		void OnKeyReleased(unsigned char keycode) noexcept;
		void OnChar(char character) noexcept;
		void ClearState() noexcept;
		template<typename T>
		static void TrimBuffer(std::queue<T>& buffer) noexcept;
	private:
		static constexpr unsigned int nKeys = 256u;
		static constexpr unsigned int bufferSize = 16u;
		bool autorepeatEnabled = false;
		std::bitset<nKeys> keystates;
		std::queue<Event> keybuffer;
		std::queue<char> charbuffer;
	};
}