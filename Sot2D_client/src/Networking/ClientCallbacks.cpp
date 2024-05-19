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
		return;

	case PacketType::INIT_PLAYERS:
	{
		const InitPlayersPacket& initPlayersPacket = buf.Read<InitPlayersPacket>();

		// Clear existing
		GameLayer::GetNetworkPlayers().clear();

		// Set new count
		GameLayer::GetNetworkPlayers().reserve(initPlayersPacket.GetPlayerCount());

		return;
	}

	case PacketType::INIT_TERRAIN:
	{
		const InitTerrainPacket& initTerrainPacket = buf.Read<InitTerrainPacket>();

		// Clear existing
		GameLayer::GetWorld().Clear();

		// Set new stats
		GameLayer::GetWorld().SetWorldSize(initTerrainPacket.GetWorldSize());
		GameLayer::GetWorld().Reserve(initTerrainPacket.GetIslandCount());

		return;
	}

	case PacketType::UPDATE:
	{
		const UpdatePacket& updateMsg = buf.Read<UpdatePacket>();

		switch (updateMsg.GetUpdateType())
		{
		case UpdateType::NONE:
			EIS_WARN("Invalid message received! (UpdatePacket - UpdateType::NONE)");
			return;

		case UpdateType::PLAYER:
		{
			const PlayerUpdatePacket& playerUpdatePacket = buf.Read<PlayerUpdatePacket>();
			switch (playerUpdatePacket.GetPlayerUpdateType())
			{
			case PlayerUpdateType::NETWORK:
			{
				const NetworkPlayerUpdatePacket& networkPlayerUpdatePacket = buf.Read<NetworkPlayerUpdatePacket>();

				if (networkPlayerUpdatePacket.GetNetUpdateType() == NetworkPlayerUpdateType::CONNECT)
					GameLayer::GetNetworkPlayers().emplace_back(networkPlayerUpdatePacket.GetClientId());
				else if (networkPlayerUpdatePacket.GetNetUpdateType() == NetworkPlayerUpdateType::DISCONNECT)
					GameLayer::GetNetworkPlayers().erase(
						GameLayer::Get().FindClientById(networkPlayerUpdatePacket.GetClientId())
					);
				return;
			}
			case PlayerUpdateType::SET:
			{
				// TODO: check if init players packet was recieved

				const SetPlayerUpdatePacket& setPlayerUpdatePacket = buf.Read<SetPlayerUpdatePacket>();

				GameLayer::GetNetworkPlayers().emplace_back(setPlayerUpdatePacket.GetClientId(), setPlayerUpdatePacket.GetPos());

				return;
			}
			case PlayerUpdateType::MOVEMENT:
			{
				const MovementPlayerUpdatePacket& movementPlayerUpdatePacket = buf.Read<MovementPlayerUpdatePacket>();

				GameLayer::Get().FindClientById(movementPlayerUpdatePacket.GetClientId())
								->SetActualPos(movementPlayerUpdatePacket.GetNewPos());

				return;
			}
			}
		}
		case UpdateType::TERRAIN:
		{
			const TerrainUpdatePacket& terrainUpdatePacket = buf.Read<TerrainUpdatePacket>();

			GameLayer::GetWorld().AddIsland(terrainUpdatePacket.GetIsland());

			return;
		}
		}
	}
	}

	EIS_ERROR("Unknown packet recieved!");
}