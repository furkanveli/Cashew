#include <CashewEngine.h>



#ifdef CASHEW_PLATFORM_WINDOWS
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	try
	{
		auto app = Cashew::CreateApplication();
		CLIENT_TRACE("Initialized Log from the client");
		int result = app->Run();
		return result;
	}
	catch (Cashew::CashewError& e)
	{
		CLIENT_TRACE("{}", Cashew::ToAString(e.what()));
		MessageBoxW(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION );
		while (true) 
		{
			// Exit the loop if the console is closed
			if (GetConsoleWindow() == NULL) {
				break;
			}

			// Add a delay to avoid high CPU usage
			Sleep(100);
		}
		return EXIT_FAILURE;
	}
	catch (std::exception& e)
	{
		CLIENT_TRACE("{}", e.what());
		MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION );
		while (true)
		{
			// Exit the loop if the console is closed
			if (GetConsoleWindow() == NULL) {
				break;
			}

			// Add a delay to avoid high CPU usage
			Sleep(100);
		}
		return EXIT_FAILURE;
	}
	catch (...)
	{
		CLIENT_TRACE("Unspecified Error, No details available, Unknown exception");
		MessageBoxW(nullptr, L"Unspecified Error, No details available", L"Unknown exception", MB_OK | MB_ICONEXCLAMATION );
		while (true)
		{
			// Exit the loop if the console is closed
			if (GetConsoleWindow() == NULL) {
				break;
			}

			// Add a delay to avoid high CPU usage
			Sleep(100);
		}
		return EXIT_FAILURE;
	}
	
	return EXIT_FAILURE;
}
#else
int main(int argc, char** argv)
{
	Cashew::Log::Init();
	ENGINE_TRACE("Initialized Log from the Engine ");
	int a = 55;
	std::string b = "Hello";
	CLIENT_DEBUG("Var={0}, and {1}", a, b);

	auto app = Cashew::CreateApplication();
	app->Run();
	delete app;
}

#endif


