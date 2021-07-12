#include "FormatUtils.h"
#include "assert.h"

using namespace hos;

mbstring hos::ut::format(LPCSTR fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);
	mbstring ret;
	ret.reserve(MAX_GSTRINGBUF);
	StringCbVPrintfA(ret.data(), MAX_GSTRINGBUF, fmt, vl);
	va_end(vl);

	ret.shrink_to_fit();
	return ret;
}

string hos::ut::format(LPCWSTR fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);
	string ret;
	ret.reserve(MAX_GSTRINGBUF);
	StringCbVPrintfW(ret.data(), MAX_GSTRINGBUF, fmt, vl);
	va_end(vl);

	ret.shrink_to_fit();
	return ret;
}

string hos::ut::AnsiToUTF16(const mbstring_view& strAnsi)
{
	string strUnicode;

	int len = ::MultiByteToWideChar(CP_ACP, 0, strAnsi.data(), -1, NULL, 0);
	if (len != 0)
	{
		wchar_t* copied = new wchar_t[len];
		len = ::MultiByteToWideChar(CP_ACP, 0, strAnsi.data(), -1, copied, len);
		strUnicode = copied;

		delete[] copied;
		return strUnicode;
	}
	return string();
}

string hos::ut::UTF8ToUTF16(const mbstring_view& strUtf8)
{
	string strUnicode;

	int len = ::MultiByteToWideChar(CP_UTF8, 0, strUtf8.data(), -1, NULL, 0);
	if (len != 0)
	{
		wchar_t* copied = new wchar_t[len];
		len = ::MultiByteToWideChar(CP_UTF8, 0, strUtf8.data(), -1, copied, len);
		strUnicode = copied;

		delete[] copied;
		return strUnicode;
	}
	return string();
}

mbstring hos::ut::UTF16ToUTF8(const string_view& strUnicode)
{
	mbstring strUtf8;

	int len = ::WideCharToMultiByte(CP_UTF8, 0, strUnicode.data(), -1, NULL, 0, NULL, NULL);
	if (len != 0)
	{
		char* copied = new char[len];
		len = ::WideCharToMultiByte(CP_UTF8, 0, strUnicode.data(), -1, copied, len, NULL, NULL);
		strUtf8 = copied;

		delete[] copied;
		return strUtf8;
	}

	return mbstring();
}

mbstring hos::ut::AnsiToUTF8(const mbstring_view& strAnsi)
{
	string strUnicode = AnsiToUTF16(strAnsi);
	if (strUnicode.size() != 0)
	{
		return UTF16ToUTF8(strUnicode);
	}

	return mbstring();
}

mbstring hos::ut::UTF16ToAnsi(const string_view& strUnicode)
{
	mbstring strAnsi;

	int len = ::WideCharToMultiByte(CP_ACP, 0, strUnicode.data(), -1, NULL, 0, NULL, NULL);
	if (len != 0)
	{
		char* copied = new char[len];
		len = ::WideCharToMultiByte(CP_ACP, 0, strUnicode.data(), -1, copied, len, NULL, NULL);
		strAnsi = copied;

		delete[] copied;
		return strAnsi;
	}

	return mbstring();
}

mbstring hos::ut::UTF8ToAnsi(const mbstring_view& strUtf8)
{
	mbstring strAnsi;

	string strUnicode = UTF8ToUTF16(strUtf8);
	if (strUnicode.size() != 0)
		return strAnsi = UTF16ToAnsi(strUnicode);

	return mbstring();
}

mbstring hos::ut::replaceAll(const mbstring& str, const mbstring& pattern, const mbstring& replace)
{
	mbstring result = str;
	mbstring::size_type pos = 0;
	mbstring::size_type offset = 0;

	while ((pos = result.find(pattern, offset)) != mbstring::npos)
	{
		result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
		offset = pos + replace.size();
	}

	return result;
}

string hos::ut::replaceAll(const string& str, const string& pattern, const string& replace)
{
	string result = str;
	string::size_type pos = 0;
	string::size_type offset = 0;

	while ((pos = result.find(pattern, offset)) != mbstring::npos)
	{
		result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
		offset = pos + replace.size();
	}

	return result;
}

string hos::ut::ToLower(string str)
{
	//현제 시스템 로케일
	std::locale loc(std::locale::classic(), ".OCP", std::locale::ctype | std::locale::collate);
	//소문자 변환 함수
	auto func = std::bind(std::tolower<wchar_t>, std::placeholders::_1, loc);
	//소문자로 변환
	std::transform(str.begin(), str.end(), str.begin(), func);
	return str;
}