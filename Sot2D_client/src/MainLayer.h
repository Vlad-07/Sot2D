#pragma once

#include <Eis.h>
#include <Eis/Networking/Client.h>
#include <imgui.h>

#include "LocalPlayer.h"
#include "NetworkPlayer.h"


class MainLayer : public Eis::Layer
{
public:
	MainLayer();
	~MainLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Eis::TimeStep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Eis::Event& e) override;

	static void ConnectedCallback();
	static void DisconnectedCallback();
	static void DataReceivedCallback(Eis::Buffer& buf);

private:
	static MainLayer& Get() { return *s_Instance; }
	static auto& GetNetworkPlayers() { return s_Instance->m_NetworkPlayers; }

	NetPlayer& GetClientById(ClientId id);
	auto FindClientById(ClientId id);

	void InitSessionStuff();

private:
	Eis::Client m_Client;
	LocalPlayer m_LocalPlayer;

	std::vector<NetPlayer> m_NetworkPlayers;

	bool m_OptionsOpen;

	Eis::Ref<Eis::Texture2D> m_Map,
							 m_NetPlayerTex; // HACK: NetPlayer textures are buggy

private:
	static MainLayer* s_Instance;
};