#pragma once

#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
#include "Application.h"

namespace CHIKU
{
	class OpenXR : public Application
	{
	public:
		OpenXR();
		void Init() override;
		void Run() override;
		void Render() override;
		void CleanUp() override;

	private:
		void CreateInstance();
		void DestroyInstance();
		void CreateDebugMessenger();
		void DestroyDebugMessenger();
		void GetInstanceProperties();
		void GetSystemID();

	private:
		XrInstance m_xrInstance = {};
	};
}