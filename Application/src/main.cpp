#include <CashewEngine.h>

#ifdef CASHEW_PLATFORM_WINDOWS
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	auto app = Cashew::CreateApplication();
	app->Init();
	int result = app->Run();
	return result;
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


