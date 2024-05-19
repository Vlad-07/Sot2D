#pragma once

#include <Eis.h>
#include <Eis/Networking/Server.h>
#include <imgui.h>

#include "Network/NetClient.h"

#include "Game/WorldManager.h"


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

	static auto& Get() { return *s_Instance; }
	static auto& GetWorldManager() { return s_Instance->m_WorldManager; }
	static auto& GetClients() { return s_Instance->m_Clients; }
	static auto& GetServer() { return s_Instance->m_Server; }

	static NetClient& GetClientByNetId(Eis::ClientID netId);
	static std::vector<NetClient>::iterator FindClientByNetId(Eis::ClientID netId);

	void Init();
	void Cleanup();

private:
	Eis::Server m_Server;
	std::vector<NetClient> m_Clients;
	WorldManager m_WorldManager;

	const float c_KillDist = 5290.0f;

private:
	static ServerLayer* s_Instance;
};