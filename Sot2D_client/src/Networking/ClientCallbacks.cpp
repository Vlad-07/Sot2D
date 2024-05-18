#include "GameLayer.h"

#include "../../Sot2D_server/src/Network/NetPackets.h" // HACK


void GameLayer::ConnectedCallback() {}
void GameLayer::DisconnectedCallback() {}


void GameLayer::DataReceivedCallback(Eis::Buffer& buf)
{
	EIS_PROFILE_FUNCTION();

	const Packet& packet = buf.Read<Packet>();
	switch (packet.GetType())
	{
	case PacketType::NONE:
		EIS_WARN("Invalid packet received! (PacketType::NONE)");
		break;

	case PacketType::INIT_PLAYERS:
	{
		const InitPlayersPacket& initPlayersPacket = buf.Read<InitPlayersPacket>();
		for (uint32_t i = 0; i < initPlayersPacket.GetPlayerCount(); i++)
		{
			const InitPlayersPacket::PlayerData pData = buf.Read<InitPlayersPacket::PlayerData>(sizeof(InitPlayersPacket) + i * sizeof(InitPlayersPacket::PlayerData));
			GameLayer::GetNetworkPlayers().emplace_back(pData.id, pData.pos);
		}
		break;
	}

	case PacketType::INIT_TERRAIN:
	{
		const InitTerrainPacket& initPlayersPacket = buf.Read<InitTerrainPacket>();
		GameLayer::GetWorld().SetWorldSize(initPlayersPacket.GetWorldSize());
		GameLayer::GetWorld().Reserve(initPlayersPacket.GetIslandCount());
		break;
	}

	case PacketType::UPDATE:
	{
		const UpdatePacket& updatePacket = buf.Read<UpdatePacket>();
		switch (updatePacket.GetUpdateType())
		{
		case UpdateType::NONE:
			EIS_WARN("Invalid message received! (UpdateMessage - UpdateType::NONE)");
			break;

		case UpdateType::CONNECT:
			GameLayer::GetNetworkPlayers().emplace_back(updatePacket.GetClientId());
			break;

		case UpdateType::DISCONNECT:
			GameLayer::GetNetworkPlayers().erase(GameLayer::Get().FindClientById(updatePacket.GetClientId()));
			break;

		case UpdateType::MOVEMENT:
			GameLayer::Get().GetClientById(updatePacket.GetClientId()).SetActualPos(
				buf.Read<glm::vec2>(sizeof(UpdatePacket))
			);
			break;

		case UpdateType::TERRAIN:
		{
			GameLayer::GetWorld().m_TerrainMtx.lock();

			const Island& is = buf.Read<Island>(sizeof(UpdatePacket));
			GameLayer::GetWorld().AddIsland(is);

			GameLayer::GetWorld().m_TerrainMtx.unlock();
			break;
		}
		}
		break;
	}
	}
}