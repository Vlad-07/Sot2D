#include "MainLayer.h"

#include "../../Sot2D_server/src/NetMessages.h" // HACK

MainLayer* MainLayer::s_Instance = nullptr;

MainLayer::MainLayer() : Layer("MainLayer"), m_LocalPlayer(), m_OptionsOpen(false)
{
	EIS_ASSERT(!s_Instance, "Layer already exists!");
	s_Instance = this;
}
MainLayer::~MainLayer()
{
	m_Client.Disconnect();
	s_Instance = nullptr;
}

void MainLayer::OnAttach()
{
	// Init app
	Eis::RenderCommands::Disable(0x0B71); // GL_DEPTH_TEST
	Eis::Application::Get().GetWindow().SetTitle("Sot2D");

	// Init net
	m_Client.SetServerConnectedCallback(ConnectedCallback);
	m_Client.SetServerDisconnectedCallback(DisconnectedCallback);
	m_Client.SetDataReceivedCallback(DataReceivedCallback);

	// Init game
	m_LocalPlayer.GetCameraController().SetZoomSpeedEffect(false);
	m_LocalPlayer.GetCameraController().SetMinZoom(2.0f);
	m_LocalPlayer.GetCameraController().SetMaxZoom(6.0f);
	m_LocalPlayer.GetCameraController().SetCameraSpeed(5.0f);
	m_Map = Eis::Texture2D::Create("assets/textures/sotmap.png");
	m_LocalPlayer.LoadTexture();
	m_NetPlayerTex = Eis::Texture2D::Create("assets/textures/player.png");
}
void MainLayer::OnDetach()
{
	m_Client.Disconnect();
}



void MainLayer::OnUpdate(Eis::TimeStep ts)
{
	EIS_PROFILE_FUNCTION();

	Eis::RenderCommands::Clear();
	Eis::RenderCommands::SetClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));

	// Check in-game status
	if (m_Client.GetConnectionStatus() != Eis::Client::Connected)
		return;

	// Local update
	m_LocalPlayer.OnUpdate(ts);

	// Update Network
	if (m_LocalPlayer.HasMoved())
	{
		UpdateMessage msg(UpdateType::MOVEMENT, 0, &m_LocalPlayer.GetPos(), sizeof(glm::vec2));
		m_Client.SendBuffer(UpdateMessage::CreateBuffer(msg));
	}

	// Rendering
	Eis::Renderer2D::BeginScene(m_LocalPlayer.GetCameraController().GetCamera());

	Eis::Renderer2D::DrawQuad(glm::vec3(0), glm::vec2(15), m_Map);

	for (const NetPlayer& p : m_NetworkPlayers)
		Eis::Renderer2D::DrawQuad(p.GetPos(), glm::vec2(1), m_NetPlayerTex);

	Eis::Renderer2D::DrawQuad(m_LocalPlayer.GetPos(), glm::vec2(1), m_LocalPlayer.GetTexture());

	Eis::Renderer2D::EndScene();

}

void MainLayer::OnImGuiRender()
{
	EIS_PROFILE_FUNCTION();

	if (m_Client.GetConnectionStatus() != Eis::Client::Connected)
	{
		if (!ImGui::Begin("Main Menu"))
		{
			ImGui::End();
			return;
		}

		static char ip[32] = "127.0.0.1:8000";

		ImGui::InputText("Ip", ip, sizeof(ip));

		if (ImGui::Button("Connect"))
			m_Client.ConnectToServer(ip);
		if (ImGui::Button("Quit"))
			std::exit(0);

		ImGui::End();
	}
	else if (m_OptionsOpen) // Running
	{
		ImGui::Begin("##PauseMenu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		if (ImGui::Button("Disconnect"))
			m_Client.Disconnect();
		ImGui::End();
	}
}

void MainLayer::OnEvent(Eis::Event& e)
{
	EIS_PROFILE_FUNCTION();

	if (m_Client.GetConnectionStatus() != Eis::Client::Connected)
		return;

	Eis::EventDispatcher d(e);
	d.Dispatch<Eis::KeyPressedEvent>([this](Eis::KeyPressedEvent& e) -> bool
	{
		if (e.GetKeyCode() == EIS_KEY_ESCAPE)
		{
			m_OptionsOpen = !m_OptionsOpen;
			return true;
		}
		return false;
	});

	m_LocalPlayer.GetCameraController().OnEvent(e);
}


void MainLayer::InitSessionStuff()
{
	MainLayer::Get().m_LocalPlayer.GetCameraController().SetPosition(glm::vec3(0));
	MainLayer::Get().m_LocalPlayer.SetPos(glm::vec2(0));
	MainLayer::Get().m_NetworkPlayers.clear();
	MainLayer::Get().m_OptionsOpen = false;
}