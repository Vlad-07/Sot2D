#include "ServerLayer.h"


ServerLayer* ServerLayer::s_Instance = nullptr;


ServerLayer::ServerLayer() : Layer("ServerLayer"), m_Server(8000), m_WorldManager()
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
	Eis::Renderer2D::SetClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
	Eis::Renderer2D::Shutdown(); // HACK: no headless config yet

	// Init net
	m_Server.SetClientConnectedCallback(ClientConnectedCallback);
	m_Server.SetClientDisconnectedCallback(ClientDisconnectedCallback);
	m_Server.SetDataReceivedCallback(DataRecievedCallback);
}
void ServerLayer::OnDetach()
{
	EIS_PROFILE_FUNCTION();

	m_Server.Stop();
	Cleanup();
}


void ServerLayer::OnUpdate(Eis::TimeStep ts)
{
	EIS_PROFILE_FUNCTION();

	Eis::Renderer2D::Clear();
}

void ServerLayer::OnImGuiRender()
{
	EIS_PROFILE_FUNCTION();

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetWorkCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(200, 120), ImGuiCond_Appearing);
	ImGui::Begin("Server", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	if (m_Server.IsRunning() == false) // Init
	{
		static int port = 8000;
		ImGui::Text("Port: ");
		ImGui::SameLine();
		ImGui::InputInt("##port", &port, 0, 0);
		constexpr ImVec2 size(45, 0);
		if (ImGui::Button("Start", size))
		{
			Init();
			m_Server.SetPort(port);
			m_Server.Start();
		}

		if (ImGui::Button("Quit", size))
			Eis::Application::ShouldClose();
	}
	else // Running
	{
		if (ImGui::Button("Stop"))
		{
			m_Server.Stop();
			Cleanup();
		}
		ImGui::Text("%i clients connected", m_Clients.size());
		for (uint32_t i = 0; i < m_Clients.size(); i++)
		{
			ImGui::Text("%i ", m_Clients[i].GetNetworkId());
			ImGui::SameLine();
			if (ImGui::Button("Kick"))
			{
				m_Server.KickClient(m_Clients[i].GetNetworkId());
				m_Clients.erase(m_Clients.begin() + i);
			}
		}
	}

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
	throw;
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

void ServerLayer::Init()
{
	m_WorldManager.Init();
}
void ServerLayer::Cleanup()
{
	m_Clients.clear();
	m_WorldManager.Clear();
}