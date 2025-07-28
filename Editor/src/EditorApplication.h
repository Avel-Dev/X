#pragma once
#include "Application.h"

namespace Editor
{
	class EditorApplication : public CHIKU::Application
	{
	public:
		EditorApplication() : CHIKU::Application()
		{
		}

		void Init() override;
	};
}