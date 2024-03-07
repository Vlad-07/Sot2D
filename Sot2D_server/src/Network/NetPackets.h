#pragma once

#include <Eis/Core/Buffer.h>


enum class PacketType : uint8_t
{
	NONE = 0, INITIAL, PLAYER_UPDATE
};

enum class UpdateType : uint8_t
{
	NONE = 0, MOVEMENT, CONNECT, DISCONNECT
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


class InitialPacket : public Packet
{
public:
	struct PlayerData
	{
		ClientId id;
		glm::vec2 pos;
	};

public:
	InitialPacket(uint32_t playerCount, const PlayerData* data) : Packet(PacketType::INITIAL), m_PlayerCount(playerCount), m_Data(nullptr)
	{
		if (playerCount == 0) return;

		EIS_ASSERT(data, "Invalid id array provided!");
		m_Data = new PlayerData[playerCount];
		memcpy(m_Data, data, playerCount * sizeof(PlayerData));
	}
	~InitialPacket()
	{
		delete[] m_Data;
	}

	uint32_t GetPlayerCount() const { return m_PlayerCount; }

	static Eis::Buffer CreateBuffer(const InitialPacket& m)
	{
		static Eis::Buffer b; // HACK: buffer destructor called twice a local object is returned (possibly)
		b.Allocate(sizeof(InitialPacket));
		b.Write(&m, sizeof(InitialPacket));
		if (m.m_Data == nullptr)
			return b;
		b.Resize(b.GetSize() + m.m_PlayerCount * sizeof(PlayerData));
		b.Write(m.m_Data, m.m_PlayerCount * sizeof(PlayerData), sizeof(InitialPacket));
		return b;
	}

private:
	uint32_t m_PlayerCount;
	PlayerData* m_Data;
};

class UpdatePacket : public Packet
{
public:
	UpdatePacket(UpdateType type, ClientId id, const void* data, uint32_t dataSize)
		: Packet(PacketType::PLAYER_UPDATE), m_UpdateType(type), m_Id(id), m_Data(nullptr), m_DataSize(dataSize)
	{
		if (dataSize == 0) return;

		EIS_ASSERT(data, "Invalid data provided!");
		m_Data = new uint8_t[dataSize];
		memcpy(m_Data, data, dataSize);
	}
	~UpdatePacket()
	{
		delete[] m_Data;
	}

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