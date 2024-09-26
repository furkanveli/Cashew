#include "CashewError.h"
#include <comdef.h>
#include <vector>

namespace Cashew
{
	Microsoft::WRL::ComPtr<ID3D12InfoQueue1> InfoQueue = nullptr;
	void QueueInit(ID3D12Device* device)
	{
		device->QueryInterface(IID_PPV_ARGS(&InfoQueue));
		D3D12_INFO_QUEUE_FILTER filter = {};

		D3D12_MESSAGE_SEVERITY allowedseverities[] = { D3D12_MESSAGE_SEVERITY_ERROR, D3D12_MESSAGE_SEVERITY_CORRUPTION };
		D3D12_MESSAGE_SEVERITY deniedseverities[] = {  D3D12_MESSAGE_SEVERITY_INFO};

		D3D12_MESSAGE_ID deniedIDs[] = {
		D3D12_MESSAGE_ID::D3D12_MESSAGE_ID_CREATEDEVICE_DEBUG_LAYER_STARTUP_OPTIONS
		};

		// Set the filter to include only errors
		filter.AllowList.NumSeverities = _countof(allowedseverities);
		filter.AllowList.pSeverityList = allowedseverities;

		filter.DenyList.NumSeverities = _countof(deniedseverities);
		filter.DenyList.pSeverityList = deniedseverities;

		filter.DenyList.NumIDs = _countof(deniedIDs);
		filter.DenyList.pIDList = deniedIDs;

		// Apply the filter
		InfoQueue->AddStorageFilterEntries(&filter);
		
	}

	CheckerToken chk;

	void operator>>(HrGrabber grabber, CheckerToken chk)
	{
		if (FAILED(grabber.hr))
		{
			if (InfoQueue->GetNumStoredMessages() > 0)
			{
				throw CashewD3DError((int)grabber.loc.line(), ToWstring(grabber.loc.file_name()).c_str(), grabber.hr, ToWstring(grabber.loc.function_name()).c_str());
			}
			else
			{
				throw CashewWindowError((int)grabber.loc.line(), ToWstring(grabber.loc.file_name()).c_str(), grabber.hr, ToWstring(grabber.loc.function_name()).c_str());
			}
		}
	}

	HrGrabber::HrGrabber(unsigned int hr, std::source_location location) noexcept
		:hr(hr), loc(location)
	{}

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

	CashewWindowError::CashewWindowError(int line, const wchar_t* file, HRESULT hr, const wchar_t* function)
		:CashewError(line, file)
	{
		m_hr = hr;
		m_function = function;
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
			<< "[Error Code] " << "\n\n" << GetErrorCode() << "\n\n" 
			<< "[Description] " << "\n\n" << pMsgBuf << "\n\n" 
			<< "[File] " <<  m_file << "\n\n" << "[Line] " << m_line << "\n\n"
			<< "[Function] " << "\n\n" << m_function;

		LocalFree(pMsgBuf);
		return oss.str();

	}

	const wchar_t* CashewWindowError::GetType()
	{
		return L"Window Error";
	}

	

	CashewD3DError::CashewD3DError(int line, const wchar_t* file, HRESULT hr, const wchar_t* function)
		:CashewWindowError(line, file, hr, function)
	{}

	std::wstring CashewD3DError::GetErrorString()
	{
		wchar_t* pMsgBuf = nullptr;
		DWORD nMsgLen = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr,
			m_hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&pMsgBuf), 0, nullptr);
		if (nMsgLen == 0)
		{
			return L"Unidentified error code";
		}
		UINT64 messageCount = InfoQueue->GetNumStoredMessages();
		std::wstring DetailedDesc;
		if (messageCount > 0)
		{
			SIZE_T messageLength = 0;
			InfoQueue->GetMessageW(messageCount - 1, nullptr, &messageLength);
			std::vector<char> messageBuffer(messageLength);
			D3D12_MESSAGE* pMessage = reinterpret_cast<D3D12_MESSAGE*>(messageBuffer.data());
			InfoQueue->GetMessageW(messageCount - 1, pMessage, &messageLength);
			DetailedDesc = ToWstring(pMessage->pDescription);
		}

		std::wstringstream oss;

		oss << GetType() << std::endl
			<< "[Error Code] " << "\n\n" << GetErrorCode() << "\n\n"
			<< "[Description] " << "\n\n" << pMsgBuf << "\n\n"
			<< "[File] " << "\n\n" << m_file << "\n\n" << "[Line] " << m_line << "\n\n"
			<< "[Function] " << "\n\n" << m_function << "\n\n" 
			<< "[Detailed Description] " << "\n\n" << DetailedDesc << "\n\n";

		LocalFree(pMsgBuf);
		auto a = oss.str();

		a += m_file;
		return oss.str();
	}

	const wchar_t* CashewD3DError::GetType()
	{
		return L"D3D12 Error";
	}

}