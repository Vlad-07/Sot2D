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

	case PacketType::INIT_PLAYERS:
	{
		GameLayer::Get().InitSession();

		InitPlayersPacket& initPlayersPacket = *((InitPlayersPacket*)buf.Data()); // HACK: ???
		for (uint32_t i = 0; i < initPlayersPacket.GetPlayerCount(); i++)
		{
			InitPlayersPacket::PlayerData pData = ((InitPlayersPacket::PlayerData*)((uint8_t*)buf.Data() + sizeof(InitPlayersPacket)))[i]; // HACK: ???
			NetPlayer p(pData.id, pData.pos);
			p.LoadTexture();
			GameLayer::GetNetworkPlayers().push_back(p);
			GameLayer::GetNetworkPlayers().back().LoadTexture();
		}
		break;
	}

	case PacketType::INIT_TERRAIN:
	{
		InitTerrainPacket& initTerrainPacket = *((InitTerrainPacket*)buf.Data()); // HACK: ???
		Island* islands = ((Island*)((uint8_t*)buf.Data() + sizeof(InitTerrainPacket)));
		for (uint32_t i = 0; i < initTerrainPacket.GetIslandCount(); i++)
		{
			Island is = islands[i];
			is.Init();
			GameLayer::Get().m_Terrain.AddIsland(is);
		}
		break;
	}

	case PacketType::UPDATE:
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

		case UpdateType::TERRAIN:
		{
			uint32_t count = updatePacket.GetDataSize() / sizeof(Island);
			for (uint32_t i = 0; i < count; i++)
			{
				Island is = ((Island*)((uint8_t*)buf.Data() + sizeof(UpdatePacket)))[i]; // HACK: ???
				is.Init();
				GameLayer::Get().m_Terrain.AddIsland(is);
			}

			break;
		}
		}
		break;
	}
	}
}