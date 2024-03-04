#pragma once

#include <cstdint>
#include <Eis/Core/Buffer.h>

#include "NetClient.h"

enum class MessageType : uint8_t
{
	NONE = 0, INITIAL, PLAYER_UPDATE
};

enum class UpdateType : uint8_t
{
	NONE = 0, MOVEMENT, CONNECT, DISCONNECT
};

// Base message
class Message
{
public:
	Message(MessageType type) : m_Type(type) {}
	virtual ~Message() = default;

	// Any data that needs to be transmitted is appended to the messages buffer
//	static Eis::Buffer CreateBuffer() {} // should be static but then inheritance breaks

	MessageType GetType() const { return m_Type; }

private:
	MessageType m_Type;
};


class InitialMessage : public Message
{
public:
	InitialMessage(uint32_t playerCount, const ClientId* ids) : Message(MessageType::INITIAL), m_PlayerCount(playerCount), m_Ids(nullptr)
	{
		if (playerCount == 0) return;

		EIS_ASSERT(ids, "Invalid id array provided!");
		m_Ids = new ClientId[playerCount];
		memcpy(m_Ids, ids, playerCount * sizeof(ClientId));
	}
	~InitialMessage()
	{
		delete[] m_Ids;
	}

	uint32_t GetPlayerCount() const { return m_PlayerCount; }

	static Eis::Buffer CreateBuffer(const InitialMessage& m)
	{
		static Eis::Buffer b; // HACK: buffer destructor called twice if i return a local object (possibly)
		b.Allocate(sizeof(InitialMessage));
		b.Write(&m, sizeof(InitialMessage));
		if (m.m_Ids == nullptr)
			return b;
		b.Resize(b.GetSize() + m.m_PlayerCount * sizeof(ClientId));
		b.Write(m.m_Ids, m.m_PlayerCount * sizeof(ClientId), sizeof(InitialMessage));
		return b;
	}

private:
	uint32_t m_PlayerCount;
	ClientId* m_Ids;
};

class UpdateMessage : public Message
{
public:
	UpdateMessage(UpdateType type, ClientId id, const void* data, uint32_t dataSize)
		: Message(MessageType::PLAYER_UPDATE), m_UpdateType(type), m_Id(id), m_Data(nullptr), m_DataSize(dataSize)
	{
		if (dataSize == 0) return;

		EIS_ASSERT(data, "Invalid data provided!");
		m_Data = new uint8_t[dataSize];
		memcpy(m_Data, data, dataSize);
	}
	~UpdateMessage()
	{
		delete[] m_Data;
	}

	UpdateType GetUpdateType() const { return m_UpdateType; }
	ClientId GetClientId() const { return m_Id; }
 	uint32_t GetDataSize() const { return m_DataSize; }

	static Eis::Buffer CreateBuffer(const UpdateMessage& m)
	{
		static Eis::Buffer b; // HACK: buffer destructor called twice if i return a local object (possibly)
		b.Allocate(sizeof(UpdateMessage));
		b.Write(&m, sizeof(UpdateMessage));
		if (m.m_Data == nullptr)
			return b;
		b.Resize(b.GetSize() + m.m_DataSize);
		b.Write(m.m_Data, m.m_DataSize, sizeof(UpdateMessage));
		return b;
	}

private:
	UpdateType m_UpdateType;
	ClientId m_Id;
	void* m_Data;
	uint32_t m_DataSize;
};