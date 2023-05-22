#pragma once


 /*
 This is the main function.It is written in a.h file in engine to seperate it from the client.that's all really.
 If you want to change anything in the main function or want to incorporate different main functions based on different
 platform then this is the file you are going to do it. It is gonna be included in the client by copy pasting it basically
 and that will be the main function.This is so that the client doesn't have to deal with creation of main function and it is 
 going to be handled by the engine based on platform and abstract the game from low-level platform specific main function details.

 */

#ifdef CASHEW_PLATFORM_WINDOWS

int main(int argc, char** argv)
{
	auto app = Cashew::CreateApplication();
	app->Run();
	delete app;
}

#endif