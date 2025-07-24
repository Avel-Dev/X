#include "OpenXR.h"

namespace CHIKU
{
	OpenXR::OpenXR() :Application()
	{
	}

	void OpenXR::Init()
	{
		Application::Init();
		// Initialize OpenXR instance, system, session, and space here
		// This is a placeholder for actual OpenXR initialization code

	}
	void OpenXR::Run()
	{
		Application::Run();
	}
	void OpenXR::Render()
	{
		Application::Render();
		// Render OpenXR content here
	}
	void OpenXR::CleanUp()
	{
		Application::CleanUp();
		// Clean up OpenXR resources here
	}


	void OpenXR::DestroyInstance()
	{
	}

	void OpenXR::CreateDebugMessenger()
	{
	}

	void OpenXR::DestroyDebugMessenger()
	{
	}

	void OpenXR::GetInstanceProperties()
	{
	}

	void OpenXR::GetSystemID()
	{
	}

} // namespace CHIKU