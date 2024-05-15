#pragma once

#include <glm/glm.hpp>
#include <Eis/Networking/Server.h>


class NetClient
{
public:
	NetClient(Eis::ClientID netId) : m_NetworkId(netId), m_Pos() {}
	~NetClient() = default;

	void SetPos(const glm::vec2& pos) { m_Pos = pos; }
	void AddPos(const glm::vec2& pos) { m_Pos += pos; }
	glm::vec2 GetPos() const { return m_Pos; }

	Eis::ClientID GetNetworkId() const { return m_NetworkId; }

private:
	Eis::ClientID m_NetworkId;
	glm::vec2 m_Pos;
};