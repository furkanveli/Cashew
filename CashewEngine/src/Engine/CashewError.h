#pragma once
#include "Macros.h"
#include <exception>
#include <Windows.h>
#include <string>
#include <sstream>
#include <source_location>
#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include "d3d12sdklayers.h"
#include "dxgidebug.h"
#include <wrl.h>
namespace Cashew
{
#ifdef CASHEW_DEBUG
	extern Microsoft::WRL::ComPtr<ID3D12InfoQueue1> ID3D12InfoQueue;
	extern Microsoft::WRL::ComPtr<IDXGIInfoQueue> DXGIInfoQueue;
	void QueueInit(ID3D12Device* device);
#endif


	struct CheckerToken {};
	extern CheckerToken chk;
	struct HrGrabber
	{
		HrGrabber(unsigned int hr, std::source_location location = std::source_location::current())  noexcept;
		unsigned int hr;
		std::source_location loc;
	};
	void operator>>(HrGrabber grabber, CheckerToken chk);


	class CASHEW_API CashewError
	{
	public:
		CashewError(int line, const wchar_t* file); // constructor for standard error
		const wchar_t* what();

		// to be overridden
		virtual std::wstring GetErrorString();
		virtual const wchar_t* GetType();

		// member getters
		int GetLine();
		const std::wstring& GetFile();

	protected:
		int m_line;
		std::wstring m_file;
		std::wstring m_whatBuffer;
	};

	class CASHEW_API CashewWindowError : public CashewError
	{
	public:
		CashewWindowError(int line, const wchar_t* file, HRESULT hr);
		CashewWindowError(int line, const wchar_t* file, HRESULT hr, const wchar_t* function);
		HRESULT GetErrorCode();

		// overrides
		std::wstring GetErrorString() override;
		const wchar_t* GetType() override;

	protected:
		HRESULT m_hr;
		std::wstring m_function;
	};

	class CASHEW_API CashewD3DError : public CashewWindowError
	{
	public:
		CashewD3DError(int line, const wchar_t* file, HRESULT hr, const wchar_t* function);

		std::wstring GetErrorString() override;
		const wchar_t* GetType() override;
		
	};

	inline std::wstring ToWstring(const std::string& str)
	{
		int size_needed = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
		std::wstring wstr(size_needed, 0);
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wstr[0], size_needed);
		return wstr;
	}

	inline std::string ToAString(const std::wstring& wstr)
	{
		if (wstr.empty()) return std::string();

		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
		std::string str(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, nullptr, nullptr);
		return str;
	}
}


// Exception handling macros

#define ERR_STD() CashewError(__LINE__, ToWstring(__FILE__).c_str());
#define ERR_WND( hr ) CashewWindowError(__LINE__, ToWstring(__FILE__).c_str(), hr);
#define ERR_WNDF( hr, funcname ) CashewWindowError(__LINE__, ToWstring(__FILE__).c_str(), hr, funcname);
#define ERR_LAST() ERR_WND(GetLastError());

#ifndef ThrowIfWin
#define ThrowIfWin(x) \
    if (!x) { \
        DWORD errorCode = GetLastError(); \
        throw ERR_WND(errorCode); \
    }
#endif