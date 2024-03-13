#include "GameLayer.h"

#include <glm/gtc/type_ptr.hpp>
#include "../../Sot2D_server/src/Network/NetPackets.h" // HACK


GameLayer* GameLayer::s_Instance = nullptr;


GameLayer::GameLayer() : Layer("GameLayer"), m_LocalPlayer(), m_PauseOpen(false), m_PerformanceOpen(false), m_GodMode(false), m_DeltaTime(), m_Terrain()
{
	EIS_ASSERT(!s_Instance, "Layer already exists!");
	s_Instance = this;
}
GameLayer::~GameLayer()
{
	m_Client.Disconnect();
	s_Instance = nullptr;
}


void GameLayer::OnAttach()
{
	// Init app
	Eis::RenderCommands::Disable(0x0B71); // GL_DEPTH_TEST
	Eis::Renderer2D::SetClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));

	// Init net
	m_Client.SetServerConnectedCallback(ConnectedCallback);
	m_Client.SetServerDisconnectedCallback(DisconnectedCallback);
	m_Client.SetDataReceivedCallback(DataReceivedCallback);

	// Init game
	m_Map = Eis::Texture2D::Create("assets/textures/sotmap.png");
	m_NetPlayerTex = Eis::Texture2D::Create("assets/textures/player.png");

}
void GameLayer::OnDetach()
{
	m_Client.Disconnect();
}


void GameLayer::OnUpdate(Eis::TimeStep ts)
{
	EIS_PROFILE_FUNCTION();

	Eis::Renderer2D::Clear();

	// Check in-game status
	if (m_Client.GetConnectionStatus() != Eis::Client::Connected)
		return;

	m_DeltaTime = ts;
	
	// Local update
	m_LocalPlayer.OnUpdate(ts);
	for (NetPlayer& p : m_NetworkPlayers)
		p.Interpolate();

//	m_Terrain.UnloadFarIslands(m_LocalPlayer.GetPos());

	// Update Network
	if (m_LocalPlayer.HasMoved())
	{
		glm::vec2 newPos = m_LocalPlayer.GetPos();
		UpdatePacket packet(UpdateType::MOVEMENT, 0, &newPos, sizeof(glm::vec2));
		m_Client.SendBuffer(UpdatePacket::CreateBuffer(packet));
	}

	// Rendering
	Eis::Renderer2D::BeginScene(m_LocalPlayer.GetCameraController().GetCamera());

	Eis::Renderer2D::DrawQuad(m_LocalPlayer.GetPos(), glm::vec2(160 * 3, 90 * 3), glm::vec4(0, 0, 0.5f, 1)); // ocean
	m_Terrain.RenderIslands(m_LocalPlayer.GetPos());

	for (const NetPlayer& p : m_NetworkPlayers)
		Eis::Renderer2D::DrawRotatedQuad(p.GetPos(), glm::vec2(1), p.GetRotation() + glm::radians(20.0f), m_NetPlayerTex); // angle the mouche texture to forward

	Eis::Renderer2D::DrawRotatedQuad(m_LocalPlayer.GetPos(), glm::vec2(1), m_LocalPlayer.GetRotation() + glm::radians(20.0f), m_LocalPlayer.GetTexture());

	Eis::Renderer2D::EndScene();
}

