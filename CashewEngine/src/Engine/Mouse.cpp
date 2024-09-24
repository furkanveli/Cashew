#pragma once


#include "Mouse.h"
#include "Log.h"
#include <Windows.h>

namespace Cashew
{
	std::pair<int, int> Mouse::GetPos() const 
	{
		return { m_x,m_y };
	}

	int Mouse::GetPosX() const 
	{
		return m_x;
	}

	int Mouse::GetPosY() const 
	{
		return m_y;
	}

	bool Mouse::IsInWindow() const 
	{
		return m_isInWindow;
	}

	bool Mouse::LeftIsPressed() const 
	{
		return m_leftIsPressed;
	}

	bool Mouse::RightIsPressed() const 
	{
		return m_rightIsPressed;
	}

	Mouse::Event Mouse::Read() 
	{
		if (m_buffer.size() > 0u)
		{
			Mouse::Event e = m_buffer.front();
			m_buffer.pop();
			return e;
		}
		else
		{
			return Mouse::Event();
		}
	}

	void Mouse::Flush() 
	{
		m_buffer = std::queue<Event>();
	}

	void Mouse::OnMouseMove(int newx, int newy) 
	{
		m_x = newx;
		m_y = newy;

		m_buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
		TrimBuffer();
	}

	void Mouse::OnMouseLeave() 
	{
		m_isInWindow = false;
		m_buffer.push(Mouse::Event(Mouse::Event::Type::Leave, *this));
		TrimBuffer();
	}

	void Mouse::OnMouseEnter() 
	{
		m_isInWindow = true;
		m_buffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
		TrimBuffer();
	}

	void Mouse::OnLeftPressed(int x, int y) 
	{
		m_leftIsPressed = true;

		m_buffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
		TrimBuffer();
	}

	void Mouse::OnLeftReleased(int x, int y) 
	{
		m_leftIsPressed = false;

		m_buffer.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));
		TrimBuffer();
	}

	void Mouse::OnRightPressed(int x, int y) 
	{
		m_rightIsPressed = true;

		m_buffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
		TrimBuffer();
	}

	void Mouse::OnRightReleased(int x, int y) 
	{
		m_rightIsPressed = false;

		m_buffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
		TrimBuffer();
	}

	void Mouse::OnWheelUp(int x, int y) 
	{
		m_buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
		TrimBuffer();
	}

	void Mouse::OnWheelDown(int x, int y) 
	{
		m_buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
		TrimBuffer();
	}

	void Mouse::TrimBuffer() 
	{
		while (m_buffer.size() > m_bufferSize)
		{
			m_buffer.pop();
		}
	}

	void Mouse::OnWheelDelta(int x, int y, int delta) 
	{
		m_wheelDeltaCarry += delta;
		// generate events for every 120 
		while (m_wheelDeltaCarry >= WHEEL_DELTA)
		{
			m_wheelDeltaCarry -= WHEEL_DELTA;
			OnWheelUp(x, y);
		}
		while (m_wheelDeltaCarry <= -WHEEL_DELTA)
		{
			m_wheelDeltaCarry += WHEEL_DELTA;
			OnWheelDown(x, y);
		}
	}
}