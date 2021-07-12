#include "QueryRecord.h"
#include "Note.h"

gzz::QueryRecord::QueryRecord()
	:Record(nullptr), QueryResult(0)
{
	Record.CreateInstance(__uuidof(ADODB::Recordset));
}

gzz::QueryRecord::~QueryRecord()
{
	if (Record == nullptr)
	{
		return;
	}
	Record.Release();
	Record = nullptr;
}

void gzz::QueryRecord::ErrorReport(_com_error& e)
{
	Note::GetIns()->Write(L"ADO : ", __FUNCTIONW__, L"  ", e.Description());
}

void gzz::QueryRecord::SetRecoard(RecPtr& ptr)
{
	Record = ptr;
}

bool gzz::QueryRecord::IsEof()
{
	if (Record == nullptr) return true;
	try
	{
		return Record->EndOfFile ? true : false;
	}
	catch (_com_error& e)
	{
		ErrorReport(e);
	}
	return false;
}

bool gzz::QueryRecord::Opened()
{
	return (Record->State == ADODB::adStateOpen) ? true : false;
}

void gzz::QueryRecord::Close()
{
	try
	{
		if (Record != nullptr && (Record->State == ADODB::adStateOpen) ? true : false)
		{
			Record->Close();
		}
	}
	catch (_com_error& e)
	{
		ErrorReport(e);
	}
}

int gzz::QueryRecord::Result()
{
	return QueryResult;
}

HRESULT gzz::QueryRecord::MoveNext()
{
	try
	{
		return Record->MoveNext();
	}
	catch (_com_error& e)
	{
		ErrorReport(e);
	}
	return S_FALSE;
}

HRESULT gzz::QueryRecord::MovePre()
{
	try
	{
		return Record->MovePrevious();
	}
	catch (_com_error& e)
	{
		ErrorReport(e);
	}
	return S_FALSE;
}

HRESULT gzz::QueryRecord::MoveFirst()
{
	try
	{
		return Record->MoveFirst();
	}
	catch (_com_error& e)
	{
		ErrorReport(e);
		Note::GetIns()->Write(L"ADO : ", __FILEW__, __FUNCTIONW__, L" Fail..");
	}
	return S_FALSE;
}

HRESULT gzz::QueryRecord::MoveLast()
{
	try
	{
		return Record->MoveLast();
	}
	catch (_com_error& e)
	{
		ErrorReport(e);
		Note::GetIns()->Write(L"ADO : ", __FILEW__, __FUNCTIONW__, L" Fail..");
	}
	return S_FALSE;
}

bool gzz::QueryRecord::Get(const char* fieldName, char* filedValue)
{
	try
	{
		_variant_t vtValue;
		vtValue = Record->Fields->GetItem(fieldName)->GetValue();
		sprintf_s(filedValue, DB_PARAM_SIZE, "%s", static_cast<LPCSTR>((_bstr_t)vtValue));
		return true;
	}
	catch (_com_error& e)
	{
		//ErrorReport(e);
		e;
		sprintf_s(filedValue, DB_PARAM_SIZE, "%s", "NULL");
		Note::GetIns()->Write(L"ADO : ", __FILEW__, __FUNCTIONW__, L" Character Fail..");
	}
	return false;
}

bool gzz::QueryRecord::Get(const char* fieldName, wchar_t* filedValue)
{
	try
	{
		_variant_t vtValue;
		vtValue = Record->Fields->GetItem(fieldName)->GetValue();
		swprintf_s(filedValue, DB_PARAM_SIZE, L"%s", static_cast<LPCWSTR>((_bstr_t)vtValue));
		return true;
	}
	catch (_com_error& e)
	{
		ErrorReport(e);
		Note::GetIns()->Write(L"ADO : ", __FILEW__, __FUNCTIONW__, L" Wide character Fail..");

	}
	return false;
}

bool gzz::QueryRecord::Get(const char* fieldName, int& filedValue)
{
	try
	{
		_variant_t vtValue;
		vtValue = Record->Fields->GetItem(fieldName)->GetValue();
		filedValue = vtValue.intVal;
		return true;
	}
	catch (_com_error& e)
	{
		ErrorReport(e);
		Note::GetIns()->Write(L"ADO : ", __FILEW__, __FUNCTIONW__, L" Integer Fail..");

	}
	return false;
}

bool gzz::QueryRecord::Get(const char* fieldName, float& filedValue)
{
	try
	{
		_variant_t vtValue;
		vtValue = Record->Fields->GetItem(fieldName)->GetValue();
		filedValue = vtValue.fltVal;
		return true;
	}
	catch (_com_error& e)
	{
		ErrorReport(e);
		Note::GetIns()->Write(L"ADO : ", __FILEW__, __FUNCTIONW__, L" Float Fail..");

	}
	return false;
}

bool gzz::QueryRecord::Get(const char* fieldName, double& filedValue)
{
	try
	{
		_variant_t vtValue;
		vtValue = Record->Fields->GetItem(fieldName)->GetValue();
		filedValue = vtValue.dblVal;
		return true;
	}
	catch (_com_error& e)
	{
		ErrorReport(e);
		Note::GetIns()->Write(L"ADO : ", __FILEW__, __FUNCTIONW__, L" Double Fail..");
	}
	return false;
}

bool gzz::QueryRecord::Get(const char* fieldName, long& filedValue)
{
	try
	{
		_variant_t vtValue;
		vtValue = Record->Fields->GetItem(fieldName)->GetValue();
		filedValue = vtValue.lVal;
		return true;
	}
	catch (_com_error& e)
	{
		ErrorReport(e);
		Note::GetIns()->Write(L"ADO : ", __FILEW__, __FUNCTIONW__, L" Long Fail..");
	}
	return false;
}
