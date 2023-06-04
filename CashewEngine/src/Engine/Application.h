#pragma once
#include "Macros.h"


namespace Cashew
{
	class CASHEW_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// this is going to be defined in client
	Application* CreateApplication();

	// this is going to be defined in client as well
	void CreateConsole();


}//namespace end

