#pragma once														
#include <Windows.h>
#include <string>
#include <vector>
using namespace std;

namespace gzz
{
	class FormatUtils
	{
	public:

		//const static UINT MAX_GSTRINGBUF = 512;
		const static UINT MAX_GSTRINGBUF = 1024;

		static string vformat(LPCSTR fmt, va_list vl);
		static wstring vformat(LPCWSTR fmt, va_list vl);
		static string format(LPCSTR fmt, ...);
		static wstring format(LPCWSTR fmt, ...);

		static int AnsiToUTF16(const string& strAnsi, wstring& strUnicode);
		static int UTF8ToUTF16(const string& strUtf8, wstring& strUnicode);

		static int UTF16ToUTF8(const wstring& strUnicode, string& strUtf8);
		static int AnsiToUTF8(const string& strAnsi, string& strUtf8);

		static int UTF16ToAnsi(const wstring& strUnicode, string& strAnsi);
		static int UTF8ToAnsi(const string& strUtf8, string& strAnsi);

		//
		//
		static wstring AnsiToUTF16(const string& strAnsi);
		static wstring UTF8ToUTF16(const string& strUtf8);

		static string UTF16ToUTF8(const wstring& strUnicode);
		static string AnsiToUTF8(const string& strAnsi);

		static string UTF16ToAnsi(const wstring& strUnicode);
		static string UTF8ToAnsi(const string& strUtf8);

		static std::string toUtf8(const std::wstring& str);

		static string replaceAll(const string& str, const string& pattern, const string& replace);
		static vector<string> split(string strSource, string strToken);
		static BOOL IsNumeric(const string& s);
	};
}