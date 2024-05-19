#include "ServerLayer.h"
#include "NetPackets.h"


void ServerLayer::ClientConnectedCallback(const Eis::ClientInfo& info)
{
	EIS_PROFILE_FUNCTION();

	NetClient c(info.Id);

	// Send active players
	{
		ServerLayer::GetServer().SendDataToClient(info.Id, InitPlayersPacket((uint8_t)ServerLayer::GetClients().size()));
		for (uint32_t i = 0; i < ServerLayer::GetClients().size(); i++)
			ServerLayer::GetServer().SendDataToClient(info.Id, SetPlayerUpdatePacket(ServerLayer::GetClients()[i].GetNetworkId(),
																					 ServerLayer::GetClients()[i].GetPos()));
	}

	// Send terrain data
	{
		const std::vector<Island>& data = ServerLayer::GetWorldManager().GetInitialIslandsToSend();

		const InitTerrainPacket initPack((uint8_t)data.size(), c_WorldSize);
		ServerLayer::GetServer().SendDataToClient(info.Id, initPack);

		for (uint32_t i = 0; i < data.size(); i++)
		{
			const TerrainUpdatePacket packet(data[i]);
			ServerLayer::GetServer().SendDataToClient(info.Id, packet);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	// Inform everyone
	{
		EIS_INFO("{0} connected ({1})", c.GetNetworkId(), info.ConnectionDescription); // local log
		const NetworkPlayerUpdatePacket packet(c.GetNetworkId(), NetworkPlayerUpdateType::CONNECT);
		ServerLayer::GetServer().SendDataToAllClients(packet, info.Id);
	}

	ServerLayer::GetClients().push_back(c);
}


void ServerLayer::ClientDisconnectedCallback(const Eis::ClientInfo& info)
{
	EIS_PROFILE_FUNCTION();

	auto clientIt = ServerLayer::FindClientByNetId(info.Id);

	const NetworkPlayerUpdatePacket packet(clientIt->GetNetworkId(), NetworkPlayerUpdateType::DISCONNECT);
	ServerLayer::GetServer().SendDataToAllClients(packet, info.Id);

	ServerLayer::GetClients().erase(clientIt);
}


void ServerLayer::DataRecievedCallback(const Eis::ClientInfo& info, Eis::Buffer& buf)
{
	EIS_PROFILE_FUNCTION();

	const Packet& packet = buf.Read<Packet>();
	switch (packet.GetType())
	{
	case PacketType::NONE:
		EIS_WARN("Invalid message recieved! (PacketType::NONE)");
		break;

	case PacketType::INIT_PLAYERS:
	case PacketType::INIT_TERRAIN:
		EIS_WARN("Invalid message received! (PacketType recieved is sent by server only)");
		break;

	case PacketType::UPDATE:
	{
		const UpdatePacket& updateMsg = buf.Read<UpdatePacket>();

		switch (updateMsg.GetUpdateType())
		{
		case UpdateType::NONE:
			EIS_WARN("Invalid message received! (UpdatePacket - UpdateType::NONE)");
			break;

		case UpdateType::TERRAIN:
			EIS_WARN("Invalid message received! (PacketType recieved is sent by server only)");
			break;

		case UpdateType::PLAYER:
		{
			const PlayerUpdatePacket& playerUpdatePacket = buf.Read<PlayerUpdatePacket>();

			switch (playerUpdatePacket.GetPlayerUpdateType())
			{
			case PlayerUpdateType::NETWORK:
			case PlayerUpdateType::SET:
				EIS_WARN("Invalid message received! (PacketType recieved is sent by server only)");
				break;

			case PlayerUpdateType::MOVEMENT:
			{
				MovementPlayerUpdatePacket& movementPlayerPacket = buf.Read<MovementPlayerUpdatePacket>();

				// Update pos on server
				ServerLayer::GetClientByNetId(info.Id).SetPos(movementPlayerPacket.GetNewPos());

				// Forward packet to others
				movementPlayerPacket.SetClientId(info.Id);
				ServerLayer::GetServer().SendDataToAllClients(movementPlayerPacket, info.Id);

				break;
			}
			}
		}
		}
		break;
	}
	}
}