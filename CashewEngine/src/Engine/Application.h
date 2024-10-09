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
		
		static D3DGraphics* GetGfx();

		static bool m_appPaused;
		static bool m_minimized;
		static bool m_maximized;
		static bool m_resizing;

	private:
		virtual void Init();
		virtual void DoFrame(const CashewTimer& timer);
		std::wstring CalcFPS();

		static void InitializeGraphics(HWND hwnd, int width, int height);

	private:
		std::optional<Window> m_window; // using std::optional so that construction can be delayed to after the init call. This way we call logger from the window constructor safely
		HWND m_consoleHandle;

		

		static std::optional <D3DGraphics> m_gfx;
	};

	// this is going to be defined in client
	std::unique_ptr<Application> CreateApplication();

	// this is going to be defined in client as well
	HWND CASHEW_API CreateConsole();


}//namespace end

