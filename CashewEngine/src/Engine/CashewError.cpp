#include "CashewError.h"

namespace Cashew
{
	CashewError::CashewError(int line, const char* file, ErrorType type) noexcept
		:line(line), file(file), type(type)
	{}

	CashewError::CashewError(int line, const char* file, ErrorType type, HRESULT hr) noexcept
		:line(line), file(file), type(type), hr(hr)
	{}

	const char* CashewError::what() const noexcept
	{
		switch (type)
		{

		case ErrorType::stdError:
			{
				std::ostringstream oss;
				oss << GetType() << std::endl
					<< "[File] " << file << std::endl << "[Line] " << line;
				whatBuffer = oss.str();
				break;
			}

		case ErrorType::WindowError:
			{
				std::ostringstream oss;
				oss << GetType() << std::endl
					<< "[Error Code] " << GetErrorCode() << std::endl
					<< "[Description] " << GetErrorString() << std::endl
					<< "[File] " << file << std::endl << "[Line] " << line;
				whatBuffer = oss.str();
				break;
			}

		default:
			whatBuffer = "Unspecified Error";
			break;
		}

		return whatBuffer.c_str();
	}


	int CashewError::GetLine() const noexcept
	{
		return line;
	}

	const std::string& CashewError::GetFile() const noexcept
	{
		return file;
	}

	const char* CashewError::GetType() const noexcept
	{
		if (type == ErrorType::stdError)
		{
			return "stdError";
		}
		if (type == ErrorType::WindowError)
		{
			return "WindowError";
		}
		if (type == ErrorType::GraphicsError)
		{
			return "Graphics Error";
		}
		if (type == ErrorType::DevicedRemovedException)
		{
			return "Device Removed Exception";
		}

		return "Unspecified";

	}

	HRESULT CashewError::GetErrorCode() const noexcept
	{
		return hr;
	}


	std::string CashewError::GetErrorString() const noexcept
	{
		char* pMsgBuf = nullptr;
		DWORD nMsgLen = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr,
			hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr);
		if (nMsgLen == 0)
		{
			return "Unidentified error code";
		}
		std::string errorString = pMsgBuf;
		LocalFree(pMsgBuf);
		return errorString;
	}
	

}