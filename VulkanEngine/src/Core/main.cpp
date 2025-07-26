#include "Application.h"

int main()
{
	ZoneScoped;    // Profile this block

	CHIKU::Application* Application = CHIKU::CreateApplication();

	Application->Init();
	Application->Render();
	Application->CleanUp();

	return 0;
}