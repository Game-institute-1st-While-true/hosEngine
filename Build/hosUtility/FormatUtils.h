#pragma once														
#include "BaseTypes.h"
#include "InternalWindow.h"
#include <strsafe.h>
#include <string>
#include <vector>
#include <algorithm>
#include <locale>
#include <functional>

// LOG
// [2020/12/31] ������ : WIN32_LEAN_AND_MEAN���� �߰� hos, ut���ӽ����̽� �߰�
// [2021/04/20] ������ : Deprecated �Լ� ���� string_view���� ����

namespace hos::ut
{
	static constexpr UINT MAX_GSTRINGBUF = 1024;

	mbstring format(LPCSTR fmt, ...);
	string format(LPCWSTR fmt, ...);

	string AnsiToUTF16(const mbstring_view& strAnsi);
	string UTF8ToUTF16(const mbstring_view& strUtf8);

	mbstring UTF16ToUTF8(const string_view& strUnicode);
	mbstring AnsiToUTF8(const mbstring_view& strAnsi);

	mbstring UTF16ToAnsi(const string_view& strUnicode);
	mbstring UTF8ToAnsi(const mbstring_view& strUtf8);

	mbstring replaceAll(const mbstring& str, const mbstring& pattern, const mbstring& replace);
	string replaceAll(const string& str, const string& pattern, const string& replace);
	
	string ToLower(string str);
}