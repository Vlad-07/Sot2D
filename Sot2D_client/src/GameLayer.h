#pragma once

#include <Eis.h>
#include <Eis/Networking/Client.h>
#include <imgui.h>

#include "Game/LocalPlayer.h"
#include "Game/World.h"
#include "Networking/NetworkPlayer.h"


class GameLayer : public Eis::Layer
{
public:
	GameLayer();
	~GameLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Eis::TimeStep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Eis::Event& e) override;

private:
	static void ConnectedCallback();
	static void DisconnectedCallback();
	static void DataReceivedCallback(Eis::Buffer& buf);

	static GameLayer& Get() { return *s_Instance; }
	static auto& GetNetworkPlayers() { return s_Instance->m_NetworkPlayers; }
	static auto& GetWorld() { return s_Instance->m_World; }

	NetworkPlayer& GetClientById(Eis::ClientID id);
	std::vector<NetworkPlayer>::iterator FindClientById(Eis::ClientID id);

	void InitSession();
	void Cleanup();

	friend class LocalPlayer;

private:
	Eis::Client m_Client;
	LocalPlayer m_LocalPlayer;

	std::vector<NetworkPlayer> m_NetworkPlayers;

	World m_World;

	bool m_PauseOpen;
	bool m_PerformanceOpen;
	bool m_GodMode = false;

	Eis::TimeStep m_DeltaTime;

	const float c_FogThickness = 0.15f,
				c_FogFade = 0.035f;

private:
	static GameLayer* s_Instance;

	const ImGuiWindowFlags m_CommonFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;
};