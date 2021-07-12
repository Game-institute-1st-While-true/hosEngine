#pragma once
#include "DBDefine.h"

//[2021-03-24 ��ȿ��]
//������ �����ͺ��̽��� ���ϰ��� �����ϴ� Ŭ����

namespace gzz
{
	class QueryRecord
	{
		friend class Ado;
		friend class Query;

	private:
		RecPtr Record;
		int	QueryResult;

	public:
		QueryRecord();
		~QueryRecord();

	public:
		void ErrorReport(_com_error& e);

		void SetRecoard(RecPtr& ptr);
		RecPtr GetPtr() { return Record; }

		bool IsEof();
		bool Opened();
		void Close();

		int Result();
		void SetResult(int result) { QueryResult = result; }

		HRESULT MoveNext();
		HRESULT MovePre();
		HRESULT MoveFirst();
		HRESULT MoveLast();

		bool Get(const char* fieldName, char* filedValue);
		bool Get(const char* fieldName, wchar_t* filedValue);
		bool Get(const char* fieldName, int& filedValue);
		bool Get(const char* fieldName, float& filedValue);
		bool Get(const char* fieldName, double& filedValue);
		bool Get(const char* fieldName, long& filedValue);
	};
}
