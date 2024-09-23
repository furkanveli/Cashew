#include "Application.h"
#include <iostream>
#include <sstream>
#include "Window.h"
#include "Log.h"
#include "CashewError.h"
namespace Cashew
{


	Application::Application()
		:m_window(L"Cashew Application Window", L"CashewClass", 800, 600)
	{
	}

	Application::~Application()
	{
	}

	
	int Application::Run()
	{
		while (true)
		{
			if (const auto exitCode = m_window.ProcessMessages())
			{
				return *exitCode;
			}
		}
		return 0;
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
		SetConsoleTitle(L"Cashew Console");

		// Move the console window to a convenient position
		
		SetWindowPos(GetConsoleWindow(), 0, 0, 0, 800, 600, SWP_SHOWWINDOW);
		return GetConsoleWindow();
	}

}//end namespace
