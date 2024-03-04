#pragma once

#include <glm/glm.hpp>
#include <Eis/Networking/Server.h>

typedef uint16_t ClientId;

class NetClient
{
public:
	NetClient(ClientId id, Eis::ClientID netId) : m_Id(id), m_NetworkId(netId), m_Pos(0) {}
	~NetClient() = default;

	void SetPos(const glm::vec2& pos) { m_Pos = pos; }
	void AddPos(const glm::vec2& pos) { m_Pos += pos; }
	glm::vec2 GetPos() const { return m_Pos; }

	ClientId GetClientId() const { return m_Id; }
	Eis::ClientID GetNetworkId() const { return m_NetworkId; }

private:
	ClientId m_Id;
	Eis::ClientID m_NetworkId;
	glm::vec2 m_Pos;
};