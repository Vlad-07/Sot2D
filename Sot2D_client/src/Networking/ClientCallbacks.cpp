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
	EIS_PROFILE_FUNCTION();

	Packet& packet = buf.Read<Packet>();
	switch (packet.GetType())
	{
	case PacketType::NONE:
		EIS_ASSERT(false, "Invalid packet received! (PacketType::NONE)");
		break;

	case PacketType::INIT_PLAYERS:
	{
		GameLayer::Get().InitSession();

		InitPlayersPacket& initPlayersPacket = buf.Read<InitPlayersPacket>();
		for (uint32_t i = 0; i < initPlayersPacket.GetPlayerCount(); i++)
		{
			InitPlayersPacket::PlayerData pData = buf.Read<InitPlayersPacket::PlayerData>(sizeof(InitPlayersPacket) + i * sizeof(InitPlayersPacket::PlayerData));
			NetPlayer p(pData.id, pData.pos);
			p.LoadTexture();
			GameLayer::GetNetworkPlayers().push_back(p);
			GameLayer::GetNetworkPlayers().back().LoadTexture();
		}
		break;
	}
	/*
	case PacketType::INIT_TERRAIN:
	{
		InitTerrainPacket& initTerrainPacket = buf.Read<InitTerrainPacket>();
		for (uint32_t i = 0; i < initTerrainPacket.GetIslandCount(); i++)
		{
			Island is = *(Island*)((uint8_t*)buf.Data() + (sizeof(InitTerrainPacket) + i * sizeof(Island)));
			is.Init();
			GameLayer::Get().m_Terrain.AddIsland(is);
		}
		break;
	}//*/

	case PacketType::UPDATE:
	{
		UpdatePacket& updatePacket = buf.Read<UpdatePacket>();
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
				buf.Read<glm::vec2>(sizeof(UpdatePacket))
			);
			break;
		}

		case UpdateType::TERRAIN:
		{
			GameLayer::Get().m_Terrain.m_TerrainMtx.lock();
			if (updatePacket.GetClientId())
				GameLayer::Get().m_Terrain.Reserve(updatePacket.GetClientId());

			Island is = buf.Read<Island>(sizeof(UpdatePacket));
			GameLayer::Get().m_Terrain.AddIsland(is);
			GameLayer::Get().m_Terrain.m_TerrainMtx.unlock();
			break;
		}
		}
		break;
	}
	}
}