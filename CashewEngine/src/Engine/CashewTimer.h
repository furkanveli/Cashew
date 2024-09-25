#pragma once
#include "Macros.h"
namespace Cashew
{
	class CASHEW_API CashewTimer
	{
	public:
		CashewTimer();

		float TotalTime() const;
		float DeltaTime() const;

		void Clear();
		void Start();
		void Pause();
		void Tick();

	private:
		double m_secondsPerCount;
		double m_deltaTime;

		__int64 m_baseTime;
		__int64 m_pausedTime;
		__int64 m_stopTime;
		__int64 m_previousTime;
		__int64 m_currentTime;

		bool m_stopped;
	};
}