void GameLayer::OnImGuiRender()
{
	EIS_PROFILE_FUNCTION();

	// Main Menu
	if (m_Client.GetConnectionStatus() != Eis::Client::Connected)
	{
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetWorkCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(265, 100), ImGuiCond_Appearing);
		ImGui::Begin("Main Menu", nullptr, m_CommonFlags);

		static char ip[32] = "127.0.0.1:8000";
		if (m_Client.GetConnectionStatus() != Eis::Client::Connecting) // Select where to connect
		{
			ImGui::Text("IP: ");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(150.0f);
			if (ImGui::InputText("##ip", ip, sizeof(ip), ImGuiInputTextFlags_EnterReturnsTrue)
				|| ImGui::Button("Connect"))
				m_Client.ConnectToServer(ip);
		}
		else // Trying to connect...
		{
			ImGui::Text("Connecting to %s...", ip);
			if (ImGui::Button("Cancel"))
				m_Client.Disconnect();
		}
		if (ImGui::Button("Quit"))
			Eis::Application::ShouldClose();

		ImGui::End();
	}
	else // In game
	{
		if (m_PauseOpen) // Pause menu
		{
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetWorkCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowSize(ImVec2(100, 50), ImGuiCond_Appearing);
			ImGui::Begin("##PauseMenu", nullptr, m_CommonFlags | ImGuiWindowFlags_NoTitleBar);
			if (ImGui::Button("Disconnect"))
				m_Client.Disconnect();
			ImGui::End();
		}
		if (m_PerformanceOpen) // Performance menu
		{
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->WorkPos, ImGuiCond_Appearing);
			ImGui::SetNextWindowSize(ImVec2(130, 50), ImGuiCond_Appearing);
			ImGui::SetNextWindowBgAlpha(0.1f);
			ImGui::Begin("##PerformanceMenu", nullptr, m_CommonFlags | ImGuiWindowFlags_NoTitleBar);
			ImGui::Text("%.0f FPS (%.1f ms)", 1.0f / m_DeltaTime, m_DeltaTime.GetMilliseconds());
			if (ImGui::Button("God mode"))
			{
				m_GodMode = !m_GodMode;
				if (m_GodMode) // enable
				{
					m_LocalPlayer.GetCameraController().SetMaxZoom(1000.0f);
					m_LocalPlayer.GetCameraController().SetZoom(100.0f);
					m_LocalPlayer.GetCameraController().SetCameraSpeed(200.0f);
				}
				else // disable
				{
					m_LocalPlayer.GetCameraController().SetCameraSpeed(5.0f);
					m_LocalPlayer.GetCameraController().SetZoom(2.0f);
					m_LocalPlayer.GetCameraController().SetMaxZoom(6.0f);
				}
			}
			ImGui::End();
		}
	}
}

void GameLayer::OnEvent(Eis::Event& e)
{
	EIS_PROFILE_FUNCTION();

	m_LocalPlayer.GetCameraController().OnEvent(e);

	if (m_Client.GetConnectionStatus() != Eis::Client::Connected)
		return;

	Eis::EventDispatcher d(e);
	d.Dispatch<Eis::KeyPressedEvent>([this](Eis::KeyPressedEvent& e) -> bool
	{
		if (e.GetKeyCode() == EIS_KEY_ESCAPE)
		{
			m_PauseOpen = !m_PauseOpen;
			return true;
		}
		if (e.GetKeyCode() == EIS_KEY_F3)
		{
			m_PerformanceOpen = !m_PerformanceOpen;
			return true;
		}
		if (e.GetKeyCode() == EIS_KEY_LEFT_SHIFT)
		{
			m_LocalPlayer.SetRunning(true);
			return true;
		}
		return false;
	});
	d.Dispatch<Eis::KeyReleasedEvent>([this](Eis::KeyReleasedEvent e) -> bool
	{
		if (e.GetKeyCode() == EIS_KEY_LEFT_SHIFT)
		{
			m_LocalPlayer.SetRunning(false);
			return true;
		}
		return false;
	});
}


void GameLayer::InitSession()
{
	EIS_PROFILE_FUNCTION();

	m_LocalPlayer.Init();
	m_NetworkPlayers.clear();
	m_Terrain.Clear();
	m_PauseOpen = false;
}


NetPlayer& GameLayer::GetClientById(ClientId id)
{
	EIS_PROFILE_FUNCTION();

	for (uint32_t i = 0; i < GameLayer::GetNetworkPlayers().size(); i++)
	{
		if (GameLayer::GetNetworkPlayers()[i].GetId() == id)
			return GameLayer::GetNetworkPlayers()[i];
	}
	EIS_ASSERT(false, "Could not find client by NetworkID!");
	throw;
}
std::vector<NetPlayer>::iterator GameLayer::FindClientById(ClientId id)
{
	EIS_PROFILE_FUNCTION();

	for (uint32_t i = 0; i < GameLayer::GetNetworkPlayers().size(); i++)
	{
		if (GameLayer::GetNetworkPlayers()[i].GetId() == id)
			return GameLayer::GetNetworkPlayers().begin() + i;
	}
	EIS_ASSERT(false, "Could not find client by NetworkID!");
	return GameLayer::GetNetworkPlayers().end();
}