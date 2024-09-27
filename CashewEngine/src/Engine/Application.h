#pragma once
#include "Macros.h"
#include "Window.h"
#include <optional>
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
		std::optional<Window> m_window; // using std::optional so that construction can be delayed to after the init call. This way we call logger from the window constructor safely
		HWND m_consoleHandle;
		std::optional <D3DGraphics> m_gfx;
	};

	// this is going to be defined in client
	std::unique_ptr<Application> CreateApplication();

	// this is going to be defined in client as well
	HWND CASHEW_API CreateConsole();


}//namespace end

