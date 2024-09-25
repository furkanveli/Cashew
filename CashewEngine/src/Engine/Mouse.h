#pragma once
#include "Macros.h"
#include <queue>

namespace Cashew
{
	class CASHEW_API Mouse
	{
		friend class Window;
	public:
		class Event
		{
		public:
			enum class Type
			{
				LPress,
				LRelease,
				RPress,
				RRelease,
				WheelUp,
				WheelDown,
				Move,
				Enter,
				Leave,
				Invalid
			};
		private:
			Type m_type;
			bool m_leftIsPressed;
			bool m_rightIsPressed;
			int m_x;
			int m_y;
		public:
			Event() 
				:
				m_type(Type::Invalid),
				m_leftIsPressed(false),
				m_rightIsPressed(false),
				m_x(0),
				m_y(0)
			{}
			Event(Type type, const Mouse& parent) 
				:
				m_type(type),
				m_leftIsPressed(parent.m_leftIsPressed),
				m_rightIsPressed(parent.m_rightIsPressed),
				m_x(parent.m_x),
				m_y(parent.m_y)
			{}
			bool IsValid() const 
			{
				return m_type != Type::Invalid;
			}
			Type GetType() const 
			{
				return m_type;
			}
			std::pair<int, int> GetPos() const 
			{
				return{ m_x,m_y };
			}
			int GetPosX() const 
			{
				return m_x;
			}
			int GetPosY() const 
			{
				return m_y;
			}
			bool LeftIsPressed() const 
			{
				return m_leftIsPressed;
			}
			bool RightIsPressed() const 
			{
				return m_rightIsPressed;
			}
		};
	public:
		Mouse() = default;
		Mouse(const Mouse&) = delete;
		Mouse& operator=(const Mouse&) = delete;
		std::pair<int, int> GetPos() const ;
		int GetPosX() const ;
		int GetPosY() const ;
		bool IsInWindow() const ;
		bool LeftIsPressed() const ;
		bool RightIsPressed() const ;
		Mouse::Event Read() ;
		bool IsEmpty() const 
		{
			return m_buffer.empty();
		}
		void Flush() ;
	private:
		void OnMouseMove(int x, int y) ;
		void OnMouseLeave() ;
		void OnMouseEnter() ;
		void OnLeftPressed(int x, int y) ;
		void OnLeftReleased(int x, int y) ;
		void OnRightPressed(int x, int y) ;
		void OnRightReleased(int x, int y) ;
		void OnWheelUp(int x, int y) ;
		void OnWheelDown(int x, int y) ;
		void TrimBuffer() ;
		void OnWheelDelta(int x, int y, int delta) ;
	private:
		static constexpr unsigned int m_bufferSize = 16u;
		int m_x;
		int m_y;
		bool m_leftIsPressed = false;
		bool m_rightIsPressed = false;
		bool m_isInWindow = false;
		int m_wheelDeltaCarry = 0;
		std::queue<Event> m_buffer;
	};
}