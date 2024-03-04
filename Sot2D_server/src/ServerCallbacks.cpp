#include "MainLayer.h"


NetClient& MainLayer::GetClientByNetId(Eis::ClientID netId)
{
	for (uint32_t i = 0; i < MainLayer::Get().m_Clients.size(); i++)
	{
		if (MainLayer::Get().m_Clients[i].GetNetworkId() == netId)
			return MainLayer::Get().m_Clients[i];
	}
	EIS_ASSERT(false, "Could not find client by NetworkID!");
	return NetClient(0, 0);
}

auto MainLayer::FindClientByNetId(Eis::ClientID netId)
{
	for (uint32_t i = 0; i < MainLayer::Get().m_Clients.size(); i++)
	{
		if (MainLayer::Get().m_Clients[i].GetNetworkId() == netId)
			return (MainLayer::Get().m_Clients.begin() + i);
	}
	EIS_ASSERT(false, "Could not find client by NetworkID!");
	return MainLayer::Get().m_Clients.end();
}



void MainLayer::ClientConnectedCallback(const Eis::ClientInfo& info)
{
	static ClientId globalIdIncrementor = 0;

	if (MainLayer::Get().m_Clients.size() == 0) globalIdIncrementor = 0;

	NetClient c(globalIdIncrementor++, info.Id);

	// Inform everyone
	UpdateMessage m(UpdateType::CONNECT, c.GetClientId(), nullptr, 0);
	MainLayer::Get().m_Server.SendBufferToAllClients(UpdateMessage::CreateBuffer(m), info.Id);

	if (MainLayer::Get().m_Clients.size() > 0)
	{
		ClientId* ids = new ClientId[MainLayer::Get().m_Clients.size()];
		for (uint32_t i = 0; i < MainLayer::Get().m_Clients.size(); i++)
			ids[i] = MainLayer::Get().m_Clients[i].GetClientId();
		InitialMessage msg(MainLayer::Get().m_Clients.size(), ids);
		delete[] ids;
		MainLayer::Get().m_Server.SendBufferToClient(info.Id, InitialMessage::CreateBuffer(msg));
	}
	else
		MainLayer::Get().m_Server.SendBufferToClient(info.Id, InitialMessage::CreateBuffer(InitialMessage(0, nullptr)));

	MainLayer::Get().m_Clients.push_back(c);
}


void MainLayer::ClientDisconnectedCallback(const Eis::ClientInfo& info)
{
	auto clientIt = MainLayer::Get().FindClientByNetId(info.Id);

	UpdateMessage m(UpdateType::DISCONNECT, clientIt->GetClientId(), nullptr, 0);
	MainLayer::Get().m_Server.SendBufferToAllClients(UpdateMessage::CreateBuffer(m), info.Id);

	MainLayer::Get().m_Clients.erase(clientIt);
}


void MainLayer::DataRecievedCallback(const Eis::ClientInfo& info, Eis::Buffer& buf)
{
	Message& msg = *((Message*)buf.Data()); // HACK: ???
	switch (msg.GetType())
	{
	case MessageType::NONE:
		EIS_ASSERT(false, "Invalid message recieved! (MessageType::NONE)");
		break;

	case MessageType::PLAYER_UPDATE:
	{
		UpdateMessage& updateMsg = *((UpdateMessage*)buf.Data()); // HACK: ???

		switch (updateMsg.GetUpdateType())
		{
		case UpdateType::NONE:
			EIS_ASSERT(false, "Invalid message received! (UpdateMessage - UpdateType::NONE)");
			break;

		case UpdateType::CONNECT:
		case UpdateType::DISCONNECT:
			// Only the server should send these messages
			EIS_ASSERT(false, "Invalid message received! (UpdateMessage - UpdateType::CONNECT/DISCONNECT)");
			break;

		case UpdateType::MOVEMENT:
		{
			NetClient& client = MainLayer::Get().GetClientByNetId(info.Id);
			client.SetPos(*((glm::vec2*)((uint8_t*)buf.Data() + sizeof(UpdateMessage)))); // HACK: ???
			UpdateMessage m(
				UpdateType::MOVEMENT,
				client.GetClientId(),
				&client.GetPos(),
				sizeof(glm::vec2));
			MainLayer::Get().m_Server.SendBufferToAllClients(UpdateMessage::CreateBuffer(m), info.Id);
		}
		}
		break;
	}
	}
}