#pragma once

#include <Eis/Core/Buffer.h>
#include <Eis/Networking/Server.h>
#include "../../Sot2D_client/src/Game/Island.h" // HACK


enum class PacketType : uint8_t
{
	NONE = 0, INIT_PLAYERS, INIT_TERRAIN, UPDATE
};

enum class UpdateType : uint8_t
{
	NONE = 0, PLAYER, TERRAIN
};

enum class PlayerUpdateType
{
	NETWORK, MOVEMENT, SET
};

enum class NetworkPlayerUpdateType : uint8_t
{
	CONNECT, DISCONNECT
};


// Base packet
class Packet
{
public:
	Packet(PacketType type) : m_Type(type) {}
	virtual ~Packet() = default;

	PacketType GetType() const { return m_Type; }

private:
	PacketType m_Type;
};



class InitPlayersPacket : public Packet
{
public:
	InitPlayersPacket(uint8_t playerCount)
		: Packet(PacketType::INIT_PLAYERS), m_PlayerCount(playerCount) {}

	uint8_t GetPlayerCount() const { return m_PlayerCount; }

private:
	uint8_t m_PlayerCount;
};

class InitTerrainPacket : public Packet
{
public:
	InitTerrainPacket(uint8_t islandCount, float worldSize)
		: Packet(PacketType::INIT_TERRAIN), m_IslandCount(islandCount), m_WorldSize(worldSize) {}

	uint8_t GetIslandCount() const { return m_IslandCount; }
	float GetWorldSize() const { return m_WorldSize; }

private:
	uint8_t m_IslandCount;
	float m_WorldSize;
};



class UpdatePacket : public Packet
{
public:
	UpdatePacket(UpdateType type) : Packet(PacketType::UPDATE), m_UpdateType(type) {}
	virtual ~UpdatePacket() = default;

	UpdateType GetUpdateType() const { return m_UpdateType; }

private:
	UpdateType m_UpdateType;
};


class PlayerUpdatePacket : public UpdatePacket
{
public:
	PlayerUpdatePacket(PlayerUpdateType pUpType, Eis::ClientID id)
		: UpdatePacket(UpdateType::PLAYER), m_PlayerUpdateType(pUpType), m_ClientId(id) {}
	virtual ~PlayerUpdatePacket() = default;

	PlayerUpdateType GetPlayerUpdateType() const { return m_PlayerUpdateType; }
	Eis::ClientID GetClientId() const { return m_ClientId; }

private:
	PlayerUpdateType m_PlayerUpdateType;

protected:
	Eis::ClientID m_ClientId;
};

class NetworkPlayerUpdatePacket : public PlayerUpdatePacket
{
public:
	NetworkPlayerUpdatePacket(Eis::ClientID id, NetworkPlayerUpdateType type)
		: PlayerUpdatePacket(PlayerUpdateType::NETWORK, id), m_NetUpdateType(type) {}

	NetworkPlayerUpdateType GetNetUpdateType() const { return m_NetUpdateType; }

private:
	NetworkPlayerUpdateType m_NetUpdateType;
};

class MovementPlayerUpdatePacket : public PlayerUpdatePacket
{
public:
	MovementPlayerUpdatePacket(Eis::ClientID id, const glm::vec2& newPos)
		: PlayerUpdatePacket(PlayerUpdateType::MOVEMENT, id), m_NewPos(newPos) {}

	// Facilitate packet forwarding
	void SetClientId(Eis::ClientID id) { m_ClientId = id; }

	glm::vec2 GetNewPos() const { return m_NewPos; }

private:
	glm::vec2 m_NewPos;
};

class SetPlayerUpdatePacket : public PlayerUpdatePacket
{
public:
	SetPlayerUpdatePacket(Eis::ClientID id, const glm::vec2& pos)
		: PlayerUpdatePacket(PlayerUpdateType::SET, id), m_Pos(pos) {}

	glm::vec2 GetPos() const { return m_Pos; }

private:
	glm::vec2 m_Pos;
};


class TerrainUpdatePacket : public UpdatePacket
{
public:
	TerrainUpdatePacket(const Island& is)
		: UpdatePacket(UpdateType::TERRAIN), m_Island(is) {}

	Island GetIsland() const { return m_Island; }

private:
	Island m_Island;
};