#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"

void ClientPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);
	PacketHeader header;
	br >> header;

	switch (header.id)
	{
	case S_TEST:
		Handle_S_TEST(buffer, len);
		break;

	default:
		break;
	}
}

#pragma pack(1)
// [고정데이터][가변데이터]
// [PKT_S_TEST][BuffListItem BuffListItem BuffListItem ...][victim..][victim...]
struct PKT_S_TEST
{
	struct BuffListItem
	{
		uint64 buffId;
		float remainTime;

		uint16 victimOffset;
		uint16 victimCount;

		bool Validate(BYTE* packetStart, uint16 packetSize, OUT uint32& size)
		{
			if (victimOffset + victimCount * sizeof(uint64) > packetSize)
				return false;

			size += victimCount * sizeof(uint64);
			return true;
		}
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

	bool Validate()
	{
		uint32 size = 0;
		size += sizeof(PKT_S_TEST);
		if (packetSize < size)
			return false;

		if (buffsOffset + buffsCount * sizeof(BuffListItem) > packetSize)
			return false;

		// Buffer 가변 데이터 크기 추가
		size += buffsCount * sizeof(BuffListItem);

		BuffList buffList = GetBuffList();
		for (int32 i = 0; i < buffList.Count(); i++)
		{
			if (buffList[i].Validate((BYTE*)this, packetSize, OUT size) == false)
				return false;
		}

		// 최종 크기 비교
		if (size != packetSize)
			return false;

		
		return true;
	}

	using BuffList = PacketList<PKT_S_TEST::BuffListItem>;
	using BuffVictimList = PacketList<uint64>;

	BuffList GetBuffList()
	{
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += buffsOffset;
		return BuffList(reinterpret_cast<PKT_S_TEST::BuffListItem*>(data), buffsCount);
	}

	BuffVictimList GetBuffVictimList(BuffListItem* buffItem)
	{
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += buffItem->victimOffset;
		return BuffVictimList(reinterpret_cast<uint64*>(data), buffItem->victimCount);
	}
};
#pragma pack()

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PKT_S_TEST* pkt = reinterpret_cast<PKT_S_TEST*>(buffer);
	
	if (pkt->Validate() == false)
		return;

	cout << "ID: " << pkt->id << ", HP: " << pkt->hp << ", ATK: " << pkt->attack << endl;

	PKT_S_TEST::BuffList buffs = pkt->GetBuffList();

	cout << "BuffCOunt: " << buffs.Count() << endl;

	for (auto& buff:buffs)
	{
		cout << "BuffInfo: " << buff.buffId << " / " << buff.remainTime << endl;

		PKT_S_TEST::BuffVictimList victims = pkt->GetBuffVictimList(&buff);
		
		cout << "Victim Count: " << victims.Count() << endl;
		for (auto& victim : victims)
		{
			cout << "Victim: " << victim << endl;
		}
	}
}
