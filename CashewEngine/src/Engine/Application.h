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
	};

	// this is going to be defined in client
	Application* CreateApplication();

	// this is going to be defined in client as well
	void CASHEW_API CreateConsole();


}//namespace end

