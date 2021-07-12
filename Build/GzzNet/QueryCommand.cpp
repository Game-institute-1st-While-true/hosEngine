#include <stdio.h>
#include "QueryCommand.h"

gzz::QueryCommand::QueryCommand()
	:Type(-1), ParamCount(0), QueryText(L""), Cmd(nullptr)
{
}

gzz::QueryCommand::~QueryCommand()
{
	if (Cmd == nullptr)
	{
		return;
	}
	Cmd.Release();
	Cmd = nullptr;
}

void gzz::QueryCommand::SetQuery(std::wstring query, int type)
{
	QueryText = query;
	Type = type;
}