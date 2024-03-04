#include "MainLayer.h"

MainLayer* MainLayer::s_Instance = nullptr;

MainLayer::MainLayer() : Layer("MainLayer"), m_Server(8000)
{
	EIS_ASSERT(!s_Instance, "Main Layer already exists!");
	s_Instance = this;
}
MainLayer::~MainLayer()
{
	s_Instance = nullptr;
}

void MainLayer::OnAttach()
{
	EIS_PROFILE_FUNCTION();

	Eis::RenderCommands::Disable(0x0B71); // GL_DEPTH_TEST
	Eis::Application::Get().GetWindow().SetTitle("Sot2D Server");

	m_Server.SetClientConnectedCallback(ClientConnectedCallback);
	m_Server.SetClientDisconnectedCallback(ClientDisconnectedCallback);
	m_Server.SetDataReceivedCallback(DataRecievedCallback);
}
void MainLayer::OnDetach()
{
	EIS_PROFILE_FUNCTION();

	m_Server.Stop();
	m_Clients.clear();
}



void MainLayer::OnUpdate(Eis::TimeStep ts)
{
	EIS_PROFILE_FUNCTION();

	Eis::RenderCommands::Clear();
	Eis::RenderCommands::SetClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
}

void MainLayer::OnImGuiRender()
{
	EIS_PROFILE_FUNCTION();

	if (!ImGui::Begin("Server"))
	{
		ImGui::End();
		return;
	}

	if (m_Server.IsRunning() == false) // Init
	{
		static int port = 8000;
		ImGui::InputInt("Port", &port, 0, 0);

		if (ImGui::Button("Start"))
		{
			m_Server.SetPort(port);
			m_Server.Start();
		}
		if (ImGui::Button("Quit"))
			std::exit(0);
	}
	else // Running
	{
		ImGui::Text("%i clients connected", m_Clients.size());
		for (const NetClient& c : m_Clients)
			ImGui::Text("%i ", c.GetClientId());
		if (ImGui::Button("Stop"))
			m_Server.Stop(), m_Clients.clear();
	}

	ImGui::Text("");

	ImGui::End();
}