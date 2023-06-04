#include <CashewEngine.h>
#include <iostream>

class Game : public Cashew::Application
{
public:
	Game()
	{

	}

	~Game()
	{

	}
};

// This is where the client implements the CreateApplication to return an application that inherits from Cashew::Application.
// Application is created on the heap because it is necessary to keep track of the memory.
Cashew::Application* Cashew::CreateApplication()
{
	return new Game();
}

void Cashew::CreateConsole()
{
    // Allocate a console for this application
    AllocConsole();

    // Redirect standard input, output, and error streams to the console
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);

    // Set the console window title
    SetConsoleTitle(L"Cashew Console");

    // Move the console window to a convenient position
    HWND consoleWindow = GetConsoleWindow();
    SetWindowPos(consoleWindow, 0, 0, 0, 800, 600, SWP_SHOWWINDOW);
}