#include "MainLayer.h"

#include "../../Sot2D_server/src/NetMessages.h" // HACK

NetPlayer& MainLayer::GetClientById(ClientId id)
{
	for (uint32_t i = 0; i < MainLayer::GetNetworkPlayers().size(); i++)
	{
		if (MainLayer::GetNetworkPlayers()[i].GetId() == id)
			return MainLayer::GetNetworkPlayers()[i];
	}
	EIS_ASSERT(false, "Could not find client by NetworkID!");
	return NetPlayer(-1);
}

auto MainLayer::FindClientById(ClientId id)
{
	for (uint32_t i = 0; i < MainLayer::GetNetworkPlayers().size(); i++)
	{
		if (MainLayer::GetNetworkPlayers()[i].GetId() == id)
			return MainLayer::GetNetworkPlayers().begin() + i;
	}
	EIS_ASSERT(false, "Could not find client by NetworkID!");
	return MainLayer::GetNetworkPlayers().end();
}


void MainLayer::ConnectedCallback()
{	// k
}
void MainLayer::DisconnectedCallback()
{	// k
}

void MainLayer::DataReceivedCallback(Eis::Buffer& buf)
{
	Message& msg = *((Message*)buf.Data()); // HACK: ???
	switch (msg.GetType())
	{
	case MessageType::NONE:
		EIS_ASSERT(false, "Invalid message received! (MessageType::NONE)");
		break;

	case MessageType::INITIAL:
	{
		MainLayer::Get().InitSessionStuff();

		InitialMessage& initialMsg = *((InitialMessage*)buf.Data()); // HACK: ???
		for (uint32_t i = 0; i < initialMsg.GetPlayerCount(); i++)
			MainLayer::GetNetworkPlayers().push_back(NetPlayer(((ClientId*)((uint8_t*)buf.Data() + sizeof(InitialMessage)))[i])); // HACK: ???
		break;
	}

	case MessageType::PLAYER_UPDATE:
	{
		UpdateMessage& updateMsg = *((UpdateMessage*)buf.Data()); // HACK: ???
		switch (updateMsg.GetUpdateType())
		{
		case UpdateType::NONE:
			EIS_ASSERT(false, "Invalid message received! (UpdateMessage - UpdateType::NONE)");
			break;

		case UpdateType::CONNECT:
		{
			NetPlayer p(updateMsg.GetClientId());
			p.LoadTexture();
			MainLayer::GetNetworkPlayers().push_back(p);
			break;
		}

		case UpdateType::DISCONNECT:
			MainLayer::GetNetworkPlayers().erase(MainLayer::Get().FindClientById(updateMsg.GetClientId()));
			break;

		case UpdateType::MOVEMENT:
		{
			glm::vec2 pos = *((glm::vec2*)((uint8_t*)buf.Data() + sizeof(UpdateMessage)));  // HACK: ???
			MainLayer::Get().GetClientById(updateMsg.GetClientId()).SetPos(
				pos
			);
			break;
		}
		}
		break;
	}
	}
}