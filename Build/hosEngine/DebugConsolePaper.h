#pragma once
#include <mutex>
#include "../hosUtility/hosUtility.h"

// [2021/01/25 ��ȿ��]
// ���� ��¼�ٺ���.. ��Ʈ��ũ �ΰſ� ���� �ΰŰ� �и��Ǿ���. (���� ������־��µ� �ڲ� �ٸ������ �Ǹ��� ��������.)
// �������̽��� �̿��Ͽ� Note�� Page�� �߰���Ű��.

// Logger Ŭ������ Output�� �Բ� ����Ѵ�.

using namespace hos::ut;

class DebugConsolePaper : public IPaper
{
private:

public:
	DebugConsolePaper() = default;
	virtual ~DebugConsolePaper() = default;

public:
	virtual void Print(std::wstring text)
	{
		Logger::GetIns()->OutSideLog(text);
	}
};