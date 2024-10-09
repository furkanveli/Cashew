#include "Cashewpch.h"


namespace Cashew
{
	CashewTimer::CashewTimer()
		: m_secondsPerCount(0.0), m_deltaTime(-1.0), m_baseTime(0),
		m_pausedTime(0), m_previousTime(0), m_currentTime(0), m_stopped(false)
	{
		__int64 countsPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
		m_secondsPerCount = 1.0 / (double)countsPerSec;
	}

	float CashewTimer::TotalTime() const
	{
		if (m_stopped)
		{
			return (float)(((m_stopTime - m_pausedTime) - m_baseTime) * m_secondsPerCount);
		}

		else
		{
			return (float)(((m_currentTime - m_pausedTime) - m_baseTime) * m_secondsPerCount);
		}
	}

	float CashewTimer::DeltaTime() const
	{
		return (float)m_deltaTime;
	}

	void CashewTimer::Clear()
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		m_baseTime = currTime;
		m_previousTime = currTime;
		m_stopTime = 0;
		m_stopped = false;
	}

	void CashewTimer::Start()
	{
		__int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);


		

		if (m_stopped)
		{
			m_pausedTime += (startTime - m_stopTime);

			m_previousTime = startTime;
			m_stopTime = 0;
			m_stopped = false;
		}
	}

	void CashewTimer::Pause()
	{
		if (!m_stopped)
		{
			__int64 currTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

			m_stopTime = currTime;
			m_stopped = true;
		}
	}

	void CashewTimer::Tick()
	{
		if (m_stopped)
		{
			m_deltaTime = 0.0;
			return;
		}

		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		m_currentTime = currTime;

	
		m_deltaTime = (m_currentTime - m_previousTime) * m_secondsPerCount;

		m_previousTime = m_currentTime;

	
		if (m_deltaTime < 0.0)
		{
			m_deltaTime = 0.0;
		}
	}

}