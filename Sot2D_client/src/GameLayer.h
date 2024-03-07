#pragma once

#include <Eis.h>
#include <Eis/Networking/Client.h>
#include <imgui.h>

#include "Game/LocalPlayer.h"
#include "Game/Terrain.h"
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

	NetPlayer& GetClientById(ClientId id);
	std::vector<NetPlayer>::iterator FindClientById(ClientId id);

	void InitSession();

private:
	Eis::Client m_Client;
	LocalPlayer m_LocalPlayer;

	std::vector<NetPlayer> m_NetworkPlayers;

	Terrain m_Terrain;

	bool m_PauseOpen;
	bool m_PerformanceOpen;

	Eis::Ref<Eis::Texture2D> m_Map,
							 m_NetPlayerTex; // HACK: NetPlayer textures are buggy

	Eis::TimeStep m_DeltaTime;

private:
	static GameLayer* s_Instance;

	const ImGuiWindowFlags m_CommonFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;
};