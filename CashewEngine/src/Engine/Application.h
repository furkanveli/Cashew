#pragma once
#include "Macros.h"


namespace Cashew
{
	class CASHEW_API Application
	{
	public:
		Application();
		virtual ~Application();
		virtual void Init();
		virtual void Run();
	};

	// this is going to be defined in client
	Application* CreateApplication();

	// this is going to be defined in client as well
	void CASHEW_API CreateConsole();


}//namespace end

