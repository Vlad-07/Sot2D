#include <Eis.h>
#include <Eis/Core/EntryPoint.h>

#include <imgui.h> // Should remove from aditional include dirs

#include "GameLayer.h"


class ClientApp : public Eis::Application
{
public:
	ClientApp() : Application(Eis::WindowProps("Sot2D"))
	{
		PushLayer(new GameLayer());
	}

	~ClientApp() = default;
};

Eis::Application* Eis::CreateApplication()
{
	return new ClientApp();
}