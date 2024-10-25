#include "Cashewpch.h"
#include "Application.h"
#include "Window.h"


namespace Cashew
{
	std::optional<D3DGraphics> Application::m_gfx = std::nullopt;
	bool Application::m_appPaused = false;
	bool Application::m_minimized = false;
	bool Application::m_maximized = false;
	bool Application::m_resizing  = false;

	D3DGraphics* Application::GetGfx()
	{
		return m_gfx.has_value() ? &m_gfx.value() : nullptr;
	}

	void Application::InitializeGraphics(HWND hwnd, int width, int height)
	{
		m_gfx.emplace(hwnd, width, height);
	}

	Application::Application()
	{
		Init(); // Call Init() to perform necessary setup

		// Now construct the window and graphics objects after Init
		m_window.emplace(std::wstring(L"Cashew Application Window").data(), std::wstring(L"Cashew Class").data(), 600, 600);
		Application::InitializeGraphics(m_window->GetHwnd(), m_window->GetWidth(), m_window->GetHeight());
	}

	Application::~Application()
	{
	}

	
	int Application::Run()
	{
	
		m_window->m_timer.Clear();
		while (true)
		{
			if (const auto exitCode = m_window->ProcessMessages())
			{
				return *exitCode;
			}
			
			DoFrame(m_window->m_timer);
		}
		return 0;
	}

	void Application::DoFrame(const CashewTimer& timer)
	{
		m_window->m_timer.Tick();
		CalcFPS();
		m_gfx->Render(m_window->m_timer, m_window->kbd, m_window->mouse);

	}

	std::wstring Application::CalcFPS()
	{
		static int frameCnt = 0;
		static float timeElapsed = 0.0f;
		std::wstring fpsText;
		frameCnt++;
		if (m_window->m_timer.TotalTime() - timeElapsed >= 1.0f)
		{
			float fps = (float)frameCnt; // fps = frameCnt / 1
			float mspf = 1000.0f / fps;

			std::wstring fpsStr = std::to_wstring(fps);
			std::wstring mspfStr = std::to_wstring(mspf);

			fpsText = L"    fps: " + fpsStr + L"   mspf: " + mspfStr;

			// Reset for next average.
			frameCnt = 0;
			timeElapsed += 1.0f;		
			std::wstringstream os;
			os << L"Cashew Window" << fpsText << "  X pos: " << m_window->mouse.GetPosX() << " Y pos: " << m_window->mouse.GetPosY();
			m_window->SetTitle(os.str());
	
		}
		
		return fpsText;
	}

	

	void Application::Init()
	{
#ifdef CASHEW_DEBUG
		Cashew::CreateConsole();
		Cashew::Log::Init();
		ENGINE_TRACE("Initialized Log from the Engine ");
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
		SetForegroundWindow(GetConsoleWindow());
		return GetConsoleWindow();
	}

}//end namespace
