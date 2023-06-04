#pragma once


 /*
 This is the main function.It is written in a.h file in engine to seperate it from the client.that's all really.
 If you want to change anything in the main function or want to incorporate different main functions based on different
 platform then this is the file you are going to do it. It is gonna be included in the client by copy pasting it basically
 and that will be the main function.This is so that the client doesn't have to deal with creation of main function and it is 
 going to be handled by the engine based on platform and abstract the game from low-level platform specific main function details.

 */

#ifdef CASHEW_PLATFORM_WINDOWS
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	Cashew::CreateConsole();

	Cashew::Log::Init();
	ENGINE_TRACE("Initialized Log from the Engine ");
	int a = 55;
	std::string b = "Hello";
	CLIENT_DEBUG("Var={0}, and {1}", a, b);

	auto app = Cashew::CreateApplication();
	app->Run();
	delete app;
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







