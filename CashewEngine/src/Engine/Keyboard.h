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
			Event() 
				:
				m_type(Type::Invalid),
				m_code(0u)
			{}
			Event(Type type, unsigned char code) 
				:
				m_type(type),
				m_code(code)
			{}
			bool IsPress() const 
			{
				return m_type == Type::Press;
			}
			bool IsRelease() const 
			{
				return m_type == Type::Release;
			}
			bool IsValid() const 
			{
				return m_type != Type::Invalid;
			}
			unsigned char GetCode() const 
			{
				return m_code;
			}
		};
	public:
		Keyboard() = default;
		Keyboard(const Keyboard&) = delete;
		Keyboard& operator=(const Keyboard&) = delete;
		// key events
		bool KeyIsPressed(unsigned char keycode) const ;
		Event ReadKey() ;
		bool KeyIsEmpty() const ;
		void FlushKey() ;
		// char events
		char ReadChar() ;
		bool CharIsEmpty() const ;
		void FlushChar() ;
		void Flush() ;
		// autorepeat control
		void EnableAutorepeat() ;
		void DisableAutorepeat() ;
		bool AutorepeatIsEnabled() const ;
	private:
		void OnKeyPressed(unsigned char keycode) ;
		void OnKeyReleased(unsigned char keycode) ;
		void OnChar(char character) ;
		void ClearState() ;
		template<typename T>
		static void TrimBuffer(std::queue<T>& buffer) ;
	private:
		static constexpr unsigned int nKeys = 256u;
		static constexpr unsigned int bufferSize = 16u;
		bool autorepeatEnabled = false;
		std::bitset<nKeys> keystates;
		std::queue<Event> keybuffer;
		std::queue<char> charbuffer;
	};
}