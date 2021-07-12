#pragma once
#include "NetDefine.h"

/// <summary>
/// [2020-11-02 ��ȿ��]
///	�����͸� ���� �� ��� �� �������̽�
/// Receive�� �Ϸ�ó���� ���.
/// </summary>

namespace gzz
{
	class IReceive abstract
	{
	public:
		IReceive() = default;
		virtual ~IReceive() = default;

	public:
		virtual int Receive(const SOCKET& sock, const LPOVERLAPPED& overraped) abstract;
		virtual PACKET GetPacket(const int completionBytes, const int recvBytes) abstract;
		virtual char* GetBuf() abstract;
		virtual void Release() abstract;
	};
}