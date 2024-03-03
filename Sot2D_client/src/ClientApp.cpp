#include <Eis.h>
#include <Eis/Core/EntryPoint.h>

#include <imgui.h> // Should remove from aditional include dirs

#include "MainLayer.h"


class ClientApp : public Eis::Application
{
public:
	ClientApp()
	{
		PushLayer(new MainLayer());
	}

	~ClientApp() = default;
};

Eis::Application* Eis::CreateApplication()
{
	return new ClientApp();
}