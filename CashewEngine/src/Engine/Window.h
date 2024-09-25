#pragma once
#include <Windows.h>
#include <optional>
#include <sstream>
#include "Macros.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Log.h"
#include "CashewError.h"
#include "CashewTimer.h"


namespace Cashew
{
	class CASHEW_API Window
	{
	public:
		Window() = default;
		~Window();
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(wchar_t* name, wchar_t* classname, unsigned int w, unsigned int h) noexcept;
	private:
		wchar_t* m_Name;
		wchar_t* m_ClassName;
		unsigned int m_width;
		unsigned int m_height;
		HINSTANCE m_hInst;
		HWND m_hwnd;
		WNDCLASSEX m_windowClass;


	public:
		Keyboard kbd;
		Mouse mouse;
		CashewTimer m_timer;
	private:
		wchar_t* GetName();
		wchar_t* GetWindowClassName();
		unsigned int GetWidth();
		unsigned int GetHeight();

		static LRESULT CALLBACK HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		static LRESULT CALLBACK HandleMsgTransfer(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		LRESULT HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	public:
		std::optional<int> ProcessMessages();
		void SetTitle(const std::wstring& title);
	};


}

