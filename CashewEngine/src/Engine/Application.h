#pragma once
#include "Macros.h"
#include "Window.h"

namespace Cashew
{
	class CASHEW_API Application
	{
	public:
		Application();
		virtual ~Application();
		virtual void Init();
		virtual int Run();
	private:
		Window m_window;
		HWND m_consoleHandle;
	};

	// this is going to be defined in client
	std::unique_ptr<Application> CreateApplication();

	// this is going to be defined in client as well
	HWND CASHEW_API CreateConsole();


}//namespace end

