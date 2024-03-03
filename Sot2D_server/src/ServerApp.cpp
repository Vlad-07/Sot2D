#include <Eis.h>
#include <Eis/Core/EntryPoint.h>

#include <imgui.h> // Should remove from aditional include dirs

#include "MainLayer.h"


class ServerApp : public Eis::Application
{
public:
	ServerApp()
	{
		PushLayer(new MainLayer());
	}

	~ServerApp() = default;
};

Eis::Application* Eis::CreateApplication()
{
	return new ServerApp();
}