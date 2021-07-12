#include "Query.h"

gzz::Query::Query()
{
	Cmd = new QueryCommand();
}

gzz::Query::~Query()
{
	Rec.Close();
	if (Cmd != nullptr)
	{
		delete Cmd;
	}
}
