#pragma once

#include <Eis/Core/Buffer.h>
#include "../../Sot2D_client/src/Game/Island.h" // HACK

enum class PacketType : uint8_t
{
	NONE = 0, INIT_PLAYERS, /*INIT_TERRAIN,*/ UPDATE
};

enum class UpdateType : uint8_t
{
	NONE = 0, MOVEMENT, CONNECT, DISCONNECT, TERRAIN
};


// Base packet
class Packet
{
public:
	Packet(PacketType type) : m_Type(type) {}
	virtual ~Packet() = default;

	// Any data that needs to be transmitted is appended to the messages buffer
//	static Eis::Buffer CreateBuffer() {}

	PacketType GetType() const { return m_Type; }

private:
	PacketType m_Type;
};


class InitPlayersPacket : public Packet
{
public:
	struct PlayerData
	{
		ClientId id;
		glm::vec2 pos;
	};

public:
	InitPlayersPacket(uint32_t playerCount, PlayerData* data)
		: Packet(PacketType::INIT_PLAYERS), m_PlayerCount(playerCount), m_PlayerData(data) {}
	~InitPlayersPacket() = default;

	uint32_t GetPlayerCount() const { return m_PlayerCount; }

	static Eis::Buffer CreateBuffer(const InitPlayersPacket& m)
	{
		static Eis::Buffer b; // HACK: buffer destructor called twice a local object is returned (possibly)
		b.Allocate(sizeof(InitPlayersPacket));
		b.Write(&m, sizeof(InitPlayersPacket));
		if (m.m_PlayerData == nullptr)
			return b;
		b.Resize(b.GetSize() + m.m_PlayerCount * sizeof(PlayerData));
		b.Write(m.m_PlayerData, m.m_PlayerCount * sizeof(PlayerData), sizeof(InitPlayersPacket));
		return b;
	}

private:
	uint32_t m_PlayerCount;
	PlayerData* m_PlayerData;
};
/*
class InitTerrainPacket : public Packet
{
public:
	InitTerrainPacket(uint32_t islandCount, Island* data)
		: Packet(PacketType::INIT_TERRAIN), m_IslandCount(islandCount), m_IslandData(data) {}
	~InitTerrainPacket() = default;

	uint32_t GetIslandCount() const { return m_IslandCount; }

	static Eis::Buffer CreateBuffer(const InitTerrainPacket& m)
	{
		static Eis::Buffer b; // HACK: buffer destructor called twice a local object is returned (possibly)
		b.Allocate(sizeof(InitTerrainPacket));
		b.Write(&m, sizeof(InitTerrainPacket));
		if (m.m_IslandData == nullptr)
			return b;
		b.Resize(b.GetSize() + m.m_IslandCount * sizeof(Island));
		b.Write(m.m_IslandData, m.m_IslandCount  * sizeof(Island), sizeof(InitTerrainPacket));
		return b;
	}

private:
	uint32_t m_IslandCount;
	Island* m_IslandData;
};//*/


class UpdatePacket : public Packet
{
public:
	UpdatePacket(UpdateType type, ClientId id, void* data, uint32_t dataSize)
		: Packet(PacketType::UPDATE), m_UpdateType(type), m_Id(id), m_Data(data), m_DataSize(dataSize) {}
	~UpdatePacket() = default;

	UpdateType GetUpdateType() const { return m_UpdateType; }
	ClientId GetClientId() const { return m_Id; }
 	uint32_t GetDataSize() const { return m_DataSize; }

	static Eis::Buffer CreateBuffer(const UpdatePacket& m)
	{
		static Eis::Buffer b; // HACK: buffer destructor called twice a local object is returned (possibly)
		b.Allocate(sizeof(UpdatePacket));
		b.Write(&m, sizeof(UpdatePacket));
		if (m.m_Data == nullptr)
			return b;
		b.Resize(b.GetSize() + m.m_DataSize);
		b.Write(m.m_Data, m.m_DataSize, sizeof(UpdatePacket));
		return b;
	}

private:
	UpdateType m_UpdateType;
	ClientId m_Id;
	void* m_Data;
	uint32_t m_DataSize;
};