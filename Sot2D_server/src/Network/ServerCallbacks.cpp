#include "ServerLayer.h"
#include "NetPackets.h"


void ServerLayer::ClientConnectedCallback(const Eis::ClientInfo& info)
{
	EIS_PROFILE_FUNCTION();

	NetClient c(info.Id);

	// Inform everyone
	{
		EIS_INFO("{0} connected ({1})", c.GetNetworkId(), info.ConnectionDescription); // local log
		const UpdatePacket packet(UpdateType::CONNECT, c.GetNetworkId(), nullptr, 0);
		ServerLayer::GetServer().SendBufferToAllClients(UpdatePacket::CreateBuffer(packet), info.Id);
	}

	// Send active players
	{
		if (ServerLayer::GetClients().size() > 0)
		{
			InitPlayersPacket::PlayerData* data = (InitPlayersPacket::PlayerData*)_malloca(ServerLayer::GetClients().size() * sizeof(InitPlayersPacket::PlayerData));
			for (uint32_t i = 0; i < ServerLayer::GetClients().size(); i++)
				data[i] = { ServerLayer::GetClients()[i].GetNetworkId(), ServerLayer::GetClients()[i].GetPos() };
			const InitPlayersPacket packet((uint32_t)ServerLayer::GetClients().size(), data);
			ServerLayer::GetServer().SendBufferToClient(info.Id, InitPlayersPacket::CreateBuffer(packet));
			_freea(data);
		}
		else
			ServerLayer::GetServer().SendBufferToClient(info.Id, InitPlayersPacket::CreateBuffer(InitPlayersPacket(0, nullptr)));
	}

	// Send terrain data
	{
		const std::vector<Island>& data = ServerLayer::Get().m_TerrainManager.GetInitialIslandsToSend();

		const InitTerrainPacket initPack((Eis::ClientID)data.size(), c_WorldSize);
		ServerLayer::GetServer().SendBufferToClient(info.Id, InitTerrainPacket::CreateBuffer(initPack));

		for (uint32_t i = 0; i < data.size(); i++)
		{
			const UpdatePacket packet(UpdateType::TERRAIN, 0, &data[i], sizeof(Island));
			ServerLayer::GetServer().SendBufferToClient(info.Id, UpdatePacket::CreateBuffer(packet));
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	ServerLayer::GetClients().push_back(c);
}


void ServerLayer::ClientDisconnectedCallback(const Eis::ClientInfo& info)
{
	EIS_PROFILE_FUNCTION();

	auto clientIt = ServerLayer::FindClientByNetId(info.Id);

	const UpdatePacket packet(UpdateType::DISCONNECT, clientIt->GetNetworkId(), nullptr, 0);
	ServerLayer::GetServer().SendBufferToAllClients(UpdatePacket::CreateBuffer(packet), info.Id);

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

		case UpdateType::CONNECT:
		case UpdateType::DISCONNECT:
		case UpdateType::TERRAIN:
			// Only the server should send these messages
			EIS_WARN("Invalid message received! (UpdatePacketType recieved is sent by server only)");
			break;

		case UpdateType::MOVEMENT:
		{
			// Update pos
			NetClient& client = ServerLayer::GetClientByNetId(info.Id);
			const glm::vec2 newPos = buf.Read<glm::vec2>(sizeof(UpdatePacket));
			client.SetPos(newPos);

			const UpdatePacket movementPacket(
				UpdateType::MOVEMENT,
				client.GetNetworkId(),
				&newPos,
				sizeof(glm::vec2));
			ServerLayer::GetServer().SendBufferToAllClients(UpdatePacket::CreateBuffer(movementPacket), info.Id);
		}
		}
		break;
	}
	}
}