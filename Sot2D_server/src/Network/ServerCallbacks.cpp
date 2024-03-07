#include "ServerLayer.h"
#include "NetPackets.h"


void ServerLayer::ClientConnectedCallback(const Eis::ClientInfo& info)
{
	static ClientId globalIdIncrementor = 0;

	if (ServerLayer::GetClients().size() == 0) globalIdIncrementor = 0;

	NetClient c(globalIdIncrementor++, info.Id);

	EIS_INFO("{0} connected ({1})", c.GetClientId(), info.ConnectionDescription);

	// Inform everyone
	UpdatePacket m(UpdateType::CONNECT, c.GetClientId(), nullptr, 0);
	ServerLayer::Get().m_Server.SendBufferToAllClients(UpdatePacket::CreateBuffer(m), info.Id);

	if (ServerLayer::GetClients().size() > 0)
	{
		InitialPacket::PlayerData* data = (InitialPacket::PlayerData*)_malloca(ServerLayer::GetClients().size() * sizeof(InitialPacket::PlayerData));
		for (uint32_t i = 0; i < ServerLayer::GetClients().size(); i++)
			data[i] = { ServerLayer::GetClients()[i].GetClientId(), ServerLayer::GetClients()[i].GetPos() };
		InitialPacket packet(ServerLayer::GetClients().size(), data);
		_freea(data);
		ServerLayer::Get().m_Server.SendBufferToClient(info.Id, InitialPacket::CreateBuffer(packet));
	}
	else
		ServerLayer::Get().m_Server.SendBufferToClient(info.Id, InitialPacket::CreateBuffer(InitialPacket(0, nullptr)));

	ServerLayer::GetClients().push_back(c);
}


void ServerLayer::ClientDisconnectedCallback(const Eis::ClientInfo& info)
{
	auto clientIt = ServerLayer::FindClientByNetId(info.Id);

	UpdatePacket m(UpdateType::DISCONNECT, clientIt->GetClientId(), nullptr, 0);
	ServerLayer::Get().m_Server.SendBufferToAllClients(UpdatePacket::CreateBuffer(m), info.Id);

	ServerLayer::GetClients().erase(clientIt);
}


void ServerLayer::DataRecievedCallback(const Eis::ClientInfo& info, Eis::Buffer& buf)
{
	Packet& msg = *((Packet*)buf.Data()); // HACK: ???
	switch (msg.GetType())
	{
	case PacketType::NONE:
		EIS_ASSERT(false, "Invalid message recieved! (PacketType::NONE)");
		break;

	case PacketType::PLAYER_UPDATE:
	{
		UpdatePacket& updateMsg = *((UpdatePacket*)buf.Data()); // HACK: ???

		switch (updateMsg.GetUpdateType())
		{
		case UpdateType::NONE:
			EIS_ASSERT(false, "Invalid message received! (UpdatePacket - UpdateType::NONE)");
			break;

		case UpdateType::CONNECT:
		case UpdateType::DISCONNECT:
			// Only the server should send these messages
			EIS_ASSERT(false, "Invalid message received! (UpdatePacket - UpdateType::CONNECT/DISCONNECT)");
			break;

		case UpdateType::MOVEMENT:
		{
			NetClient& client = ServerLayer::GetClientByNetId(info.Id);
			client.SetPos(*((glm::vec2*)((uint8_t*)buf.Data() + sizeof(UpdatePacket)))); // HACK: ???
			UpdatePacket m(
				UpdateType::MOVEMENT,
				client.GetClientId(),
				&client.GetPos(),
				sizeof(glm::vec2));
			ServerLayer::Get().m_Server.SendBufferToAllClients(UpdatePacket::CreateBuffer(m), info.Id);
		}
		}
		break;
	}
	}
}