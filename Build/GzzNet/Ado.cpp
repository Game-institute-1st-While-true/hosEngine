#include "Ado.h"
#include "Note.h"
#include "DBManager.h"

gzz::Ado::Ado(int id, DBManager* manager)
	:IDataBase(id, manager), Name(L"GonDB")
{
	HRESULT res = ::CoInitialize(nullptr);

	switch (res)
	{
	case S_OK:
	case S_FALSE:
		Note::GetIns()->Write(L"ADO : ", __FUNCTIONW__, L"   CoInitialize Sucess");
		break;
	case RPC_E_CHANGED_MODE:
		Note::GetIns()->Write(L"ADO : ", __FUNCTIONW__, L"   CoInitialize Fail");
	}


	Connection.CreateInstance(__uuidof(ADODB::Connection));
	if (Connection == nullptr)
	{
		Note::GetIns()->Write(L"ADO : ", __FUNCTIONW__, L"   Db Init Fail..");
	}
	State = STOP;
	IsRun.store(false);
	SetConnectTimeOut(DEFAULT_TIMEOUT);
}

gzz::Ado::~Ado()
{
	DestroyThread();
	Disconnect();
	if (Connection)
	{
		Connection.Release();
	}
	::CoUninitialize();
}

bool gzz::Ado::Connect()
{
	Connection->ConnectionString = ConnectString.c_str();
	if (!Connection) return false;
	try
	{
		HRESULT hr = Connection->Open(ConnectString.c_str(), "", "", NULL);
		if (SUCCEEDED(hr))
		{
			Connection->put_CursorLocation(ADODB::adUseClient);
			Note::GetIns()->Write(L"ADO : ", __FUNCTIONW__, L"   Db Connect Sucess");
			State = STANDBY;
			return true;
		}
	}
	catch (_com_error& e)
	{
		ErrorReport(L"Connect", e);
	}
	return false;
}

bool gzz::Ado::Disconnect()
{
	if (!Connection) return false;
	if (State == STOP) return true;

	try
	{
		Excute();
		if (!Connection) return true;
		Connection->Close();
		State = STOP;
		ConnectString.clear();
		Name.clear();
		return true;
	}
	catch (...)
	{
		Note::GetIns()->Write(L"ADO : ", __FILEW__, __FUNCTIONW__, L"   Db Disconnect Fail");
	}
	return false;
}

bool gzz::Ado::IsConnect()
{
	return Connection->State != ADODB::adStateClosed ? true : false;
}

void gzz::Ado::Run()
{
}

void gzz::Ado::ErrorReport(const wchar_t* name, _com_error& e)
{
	Note::GetIns()->Write(L"ADO : ", __FUNCTIONW__, L"  ", name, L"   ", e.Description());
}

HRESULT gzz::Ado::SetConnectTimeOut(long second)
{
	if (!Connection)
	{
		return S_FALSE;
	}
	return Connection->put_ConnectionTimeout(second);
}

void gzz::Ado::Excute()
{
	if (Manager->QueryCount() <= 0) return;

	Query* query = nullptr;
	if (Manager->PopQuery(&query) == false) return;

	QueryCommand* command = query->GetCommand();
	const wchar_t* sql_query = command->GetQuery().data();
	try
	{
		State = RUNNING;
		QueryRecord record;

		ADODB::_CommandPtr cmd;
		cmd.CreateInstance(__uuidof(ADODB::Command));
		cmd->ActiveConnection = Connection;
		cmd->CommandType = ADODB::adCmdText;
		cmd->CommandText = sql_query;

		_variant_t result;
		switch (static_cast<QueryCommand::TYPE>(command->Type))
		{
		case QueryCommand::TYPE::NO_RETURN:
			record.Record = cmd->Execute(&result, NULL, ADODB::adCmdText | ADODB::adExecuteNoRecords);
			break;
		case QueryCommand::TYPE::RETURN:
			record.Record = cmd->Execute(&result, NULL, ADODB::adCmdText | ADODB::adExecuteRecord);
			break;
		case QueryCommand::TYPE::CALL_BACK:
			record.Record = cmd->Execute(&result, NULL, ADODB::adCmdText | ADODB::adAsyncFetchNonBlocking);
			break;
		}
		if (record.IsEof())
		{
			int query_result = atol((char*)((_bstr_t)result));
			if (query_result < 1)
			{
				Note::GetIns()->Write(L"ADO : ", __FUNCTIONW__, L"   Query : ", sql_query, L" Code : ", query_result);
			}
			else
			{
				record.SetResult(query_result);
			}
		}
		query->SetRecord(record);
		State = STANDBY;
	}
	catch (_com_error& e)
	{
		ErrorReport(L"sql_query", e);
	}
	if (query)
	{
		delete query;
	}
}

void gzz::Ado::Process()
{
	while (IsRun.load())
	{
		if (!IsConnect())
		{
			Note::GetIns()->Write(L"ADO : ", __FUNCTIONW__, L"   Connection Close..");
			IsRun.store(false);
		}
		Excute();
	}
}

void gzz::Ado::StartThread()
{
	IsRun.store(true);
	Woker = std::thread([&]() { Process(); });
}

void gzz::Ado::DestroyThread()
{
	IsRun.store(false);
	if (Woker.joinable() == true)
	{
		Woker.join();
	}
}