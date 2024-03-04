#pragma once

#include <Eis.h>
#include <Eis/Networking/Server.h>
#include <imgui.h>

#include "NetClient.h"
#include "NetMessages.h"


class MainLayer : public Eis::Layer
{
public:
	MainLayer();
	~MainLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Eis::TimeStep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Eis::Event& e) override {}

	static void ClientConnectedCallback(const Eis::ClientInfo& info);
	static void ClientDisconnectedCallback(const Eis::ClientInfo& info);
	static void DataRecievedCallback(const Eis::ClientInfo& info, Eis::Buffer& buf);

private:
	static MainLayer& Get() { return *s_Instance; }

	NetClient& GetClientByNetId(Eis::ClientID netId);
	auto FindClientByNetId(Eis::ClientID netId);

private:
	Eis::Server m_Server;
	std::vector<NetClient> m_Clients;

private:
	static MainLayer* s_Instance;
};