#include "OpenXR.h"

int main()
{
	ZoneScoped;    // Profile this block

	CHIKU::OpenXR Application;

	Application.Init();
	Application.Render();
	Application.CleanUp();

	return 0;
}