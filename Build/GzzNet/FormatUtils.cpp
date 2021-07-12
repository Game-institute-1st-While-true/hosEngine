#include "FormatUtils.h"
#include "assert.h"
#include "atlconv.h"

string gzz::FormatUtils::vformat(LPCSTR fmt, va_list vl)
{
	CHAR buf[MAX_GSTRINGBUF + 1];
	int nBuf = _vsnprintf_s(buf, MAX_GSTRINGBUF, fmt, vl);

	assert(nBuf >= 0);

	if (nBuf < 0)
		buf[MAX_GSTRINGBUF] = 0;
	return string(buf);
}


wstring gzz::FormatUtils::vformat(LPCWSTR fmt, va_list vl)
{
	WCHAR buf[MAX_GSTRINGBUF + 1];
	int nBuf = _vsnwprintf_s(buf, MAX_GSTRINGBUF, fmt, vl);

	assert(nBuf >= 0);

	if (nBuf < 0)
		buf[MAX_GSTRINGBUF] = 0;
	return wstring(buf);
}


string gzz::FormatUtils::format(LPCSTR fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);
	string ret = vformat(fmt, vl);
	va_end(vl);
	return ret;
}

wstring gzz::FormatUtils::format(LPCWSTR fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);
	wstring ret = vformat(fmt, vl);
	va_end(vl);
	return ret;
}


int gzz::FormatUtils::AnsiToUTF16(const string& strAnsi, wstring& strUnicode)
{
	int len = ::MultiByteToWideChar(CP_ACP, 0, strAnsi.c_str(), -1, NULL, 0);
	if (len == 0)	return len;
	wchar_t* wbuf = new wchar_t[len];
	len = ::MultiByteToWideChar(CP_ACP, 0, strAnsi.c_str(), -1, wbuf, len);
	if (len != 0)	strUnicode = wbuf;
	delete[] wbuf;
	return len;
}

int gzz::FormatUtils::UTF16ToUTF8(const wstring& strUnicode, string& strUtf8)
{
	int len = ::WideCharToMultiByte(CP_UTF8, 0, strUnicode.c_str(), -1, NULL, 0, NULL, NULL);
	if (len == 0)	return len;
	char* buf = new char[len];
	len = ::WideCharToMultiByte(CP_UTF8, 0, strUnicode.c_str(), -1, buf, len, NULL, NULL);
	if (len != 0)	strUtf8 = buf;
	delete[] buf;
	return len;
}

int gzz::FormatUtils::AnsiToUTF8(const string& strAnsi, string& strUtf8)
{
	wstring strUnicode;
	if (AnsiToUTF16(strAnsi, strUnicode) != 0)
		return UTF16ToUTF8(strUnicode, strUtf8);
	return 0;
}

int gzz::FormatUtils::UTF8ToUTF16(const string& strUtf8, wstring& strUnicode)
{
	int len = ::MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, NULL, 0);
	if (len == 0)	return len;
	wchar_t* wbuf = new wchar_t[len];
	len = ::MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, wbuf, len);
	if (len != 0)	strUnicode = wbuf;
	delete[] wbuf;
	return len;
}

int gzz::FormatUtils::UTF16ToAnsi(const wstring& strUnicode, string& strAnsi)
{
	int len = ::WideCharToMultiByte(CP_ACP, 0, strUnicode.c_str(), -1, NULL, 0, NULL, NULL);
	if (len == 0)	return len;
	char* buf = new char[len];
	len = ::WideCharToMultiByte(CP_ACP, 0, strUnicode.c_str(), -1, buf, len, NULL, NULL);
	if (len != 0)	strAnsi = buf;
	delete[] buf;
	return len;
}

int gzz::FormatUtils::UTF8ToAnsi(const string& strUtf8, string& strAnsi)
{
	wstring strUnicode;
	if (UTF8ToUTF16(strUtf8, strUnicode) != 0)
		return UTF16ToAnsi(strUnicode, strAnsi);
	return 0;
}

///////////////////////////////////////////////////////////
wstring gzz::FormatUtils::AnsiToUTF16(const string& strAnsi)
{
	wstring strUnicode = L"";

	AnsiToUTF16(strAnsi, strUnicode);

	return strUnicode;
}

wstring gzz::FormatUtils::UTF8ToUTF16(const string& strUtf8)
{
	wstring strUnicode = L"";

	UTF8ToUTF16(strUtf8, strUnicode);

	return strUnicode;
}

string gzz::FormatUtils::UTF16ToUTF8(const wstring& strUnicode)
{
	string strUtf8 = "";

	UTF16ToUTF8(strUnicode, strUtf8);

	return strUtf8;
}

string gzz::FormatUtils::AnsiToUTF8(const string& strAnsi)
{
	string strUtf8 = "";

	AnsiToUTF8(strAnsi, strUtf8);

	return strUtf8;
}

string gzz::FormatUtils::UTF16ToAnsi(const wstring& strUnicode)
{
	string strAnsi = "";

	UTF16ToAnsi(strUnicode, strAnsi);

	return strAnsi;
}

string gzz::FormatUtils::UTF8ToAnsi(const string& strUtf8)
{
	string strAnsi = "";

	UTF8ToAnsi(strUtf8, strAnsi);

	return strAnsi;
}


std::string gzz::FormatUtils::toUtf8(const std::wstring& str)
{
	std::string ret;
	int len = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), NULL, 0, NULL, NULL);
	if (len > 0)
	{
		ret.resize(static_cast<size_t>(len));
		WideCharToMultiByte(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), &ret[0], len, NULL, NULL);
	}
	return ret;
}

string gzz::FormatUtils::replaceAll(const string &str, const string &pattern, const string &replace)
{
	string result = str;
	string::size_type pos = 0;
	string::size_type offset = 0;

	while ((pos = result.find(pattern, offset)) != string::npos)
	{
		result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
		offset = pos + replace.size();
	}

	return result;
}

vector<string> gzz::FormatUtils::split(string strSource, string strToken)
{
	size_t cutAt;
	//	int index = 0;

	vector<string> vecResult;

	while ((cutAt = strSource.find_first_of(strToken)) != strSource.npos)
	{
		if (cutAt > 0)
		{
			vecResult.push_back(strSource.substr(0, cutAt));
		}
		strSource = strSource.substr(cutAt + 1);
	}

	if (strSource.length() > 0)
	{
		vecResult.push_back(strSource.substr(0, cutAt));
	}

	return vecResult;
}

BOOL gzz::FormatUtils::IsNumeric(const string& s)
{
	BOOL decimal = FALSE;
	for (unsigned int i = 0; i < s.length(); ++i)
	{
		if (s[i] == '.' && !decimal)
			decimal = TRUE;
		else if (s[i] < '0' || s[i] > '9')
			return FALSE;
	}
	return TRUE;
}


