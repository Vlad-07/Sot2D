#include "GameLayer.h"

#include "../../Sot2D_server/src/Network/NetPackets.h" // HACK


void GameLayer::ConnectedCallback()
{	// k
}
void GameLayer::DisconnectedCallback()
{	// k
}

void GameLayer::DataReceivedCallback(Eis::Buffer& buf)
{
	Packet& packet = *((Packet*)buf.Data()); // HACK: ???
	switch (packet.GetType())
	{
	case PacketType::NONE:
		EIS_ASSERT(false, "Invalid packet received! (PacketType::NONE)");
		break;

	case PacketType::INITIAL:
	{
		GameLayer::Get().InitSession();

		InitialPacket& initialPacket = *((InitialPacket*)buf.Data()); // HACK: ???
		for (uint32_t i = 0; i < initialPacket.GetPlayerCount(); i++)
		{
			InitialPacket::PlayerData pData = ((InitialPacket::PlayerData*)((uint8_t*)buf.Data() + sizeof(InitialPacket)))[i]; // HACK: ???
			GameLayer::GetNetworkPlayers().push_back(NetPlayer(pData.id, pData.pos));
		}
		break;
	}

	case PacketType::PLAYER_UPDATE:
	{
		UpdatePacket& updatePacket = *((UpdatePacket*)buf.Data()); // HACK: ???
		switch (updatePacket.GetUpdateType())
		{
		case UpdateType::NONE:
			EIS_ASSERT(false, "Invalid message received! (UpdateMessage - UpdateType::NONE)");
			break;

		case UpdateType::CONNECT:
		{
			NetPlayer p(updatePacket.GetClientId());
			p.LoadTexture();
			GameLayer::GetNetworkPlayers().push_back(p);
			break;
		}

		case UpdateType::DISCONNECT:
			GameLayer::GetNetworkPlayers().erase(GameLayer::Get().FindClientById(updatePacket.GetClientId()));
			break;

		case UpdateType::MOVEMENT:
		{
			GameLayer::Get().GetClientById(updatePacket.GetClientId()).SetActualPos(
				*((glm::vec2*)((uint8_t*)buf.Data() + sizeof(UpdatePacket))) // HACK: ???
			);
			break;
		}
		}
		break;
	}
	}
}