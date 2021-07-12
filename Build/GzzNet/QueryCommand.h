#pragma once
#include <string>
#include <sstream>
#include "DBDefine.h"

// [2021-03-24 ��ȿ��]
// ������ �����ͺ��̽��� �����ϴ� Ŭ����

static constexpr int MAX_PARAM = 100;
namespace gzz
{
	class QueryCommand
	{
		friend class Query;
		friend class Ado;
		friend class DBManager;

	public:
		enum class TYPE : unsigned char
		{
			RETURN,		// ���� �ִ� ���� �� �� ���� ����� (adAsyncFetch : ���� ���� ������ �� ���� ��� ���Ŀ� �񵿱�)
			CALL_BACK,	// �ݹ� ���� (������ ������ ����, ��� x) (adAsyncFetchNonBlocking)
			NO_RETURN,	// ���� ���� ���� (adExecuteNoRecords)
		};

	private:
		CmdPtr Cmd;
		std::wstring QueryText;
		int Type;
		int ParamCount;

	public:
		QueryCommand();
		~QueryCommand();

	public:
		void SetQuery(std::wstring query, int type);									// ���� �������� �����մϴ�.
		std::wstring_view GetQuery() { return QueryText; }								// �������� Ȯ�� �մϴ�.

		template<typename ... Args>
		void Call_Procedure(TYPE type, std::wstring name, Args... message);				// ���� ���ν����� ���� �������� �����մϴ�.

	private:
		template<typename T>
		void AddParam(T word, int& count);
	};

	template<typename ...Args>
	inline void QueryCommand::Call_Procedure(TYPE type, std::wstring name, Args... message)
	{
		Type = static_cast<int>(type);
		QueryText.clear();

		QueryText += L"CALL ";
		QueryText += name;
		QueryText += L"(";

		(AddParam(message, ParamCount), ...);
		if (ParamCount != 0)
		{
			QueryText.pop_back();
			QueryText.pop_back();
		}
		QueryText += L")";
	}

	template<typename T>
	inline void QueryCommand::AddParam(T param, int& count)
	{
		std::wstringstream text;
		text << param;
		text << ", ";

		QueryText += text.str();
		count++;
	}
}
