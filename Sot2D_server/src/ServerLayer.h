#pragma once

#include <Eis.h>
#include <Eis/Networking/Server.h>
#include <imgui.h>

#include "Network/NetClient.h"

#include "Game/TerrainManager.h"


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
	TerrainManager m_TerrainManager;

private:
	static ServerLayer* s_Instance;

	const ImGuiWindowFlags m_CommonFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;
};