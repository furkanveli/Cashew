#pragma once
#include "Macros.h"
#include <exception>
#include <Windows.h>
#include <string>
#include <sstream>
#include <source_location>
namespace Cashew
{
	struct CheckerToken {};
	extern CheckerToken chk;
	struct HrGrabber
	{
		HrGrabber(unsigned int hr, std::source_location location = std::source_location::current()) noexcept;
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

	private:
		HRESULT m_hr;
		std::wstring m_function;
	};


	inline std::wstring ToWstring(const std::string& str)
	{
		int size_needed = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
		std::wstring wstr(size_needed, 0);
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wstr[0], size_needed);
		return wstr;
	}
}


// Exception handling macros

#define ERR_STD() CashewError(__LINE__, ToWstring(__FILE__).c_str());
#define ERR_WND( hr ) CashewWindowError(__LINE__, ToWstring(__FILE__).c_str(), hr);
#define ERR_WNDF( hr, funcname ) CashewWindowError(__LINE__, ToWstring(__FILE__).c_str(), hr, funcname);

#ifndef ThrowIfWin
#define ThrowIfWin(x) \
    if (!x) { \
        DWORD errorCode = GetLastError(); \
        throw ERR_WND(errorCode); \
    }
#endif