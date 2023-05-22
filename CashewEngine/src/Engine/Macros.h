#pragma once

#ifdef CASHEW_PLATFORM_WINDOWS
	#ifdef CASHEW_BUILD_DLL
		#define	CASHEW_API __declspec(dllexport)
	#else
		#define CASHEW_API __declspec(dllimport)
	#endif
#else
	#error Cashew only supports Windows right now!
#endif
