#include <Eis.h>
#include <Eis/Core/EntryPoint.h>

#include <imgui.h> // Should remove from aditional include dirs

#include "ServerLayer.h"


class ServerApp : public Eis::Application
{
public:
	ServerApp() : Application(Eis::WindowProps("Sot2D GUI Server", 300, 200))
	{
		PushLayer(new ServerLayer());
	}
	
	~ServerApp() = default;
};

Eis::Application* Eis::CreateApplication()
{
	return new ServerApp();
}