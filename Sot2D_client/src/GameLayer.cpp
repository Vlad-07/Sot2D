#include "GameLayer.h"

#include "../../Sot2D_server/src/Network/NetPackets.h" // HACK


GameLayer* GameLayer::s_Instance = nullptr;


GameLayer::GameLayer() : Layer("GameLayer"), m_LocalPlayer(), m_PauseOpen(false), m_PerformanceOpen(false), m_DeltaTime(), m_World()
{
	EIS_ASSERT(!s_Instance, "GameLayer already exists!");
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
	ImGui::GetStyle().WindowBorderSize = 0.0f;
}
void GameLayer::OnDetach()
{
	m_Client.Disconnect();
}


void GameLayer::OnUpdate(Eis::TimeStep ts)
{
	EIS_PROFILE_FUNCTION();

	m_DeltaTime = ts;

	// Check in-game status
	if (m_Client.GetConnectionStatus() != Eis::Client::Connected)
	{
		Eis::Renderer2D::Clear();
		return;
	}

	// Local update
	m_LocalPlayer.OnUpdate(ts);

	if (m_GodMode)
	{
		if (Eis::Input::IsKeyPressed(EIS_KEY_F1))
			m_LocalPlayer.GetCameraController().SetZoom(m_LocalPlayer.GetCameraController().GetZoom() + 10.0f);
		else if (Eis::Input::IsKeyPressed(EIS_KEY_F2))
			m_LocalPlayer.GetCameraController().SetZoom(m_LocalPlayer.GetCameraController().GetZoom() - 10.0f);
	}

	// Network update
	if (m_LocalPlayer.HasMoved())
		m_Client.SendData(MovementPlayerUpdatePacket(0, m_LocalPlayer.GetPos()));

	// Remote update
	for (NetworkPlayer& p : m_NetworkPlayers)
	{
		if (!p.GetTexture())
			p.LoadTexture(); // BUG: if player is added from net thread between here and net player rendering, the texture wont be loaded thus the app crashing
		p.Update(ts);
	}

	// Rendering
	Eis::Renderer2D::ResetStats();
	Eis::Renderer2D::Clear();
	Eis::Renderer2D::BeginScene(m_LocalPlayer.GetCameraController().GetCamera());

	// Draw ocean
	Eis::Renderer2D::DrawQuad(m_LocalPlayer.GetPos(), glm::vec2(160 * 3, 90 * 3), glm::vec4(0, 0, 0.5f, 1)); // ocean
	
	// Draw terrain
	m_World.RenderIslands(m_LocalPlayer.GetPos());

	// Draw network players
	for (const NetworkPlayer& p : m_NetworkPlayers)
		Eis::Renderer2D::DrawRotatedQuad(p.GetPos(), glm::vec2(1), p.GetRotation() + 20.0f, p.GetTexture()); // angle the mouche texture to forward

	// Draw local player
	Eis::Renderer2D::DrawRotatedQuad(m_LocalPlayer.GetPos(), glm::vec2(1), m_LocalPlayer.GetRotation() + 20.0f, m_LocalPlayer.GetTexture());

	// Draw the F O G
	if (glm::length(m_LocalPlayer.GetPos()) > m_World.GetWorldSize() - c_RenderDistance || c_InfiniteRendering)
		Eis::Renderer2D::DrawCircle(glm::vec2(0.0f), glm::vec2(m_World.GetWorldSize() * 2.5f), glm::vec4(0.0f, 0.2f, 0.4f, 1.0f), c_FogThickness, c_FogFade);

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
			{
				InitSession();
				m_Client.ConnectToServer(ip);
			}
			if (ImGui::Button("Quit"))
				Eis::Application::ShouldClose();
		}
		else // Trying to connect...
		{
			ImGui::Text("Connecting to %s...", ip);
			if (ImGui::Button("Cancel"))
				m_Client.Disconnect();
		}

		ImGui::End();
	}
	else // In game
	{
	//	#ifdef EIS_DEBUG
		if (ImGui::Begin("Debug")) {
			if (ImGui::Button("God mode"))
			{
				m_GodMode = !m_GodMode;
				if (m_GodMode) // enable
				{
					m_LocalPlayer.GetCameraController().SetMaxZoom(1000000.0f);
					m_LocalPlayer.GetCameraController().SetZoom(100.0f);
					m_LocalPlayer.GetCameraController().SetCameraSpeed(250.0f);
				}
				else // disable
				{
					m_LocalPlayer.GetCameraController().SetZoom(10.0f);
					m_LocalPlayer.GetCameraController().SetMaxZoom(10.0f);
				}
			}

			ImGui::Checkbox("Infinite Rendering", &c_InfiniteRendering);
		} ImGui::End();
	//	#endif

		if (m_PauseOpen) // Pause menu
		{
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetWorkCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowSize(ImVec2(100, 50), ImGuiCond_Appearing);
			ImGui::Begin("##PauseMenu", nullptr, m_CommonFlags | ImGuiWindowFlags_NoTitleBar);
			if (ImGui::Button("Disconnect"))
			{
				m_Client.Disconnect();
				Cleanup();
			}
			ImGui::End();
		}
		if (m_PerformanceOpen) // Performance menu
		{
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->WorkPos, ImGuiCond_Appearing);
			ImGui::SetNextWindowSize(ImVec2(150, 120), ImGuiCond_Appearing);
			ImGui::SetNextWindowBgAlpha(0.2f);
			ImGui::Begin("##PerformanceMenu", nullptr,
						 m_CommonFlags | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing);
			ImGui::Text("%.0f FPS (%.1f ms)", 1.0f / m_DeltaTime, m_DeltaTime.GetMilliseconds());
			ImGui::Text("Draw calls:   %i", Eis::Renderer2D::GetStats().DrawCalls);
			ImGui::Text("Quad count:   %i", Eis::Renderer2D::GetStats().QuadCount);
			ImGui::Text("Circle count: %i", Eis::Renderer2D::GetStats().CircleCount);
			ImGui::Text("Line count:   %i", Eis::Renderer2D::GetStats().LineCount);
			ImGui::End();
		}
	}
}

void GameLayer::OnEvent(Eis::Event& e)
{
	EIS_PROFILE_FUNCTION();

	if (m_Client.GetConnectionStatus() != Eis::Client::Connected)
		return;

	m_LocalPlayer.GetCameraController().OnEvent(e);

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
		return false;
	});
}


void GameLayer::InitSession()
{
	EIS_PROFILE_FUNCTION();

	m_LocalPlayer.Init();
	m_PauseOpen = false;
	m_GodMode = false;
}
void GameLayer::Cleanup()
{
	m_NetworkPlayers.clear();
	m_World.Clear();
}


NetworkPlayer& GameLayer::GetClientById(Eis::ClientID id)
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
std::vector<NetworkPlayer>::iterator GameLayer::FindClientById(Eis::ClientID id)
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