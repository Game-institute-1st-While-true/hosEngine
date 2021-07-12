#pragma once
#include "NetDefine.h"

/// <summary>
/// [2020-11-02 ��ȿ��]
///	�����͸� ���� �� ����� �������̽�.
/// Send�� �Ϸ�ó���� ���.
/// </summary>

namespace gzz
{
	class ISend abstract
	{
	public:
		ISend() = default;
		virtual ~ISend() = default;

	public:
		virtual int SendMsg(const SOCKET& sock, const LPOVERLAPPED& overraped, PACKET& packet) abstract;
		//virtual int RemainingMsg(const SOCKET& sock, const LPOVERLAPPED& overraped) abstract;
		virtual int SendProcess(const int completionBytes) abstract;
		virtual void Release() abstract;
	};
}