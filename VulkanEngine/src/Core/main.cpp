#include "Application.h"
#include "Events/KeyEvents.h"

int main()
{
	ZoneScoped;    // Profile this block

	CHIKU::Application* Application = CHIKU::CreateApplication();
	Application->Init();
	Application->Render();
	Application->CleanUp();

	delete Application;

	return 0;
}