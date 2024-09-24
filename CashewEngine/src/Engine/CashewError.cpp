#include "CashewError.h"
#include <comdef.h>

namespace Cashew
{
	CashewError::CashewError(int line, const wchar_t* file)
		:m_line(line), m_file(file)
	{}

	const wchar_t* CashewError::what()
	{
		m_whatBuffer = GetErrorString();
		return m_whatBuffer.c_str();
	}


	int CashewError::GetLine() 
	{
		return m_line;
	}

	const std::wstring& CashewError::GetFile() 
	{
		return m_file;
	}

	const wchar_t* CashewError::GetType() 
	{
		return L"Standard Error";
	}

	std::wstring CashewError::GetErrorString()
	{
		/*if (m_type == ErrorType::GraphicsError)
		{
			_com_error err(m_hr);
			std::wstring msg = err.ErrorMessage();

		}
		*/

		std::wstringstream oss;
		oss << GetType() << std::endl
			<< "[File] " << m_file << std::endl << "[Line] " << m_line;
		return oss.str();
	}
	

	CashewWindowError::CashewWindowError(int line, const wchar_t* file,  HRESULT hr)
		:CashewError(line, file)
	{
		m_hr = hr;
	}

	HRESULT CashewWindowError::GetErrorCode()
	{
		return m_hr;
	}


	std::wstring CashewWindowError::GetErrorString()
	{
		wchar_t* pMsgBuf = nullptr;
		DWORD nMsgLen = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr,
			m_hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&pMsgBuf), 0, nullptr);
		if (nMsgLen == 0)
		{
			return L"Unidentified error code";
		}

		std::wstringstream oss;
		oss << GetType() << std::endl
			<< "[Error Code] " << GetErrorCode() << std::endl
			<< "[Description] " << pMsgBuf << std::endl
			<< "[File] " << m_file << std::endl << "[Line] " << m_line;

		LocalFree(pMsgBuf);
		return oss.str();

	}

	const wchar_t* CashewWindowError::GetType()
	{
		return L"Window Error";
	}

}