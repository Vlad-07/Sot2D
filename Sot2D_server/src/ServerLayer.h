#pragma once

#include <Eis.h>
#include <Eis/Networking/Server.h>
#include <imgui.h>

#include "Network/NetClient.h"

#include "../../Sot2D_client/src/Game/Terrain.h" // HACK


class ServerLayer : public Eis::Layer
{
public:
	ServerLayer();
	~ServerLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Eis::TimeStep ts) override;
	virtual void OnImGuiRender() override;

private:
	static void ClientConnectedCallback(const Eis::ClientInfo& info);
	static void ClientDisconnectedCallback(const Eis::ClientInfo& info);
	static void DataRecievedCallback(const Eis::ClientInfo& info, Eis::Buffer& buf);

	static ServerLayer& Get() { return *s_Instance; }
	static auto& GetClients() { return s_Instance->m_Clients; }

	static NetClient& GetClientByNetId(Eis::ClientID netId);
	static std::vector<NetClient>::iterator FindClientByNetId(Eis::ClientID netId);

private:
	Eis::Server m_Server;
	std::vector<NetClient> m_Clients;
	Terrain m_Terrain;

private:
	static ServerLayer* s_Instance;

	const ImGuiWindowFlags commonFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;
};