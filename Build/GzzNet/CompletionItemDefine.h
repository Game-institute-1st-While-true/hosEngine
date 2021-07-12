#pragma once
#include "NetDefine.h"
#include "ICompletionItem.h"
#include <Windows.h>

// �ý��� ��Ŷ�� Net Form���� Logic�ʿ� �����ϴ� Message�̴�. 
// ���� ������ ���۰� ��, �׸��� � ���� ID�� � �ൿ�� �ߴ��� (����, ����, ����)�� �����Ѵ�.
// �׷��Ƿ� ��Ŷ ������ ���, ID�� ������.
// �������� ��Ŷ���� ���������� ��Ŷ�� ������� �׻� PACKETHEADERSIZE + SOCK_ID_SIZE �̴�.
// [--------- HEADER ---------][--- SOCKID ---]

namespace gzz
{
	constexpr int SYSTEM_PACKET_SIZE = PACKET_HEADER_SIZE + 2;
	constexpr int MAX_CONNECT_TRY_COUNT = 3;

#pragma pack(push, 1)
	struct PACKAGE
	{
		PACKAGE() = default;
		PACKAGE(int id, PACKET pack) { this->id = id; packet = pack; }
		PACKET packet;
		int id;
	};
#pragma pack(pop)

	enum class SYSTEM_PROTOCOL : unsigned char
	{
		NONE = 0,

		ACCEPT,
		CONNECT,
		CONNECT_TRY,
		CONNECT_FAIL,
		DISCONNECT_ACCEPT,
		DISCONNECT_CONNECT,
		JOB,

		END,

		MAX = 0xff
	};
}