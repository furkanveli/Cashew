#pragma once
#include "Macros.h"
#include "Window.h"
#include "D3DGraphics.h"

namespace Cashew
{
	class CASHEW_API Application
	{
	public:
		Application();
		virtual ~Application();
		virtual int Run();
		

	private:
		virtual void Init();
		virtual void Render(const CashewTimer& timer);
		std::wstring CalcFPS();
	private:
		Window m_window;
		HWND m_consoleHandle;
		D3DGraphics m_gfx;
	};

	// this is going to be defined in client
	std::unique_ptr<Application> CreateApplication();

	// this is going to be defined in client as well
	HWND CASHEW_API CreateConsole();


}//namespace end

