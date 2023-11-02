#include "Window.h"

namespace Cashew
{
	Window::Window()
	{

	};

	Window::~Window()
	{
		// Unregister class and destroy window
		UnregisterClass(m_ClassName, m_hInst);
		DestroyWindow(m_hwnd);
	}

	Window::Window(LPCWSTR name, LPCWSTR classname, unsigned int w, unsigned int h)
		:m_Name(name), m_ClassName(classname), m_width(w), m_height(h), m_hInst(GetModuleHandle(nullptr))
	{
		// Register the window class
		m_windowClass.cbSize = sizeof(m_windowClass);
		m_windowClass.style = CS_OWNDC;
		m_windowClass.lpfnWndProc = HandleMsgSetup;
		m_windowClass.cbClsExtra = 0;
		m_windowClass.cbWndExtra = 0;
		m_windowClass.hInstance = m_hInst;
		m_windowClass.hIcon = nullptr;
		m_windowClass.hCursor = nullptr;
		m_windowClass.hbrBackground = nullptr;
		m_windowClass.lpszMenuName = nullptr;
		m_windowClass.lpszClassName = classname;
		m_windowClass.hIconSm = nullptr;
		RegisterClassEx(&m_windowClass);

		// calculate window size based on desired client region size
		RECT wr;
		wr.left = 100;
		wr.right = m_width + wr.left;
		wr.top = 100;
		wr.bottom = m_height + wr.top;
		AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

		// Create window and get window handle
		m_hwnd = CreateWindow(m_ClassName, m_Name, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
			wr.right - wr.left, wr.bottom - wr.top, nullptr, nullptr, m_hInst, this);

		ShowWindow(m_hwnd, SW_SHOWDEFAULT);
	}

	LPCWSTR Window::GetName()
	{
		return m_Name;
	}

	LPCWSTR Window::GetWindowClassName()
	{
		return m_ClassName;
	}

	unsigned int Window::GetWidth()
	{
		return m_width;
	}

	unsigned int Window::GetHeight()
	{
		return m_height;
	}


	LRESULT Window::HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		// since windows callback functions that have CALLBACK in them cannot receive class instance pointers, I use this function to 
		// send a window pointer on winAPI side to store, then switch the windows procedure function to HandleMsgTransfer. HandleMsgTransfer
		// just takes the parameters and passes them to the member Handle with given parameters.
		if (msg == WM_NCCREATE)
		{
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			Window* const WinWindowPtr = static_cast<Window*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(WinWindowPtr));
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgTransfer));
			
			return WinWindowPtr->HandleMsg(hwnd, msg, wParam, lParam);
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	LRESULT Window::HandleMsgTransfer(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		Window* const WinWindowPtr = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		return WinWindowPtr->HandleMsg(hwnd, msg, wParam, lParam);
	}

	std::optional<int> Window::ProcessMessages()
	{
		MSG msg;

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				return msg.wParam;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return {};
	}

	LRESULT Window::HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	

}