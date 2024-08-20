#pragma once
#include "BufferReader.h"
#include "BufferWriter.h"

class ServerPacketHandler
{
public:
	static void HandlePacket(BYTE* buffer, int32 len);
};

enum
{
	S_TEST = 1,
};

template<typename T, typename C>
class PacketIterator
{
public:
	PacketIterator(C& container, uint16 index) :_container(container), _index(index) {};

	bool operator!=(const PacketIterator& other) { return _index != other._index; }
	const T& operator*() const { return _container[_index]; }
	T& operator*() { return _container[_index]; }
	T* operator->() { return &_container[_index]; }
	PacketIterator& operator++()
	{
		_index++;
		return *this;
	}
	PacketIterator operator++(int32)
	{
		PacketIterator ret = *this;
		++_index;
		return ret;
	}

private:
	C& _container;
	uint16 _index;
};

template<typename T>
class PacketList
{
public:
	PacketList() :_data(nullptr), _count(0) {}
	PacketList(T* data, uint16 count) :_data(data), _count(count) {}
	~PacketList() {}

	T& operator[](uint16 index)
	{
		ASSERT_CRASH(index < _count);
		return _data[index];
	}

	uint16 Count() { return _count; }

	PacketIterator<T, PacketList<T>> begin() { return PacketIterator<T, PacketList<T>>(*this, 0); }
	PacketIterator<T, PacketList<T>> end() { return PacketIterator<T, PacketList<T>>(*this, _count); }

private:
	T* _data;
	uint16 _count;
};


#pragma pack(1)
// [고정데이터][가변데이터]
// [PKT_S_TEST][BuffListItem BuffListItem BuffListItem ...]
struct PKT_S_TEST
{
	struct BuffListItem
	{
		uint64 buffId;
		float remainTime;

		uint16 victimOffset;
		uint16 victimCount;
	};

	// Header
	uint16 packetSize;
	uint16 packetId;

	// Data
	uint64 id;
	uint32 hp;
	uint16 attack;

	/*
		가변 데이터
		1) 문자열 - 닉네임
		2) 바이트 배열 - 길드 이미지
		3) 일반 리스트
	*/
	uint16 buffsOffset; // 가변 데이터의 시작 주소 위치
	uint16 buffsCount; // 가변 데이터 갯수
};

class PKT_S_TEST_WRITE
{
public:
	using BuffListItem = PKT_S_TEST::BuffListItem;
	using BuffList = PacketList<PKT_S_TEST::BuffListItem>;
	using BuffVictimList = PacketList<uint64>;

	PKT_S_TEST_WRITE(uint64 id, uint32 hp, uint16 attack)
	{
		_sendBuffer = GSendBufferManager->Open(4096);
		_bw = BufferWriter(_sendBuffer->Buffer(), _sendBuffer->AllocSize());

		_pkt = _bw.Reserve<PKT_S_TEST>();
		_pkt->packetSize = 0;
		_pkt->packetId = S_TEST;
		_pkt->id = id;
		_pkt->hp = hp;
		_pkt->attack = attack;
		_pkt->buffsOffset = 0;
		_pkt->buffsCount = 0;
	}

	BuffList ReserveBuffList(uint16 buffCount)
	{
		BuffListItem* firstBuffListItem = _bw.Reserve<BuffListItem>(buffCount);
		_pkt->buffsCount = buffCount;
		_pkt->buffsOffset = (uint64)firstBuffListItem - (uint64)_pkt;

		return BuffList(firstBuffListItem, buffCount);
	}

	BuffVictimList ReserveBuffVictimList(BuffListItem* buffItem, uint16 victimCount)
	{
		// 가변 패킷 할당
		uint64* firstVictimListItem = _bw.Reserve<uint64>(victimCount);
		buffItem->victimCount = victimCount;
		buffItem->victimOffset = (uint64)firstVictimListItem - (uint64)_pkt;

		return BuffVictimList(firstVictimListItem, victimCount);
	}

	SendBufferRef CloseAndReturn()
	{
		// 패킷 사이즈 계산
		_pkt->packetSize = _bw.WriteSize();
		_sendBuffer->Close(_bw.WriteSize());
		return _sendBuffer;
		
	}

private:
	PKT_S_TEST* _pkt = nullptr;
	SendBufferRef _sendBuffer;
	BufferWriter _bw;
};
#pragma pack()