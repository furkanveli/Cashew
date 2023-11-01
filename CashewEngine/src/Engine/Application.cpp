#include "Application.h"
#include <iostream>
#include <sstream>
#include "Log.h"
namespace Cashew
{


	Application::Application()
	{
	}

	Application::~Application()
	{
	}

	
	void Application::Run()
	{
		while (true)
		{
		}
			
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

	void CASHEW_API Cashew::CreateConsole()
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
		HWND consoleWindow = GetConsoleWindow();
		SetWindowPos(consoleWindow, 0, 0, 0, 800, 600, SWP_SHOWWINDOW);
	}

}//end namespace
