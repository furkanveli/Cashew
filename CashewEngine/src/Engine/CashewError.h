#pragma once
#include "Macros.h"
#include <exception>
#include <Windows.h>
#include <string>
#include <sstream>
namespace Cashew
{
	class CASHEW_API CashewError : public std::exception
	{
	public:
		enum class ErrorType 
		{
			stdError,
			WindowError,
			GraphicsError,
			DevicedRemovedException,
			Unspecified
		};

	public:
		CashewError(int line, const char* file, ErrorType type) noexcept;
		CashewError(int line, const char* file, ErrorType type, HRESULT hr) noexcept;
		const char* what() const noexcept override;

		// member getters
		int GetLine() const noexcept;
		const std::string& GetFile() const noexcept;
		const char* GetType() const noexcept;
		HRESULT GetErrorCode() const noexcept;

		// convenience funtions
		std::string GetErrorString() const noexcept;


	private:
		int line;
		std::string file;
		ErrorType type = ErrorType::Unspecified;
		HRESULT hr;
		mutable std::string whatBuffer;

	};
}