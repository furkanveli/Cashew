#include <CashewEngine.h>

#ifdef CASHEW_PLATFORM_WINDOWS
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	try
	{
		auto app = Cashew::CreateApplication();
		app->Init();
		int result = app->Run();
		return result;
	}
	catch (Cashew::CashewError& e)
	{
		MessageBoxW(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBoxW(nullptr, L"Unspecified Error, No details available", L"Unknown exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
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


