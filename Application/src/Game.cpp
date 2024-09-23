#include <CashewEngine.h>

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
 std::unique_ptr<Cashew::Application> Cashew::CreateApplication()
{
	return std::make_unique<Game>();
}
