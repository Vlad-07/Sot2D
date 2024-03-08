#include "ServerLayer.h"


ServerLayer* ServerLayer::s_Instance = nullptr;


ServerLayer::ServerLayer() : Layer("ServerLayer"), m_Server(8000), m_TerrainManager()
{
	EIS_ASSERT(!s_Instance, "Main Layer already exists!");
	s_Instance = this;
}
ServerLayer::~ServerLayer()
{
	s_Instance = nullptr;
}

void ServerLayer::OnAttach()
{
	EIS_PROFILE_FUNCTION();

	// Init app
	Eis::RenderCommands::Disable(0x0B71); // GL_DEPTH_TEST
	Eis::RenderCommands::SetClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));

	// Init net
	m_Server.SetClientConnectedCallback(ClientConnectedCallback);
	m_Server.SetClientDisconnectedCallback(ClientDisconnectedCallback);
	m_Server.SetDataReceivedCallback(DataRecievedCallback);
}
void ServerLayer::OnDetach()
{
	EIS_PROFILE_FUNCTION();

	m_Server.Stop();
	m_Clients.clear();
}


void ServerLayer::OnUpdate(Eis::TimeStep ts)
{
	EIS_PROFILE_FUNCTION();

	Eis::RenderCommands::Clear();
}

void ServerLayer::OnImGuiRender()
{
	EIS_PROFILE_FUNCTION();

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetWorkCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(200, 120), ImGuiCond_Appearing);
	ImGui::Begin("Server", nullptr, m_CommonFlags);

	if (m_Server.IsRunning() == false) // Init
	{
		static int port = 8000;
		ImGui::Text("Port: ");
		ImGui::SameLine();
		ImGui::InputInt("##port", &port, 0, 0);

		if (ImGui::Button("Start"))
		{
			m_TerrainManager.InitStartingArea();
			m_Server.SetPort(port);
			m_Server.Start();
		}
		if (ImGui::Button("Quit"))
			Eis::Application::ShouldClose();
	}
	else // Running
	{
		ImGui::Text("%i clients connected", m_Clients.size());
		for (const NetClient& c : m_Clients)
			ImGui::Text("%i ", c.GetClientId());
		if (ImGui::Button("Stop"))
			m_Server.Stop(), m_Clients.clear(), m_TerrainManager.Clear();
	}

	ImGui::Text("");

	ImGui::End();
}



NetClient& ServerLayer::GetClientByNetId(Eis::ClientID netId)
{
	EIS_PROFILE_FUNCTION();

	for (uint32_t i = 0; i < ServerLayer::GetClients().size(); i++)
	{
		if (ServerLayer::GetClients()[i].GetNetworkId() == netId)
			return ServerLayer::GetClients()[i];
	}
	EIS_ASSERT(false, "Could not find client by NetworkID!");
	return NetClient(0, 0);
}
std::vector<NetClient>::iterator ServerLayer::FindClientByNetId(Eis::ClientID netId)
{
	EIS_PROFILE_FUNCTION();

	for (uint32_t i = 0; i < ServerLayer::GetClients().size(); i++)
	{
		if (ServerLayer::GetClients()[i].GetNetworkId() == netId)
			return (ServerLayer::GetClients().begin() + i);
	}
	EIS_ASSERT(false, "Could not find client by NetworkID!");
	return ServerLayer::GetClients().end();
}