#include "Cashewpch.h"

#include "Window.h"
#include "Application.h"



namespace Cashew
{

	Window::~Window()
	{
		// Unregister class and destroy window
		UnregisterClassW(m_ClassName, m_hInst);
		DestroyWindow(m_hwnd);
	}

	Window::Window(wchar_t* name, wchar_t* classname, unsigned int w, unsigned int h) noexcept
		:m_Name(name), m_ClassName(classname), m_width(w), m_height(h), m_hInst(GetModuleHandleW(nullptr))
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
		RegisterClassExW(&m_windowClass);


		// calculate window size based on desired client region size
		RECT wr;
		wr.left = 0;
		wr.right = m_width + wr.left;
		wr.top = 0;
		wr.bottom = m_height + wr.top;
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE) >> chk;
		
		// Create window and get window handle
		m_hwnd = CreateWindowW(m_ClassName, m_Name, WS_OVERLAPPEDWINDOW, 850, 200,
			wr.right - wr.left, wr.bottom - wr.top, nullptr, nullptr, m_hInst, this);
		if (m_hwnd == nullptr)
		{
			throw ERR_LAST();
		}
		RECT clientRect;
		GetClientRect(m_hwnd, &clientRect);
		m_width = clientRect.right - clientRect.left;
		m_height = clientRect.bottom - clientRect.top;
		ShowWindow(m_hwnd, SW_SHOWDEFAULT);
	}

	wchar_t* Window::GetName()
	{
		return m_Name;
	}

	wchar_t* Window::GetWindowClassName()
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

	HWND Window::GetHwnd()
	{
		return m_hwnd;
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
			SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(WinWindowPtr));
			SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgTransfer));
			
			return WinWindowPtr->HandleMsg(hwnd, msg, wParam, lParam);
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	LRESULT Window::HandleMsgTransfer(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		Window* const WinWindowPtr = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
		return WinWindowPtr->HandleMsg(hwnd, msg, wParam, lParam);
	}

	std::optional<int> Window::ProcessMessages()
	{
		MSG msg = { 0 };

		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				return msg.wParam;
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		return {};

		
	}

	LRESULT Window::HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				Application::m_appPaused = true;
				m_timer.Pause();
			}
			else
			{
				Application::m_appPaused = false;
				m_timer.Start();
			}
			break;
		case WM_SIZE:
		{
			D3DGraphics* gfx = Application::GetGfx();
			m_width = LOWORD(lParam);
			m_height = HIWORD(lParam);
			if (gfx)
			{
				gfx->SetWidth(LOWORD(lParam));
				gfx->SetHeight(HIWORD(lParam));
				if (gfx->DeviceExists())
				{
					if (wParam == SIZE_MINIMIZED)
					{
						Application::m_appPaused = true;
						Application::m_minimized = true;
						Application::m_maximized = false;
					}
					else if (wParam == SIZE_MAXIMIZED)
					{
						Application::m_appPaused = false;
						Application::m_minimized = false;
						Application::m_maximized = true;
						gfx->OnResize();
					}
					else if (wParam == SIZE_RESTORED)
					{
						if (Application::m_minimized)
						{
							Application::m_appPaused = false;
							Application::m_minimized = false;
							gfx->OnResize();
						}
						else if (Application::m_maximized)
						{
							Application::m_appPaused = false;
							Application::m_maximized = false;
							gfx->OnResize();
						}
						else if (Application::m_resizing)
						{
							// do nothing until resize bars are let go.
						}
						else
						{
							gfx->OnResize();
						}
					}
				}
			}
			break;
		}
		case WM_ENTERSIZEMOVE:
			Application::m_appPaused = true;
			Application::m_resizing = true;
			m_timer.Pause();
			break;

		case WM_EXITSIZEMOVE:
			Application::m_appPaused = false;
			Application::m_resizing = false;
			m_timer.Start();
			Application::GetGfx()->OnResize();
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_MENUCHAR:
			// Don't beep when we alt-enter.
			return MAKELRESULT(0, MNC_CLOSE);
		case WM_GETMINMAXINFO:
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
			break;
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
			// clear keystate when window loses focus to prevent input getting "stuck"
		case WM_KILLFOCUS:
			kbd.ClearState();
			break;

			/*********** KEYBOARD MESSAGES ***********/
		case WM_KEYDOWN:
			// syskey commands need to be handled to track ALT key (VK_MENU) and F10
		case WM_SYSKEYDOWN:
			if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled()) // filter autorepeat
			{
				kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
			}
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
			if (wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
				return 0;
			}
			break;
		case WM_CHAR:
			kbd.OnChar(static_cast<unsigned char>(wParam));
			break;
			/*********** END KEYBOARD MESSAGES ***********/

			/************* MOUSE MESSAGES ****************/
		case WM_MOUSEMOVE:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			// in client region -> log move, and log enter + capture mouse (if not previously in window)
			if (pt.x >= 0 && pt.x < m_width && pt.y >= 0 && pt.y < m_height)
			{
				mouse.OnMouseMove(pt.x, pt.y);
				if (!mouse.IsInWindow())
				{
					SetCapture(hwnd);
					mouse.OnMouseEnter();
				}
			}
			// not in client -> log move / maintain capture if button down
			else
			{
				if (wParam & (MK_LBUTTON | MK_RBUTTON))
				{
					mouse.OnMouseMove(pt.x, pt.y);
				}
				// button up -> release capture / log event for leaving
				else
				{
					ReleaseCapture();
					mouse.OnMouseLeave();
				}
			}
			break;
		}
		case WM_LBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnLeftPressed(pt.x, pt.y);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnRightPressed(pt.x, pt.y);
			break;
		}
		case WM_LBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnLeftReleased(pt.x, pt.y);
			// release mouse if outside of window
			if (pt.x < 0 || pt.x >= m_width || pt.y < 0 || pt.y >= m_height)
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
			break;
		}
		case WM_RBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnRightReleased(pt.x, pt.y);
			// release mouse if outside of window
			if (pt.x < 0 || pt.x >= m_width || pt.y < 0 || pt.y >= m_height)
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
			break;
		}
		case WM_MOUSEWHEEL:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
			mouse.OnWheelDelta(pt.x, pt.y, delta);
			break;
		}
		/************** END MOUSE MESSAGES **************/
		}

		return DefWindowProcW(hwnd, msg, wParam, lParam);
	}
	
	void Window::SetTitle(const std::wstring& title)
	{
		ThrowIfWin(SetWindowTextW(m_hwnd, title.c_str()));
	}

}