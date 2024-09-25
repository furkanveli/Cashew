#include "Application.h"
#include <iostream>
#include <sstream>
#include "Window.h"
#include "Log.h"
#include "CashewError.h"

namespace Cashew
{


	Application::Application()
		:m_window(std::wstring(L"Cashew Application Window").data(), std::wstring(L"Cashew Class").data(), 800, 600)
	{
	}

	Application::~Application()
	{
	}

	
	int Application::Run()
	{
		Init();
		m_window.m_timer.Clear();
		while (true)
		{
			if (const auto exitCode = m_window.ProcessMessages())
			{
				return *exitCode;
			}
			
			Render(m_window.m_timer);
		}
		return 0;
	}

	void Application::Render(const CashewTimer& timer)
	{
		m_window.m_timer.Tick();
		CalcFPS();

		std::wstringstream os;
		os << "Mouse X: " << m_window.mouse.GetPosX() << " " << "Mouse Y: " << m_window.mouse.GetPosY();
		m_window.SetTitle(os.str());

	}

	std::wstring Application::CalcFPS()
	{

		static int frameCnt = 0;
		static float timeElapsed = 0.0f;
		std::wstring fpsText;
		frameCnt++;
		if (m_window.m_timer.TotalTime() - timeElapsed >= 1.0f)
		{
			float fps = (float)frameCnt; // fps = frameCnt / 1
			float mspf = 1000.0f / fps;

			std::wstring fpsStr = std::to_wstring(fps);
			std::wstring mspfStr = std::to_wstring(mspf);

			fpsText = L"    fps: " + fpsStr + L"   mspf: " + mspfStr;

			// Reset for next average.
			frameCnt = 0;
			timeElapsed += 1.0f;
			ENGINE_INFO("Fps >> {}", ToAString(fpsText));
		}

		return fpsText;
	}

	void Application::Init()
	{
#ifdef CASHEW_DEBUG
		Cashew::CreateConsole();

		Cashew::Log::Init();
		ENGINE_TRACE("Initialized Log from the Engine ");
		CLIENT_TRACE("Initialized Log from the Client");
#endif
	}

	HWND CASHEW_API Cashew::CreateConsole()
	{
		// Allocate a console for this application
		AllocConsole();

		// Redirect standard input, output, and error streams to the console
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
		freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);

		// Set the console window title
		SetConsoleTitleW(L"Cashew Console");

		// Move the console window to a convenient position
		
		SetWindowPos(GetConsoleWindow(), 0, 0, 0, 800, 600, SWP_SHOWWINDOW);
		return GetConsoleWindow();
	}

}//end namespace
