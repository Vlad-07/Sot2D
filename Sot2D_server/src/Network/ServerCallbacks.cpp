#include "ServerLayer.h"
#include "NetPackets.h"


void ServerLayer::ClientConnectedCallback(const Eis::ClientInfo& info)
{
	EIS_PROFILE_FUNCTION();

	static ClientId globalIdIncrementor = 0;
	if (ServerLayer::GetClients().size() == 0) globalIdIncrementor = 0;

	NetClient c(globalIdIncrementor++, info.Id);

	// Inform everyone
	{
		EIS_INFO("{0} connected ({1})", c.GetClientId(), info.ConnectionDescription); // local included
		UpdatePacket packet(UpdateType::CONNECT, c.GetClientId(), nullptr, 0);
		ServerLayer::Get().m_Server.SendBufferToAllClients(UpdatePacket::CreateBuffer(packet), info.Id);
	}

	// Send active players
	{
		if (ServerLayer::GetClients().size() > 0)
		{
			InitPlayersPacket::PlayerData* data = (InitPlayersPacket::PlayerData*)_malloca(ServerLayer::GetClients().size() * sizeof(InitPlayersPacket::PlayerData));
			for (uint32_t i = 0; i < ServerLayer::GetClients().size(); i++)
				data[i] = { ServerLayer::GetClients()[i].GetClientId(), ServerLayer::GetClients()[i].GetPos() };
			InitPlayersPacket packet(ServerLayer::GetClients().size(), data);
			_freea(data);
			ServerLayer::Get().m_Server.SendBufferToClient(info.Id, InitPlayersPacket::CreateBuffer(packet));
		}
		else
			ServerLayer::Get().m_Server.SendBufferToClient(info.Id, InitPlayersPacket::CreateBuffer(InitPlayersPacket(0, nullptr)));
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	// Send terrain data
	{
		uint32_t nr = ServerLayer::Get().m_TerrainManager.GetInitialIslandsNrToSend();
		std::vector<Island>& data = ServerLayer::Get().m_TerrainManager.GetInitialIslandsToSend();
		for (uint32_t i = 0; i < nr; i++)
		{
			UpdatePacket packet(UpdateType::TERRAIN, (i == 0 ? nr : 0), &data[i], sizeof(Island));
			ServerLayer::Get().m_Server.SendBufferToClient(info.Id, UpdatePacket::CreateBuffer(packet));
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	ServerLayer::GetClients().push_back(c);
}


void ServerLayer::ClientDisconnectedCallback(const Eis::ClientInfo& info)
{
	EIS_PROFILE_FUNCTION();

	auto clientIt = ServerLayer::FindClientByNetId(info.Id);

	UpdatePacket m(UpdateType::DISCONNECT, clientIt->GetClientId(), nullptr, 0);
	ServerLayer::Get().m_Server.SendBufferToAllClients(UpdatePacket::CreateBuffer(m), info.Id);

	ServerLayer::GetClients().erase(clientIt);
}


void ServerLayer::DataRecievedCallback(const Eis::ClientInfo& info, Eis::Buffer& buf)
{
	EIS_PROFILE_FUNCTION();

	Packet& msg = *((Packet*)buf.Data()); // HACK: ???
	switch (msg.GetType())
	{
	case PacketType::NONE:
		EIS_ASSERT(false, "Invalid message recieved! (PacketType::NONE)");
		break;

	case PacketType::INIT_PLAYERS:
//	case PacketType::INIT_TERRAIN:
		EIS_ASSERT(false, "Invalid message received! (PacketType recieved is sent by server only)");
		break;

	case PacketType::UPDATE:
	{
		UpdatePacket& updateMsg = *((UpdatePacket*)buf.Data()); // HACK: ???

		switch (updateMsg.GetUpdateType())
		{
		case UpdateType::NONE:
			EIS_ASSERT(false, "Invalid message received! (UpdatePacket - UpdateType::NONE)");
			break;

		case UpdateType::CONNECT:
		case UpdateType::DISCONNECT:
		case UpdateType::TERRAIN:
			// Only the server should send these messages
			EIS_ASSERT(false, "Invalid message received! (UpdatePacketType recieved is sent by server only)");
			break;

		case UpdateType::MOVEMENT:
		{
			// Update pos
			NetClient& client = ServerLayer::GetClientByNetId(info.Id);
			glm::vec2 newPos = *((glm::vec2*)((uint8_t*)buf.Data() + sizeof(UpdatePacket))); // HACK: ???
			client.SetPos(newPos);

			UpdatePacket movementPacket(
				UpdateType::MOVEMENT,
				client.GetClientId(),
				&newPos,
				sizeof(glm::vec2));
			ServerLayer::Get().m_Server.SendBufferToAllClients(UpdatePacket::CreateBuffer(movementPacket), info.Id);

			// Update and send terrain data
		/*	uint32_t count = 0;
			Island* data = ServerLayer::Get().m_TerrainManager.GenerateNewIslands(client.GetPos(), count);

			if (count == 0) break;

			UpdatePacket terrainPacket(UpdateType::TERRAIN, 0, data, count * sizeof(Island));
			ServerLayer::Get().m_Server.SendBufferToClient(info.Id, UpdatePacket::CreateBuffer(terrainPacket));
			delete[] data;*/
		}
		}
		break;
	}
	}
}