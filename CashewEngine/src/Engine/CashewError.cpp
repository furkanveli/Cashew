#include "CashewError.h"
#include <vector>

namespace Cashew
{
	Microsoft::WRL::ComPtr<ID3D12InfoQueue1> D3D12InfoQueue; // has to be created for dxgiinfoqueue to catch d3d12 errors
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> DXGIInfoQueue = nullptr;
	void QueueInit(ID3D12Device* device)
	{
		device->QueryInterface(IID_PPV_ARGS(&D3D12InfoQueue));

		// load the dll and get the address of the DXGIGetDebugInterface to call it for initializing DXGIInfoQueue
		typedef HRESULT(WINAPI* GetDXGIInterface)(REFIID, void**);

		auto dxgidebugdll = LoadLibraryExW(L"dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);
		if (dxgidebugdll == nullptr)
		{
			throw ERR_LAST();
		}
		auto GetInterface = reinterpret_cast<GetDXGIInterface>(reinterpret_cast<void*>(GetProcAddress(dxgidebugdll, "DXGIGetDebugInterface")));
		if (GetInterface == nullptr)
		{
			throw ERR_LAST();
		}

		GetInterface(IID_PPV_ARGS(&DXGIInfoQueue)) >> chk;

		// filtering messages removes useful warning messages from visual studio window too. Might wanna change CashewD3DError.GetErrorString() implementation if necessary.
		// would probably require looping through all the messages in the queue until one with corruption or error severity is found and that would be logged, leaving the warning
		// messages available for vs console.

		DXGI_INFO_QUEUE_FILTER filter = {};
		DXGI_INFO_QUEUE_MESSAGE_SEVERITY allowedseverities[] = { DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION };
		filter.AllowList.NumSeverities = _countof(allowedseverities);
		filter.AllowList.pSeverityList = allowedseverities;
	
		DXGIInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_ALL, &filter);
	}

	CheckerToken chk;

	void operator>>(HrGrabber grabber, CheckerToken chk)
	{
		if (FAILED(grabber.hr))
		{
			if ( DXGIInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL) > 0)
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
		return L"Standard Cashew Error";
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
		UINT64 d3dMessageCount = DXGIInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
		std::wstring DetailedDesc = L"No detailed description available!";
		if (d3dMessageCount > 0)
		{
			SIZE_T messageLength = 0;
			DXGIInfoQueue->GetMessageW(DXGI_DEBUG_ALL, d3dMessageCount -1, nullptr, &messageLength);
			std::vector<char> messageBuffer(messageLength);
			DXGI_INFO_QUEUE_MESSAGE* pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(messageBuffer.data());
			DXGIInfoQueue->GetMessageW(DXGI_DEBUG_ALL, d3dMessageCount -1, pMessage, &messageLength);
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