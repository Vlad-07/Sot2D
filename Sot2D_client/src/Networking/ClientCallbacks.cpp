#include "GameLayer.h"

#include "../../Sot2D_server/src/Network/NetPackets.h" // HACK


void GameLayer::ConnectedCallback()
{
	GameLayer::Get().InitSession();
}

void GameLayer::DisconnectedCallback()
{
	GameLayer::Get().Cleanup();
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

			GameLayer::GetNetworkPlayers().emplace_back(pData.id, pData.pos);
		}
		break;
	}

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
			GameLayer::GetNetworkPlayers().emplace_back(updatePacket.GetClientId());
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
			GameLayer::GetTerrain().m_TerrainMtx.lock();
			if (updatePacket.GetClientId())
				GameLayer::GetTerrain().Reserve(updatePacket.GetClientId());

			Island is = buf.Read<Island>(sizeof(UpdatePacket));
			GameLayer::GetTerrain().AddIsland(is);
			GameLayer::GetTerrain().m_TerrainMtx.unlock();
			break;
		}
		}
		break;
	}
	}
}