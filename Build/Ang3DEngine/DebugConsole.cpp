// 필수 헤더
#include "GoodFunc.h"

// 참조 헤더
#include <stdio.h>
#include <time.h>

// 자기 헤더
#include "DebugConsole.h"

hos::cg::DebugConsole* hos::cg::DebugConsole::m_pInstance = nullptr;	    // 싱글턴 인스턴스 주소
FILE* hos::cg::DebugConsole::stream = nullptr;

hos::cg::DebugConsole::DebugConsole()
{
}

hos::cg::DebugConsole::~DebugConsole()
{
}

hos::cg::DebugConsole* hos::cg::DebugConsole::GetInstance()
{
    // 최초 호출 시 클래스 생성
    if (!m_pInstance)
    {
        m_pInstance = new hos::cg::DebugConsole();

		// 로케일 설정
		setlocale(LC_ALL, "");

		// 콘솔 생성 및 출력 스트림 설정
		AllocConsole();
		freopen_s(&stream, "CONOUT$", "wt", stdout);

		// 콘솔 설정 변경
		//HANDLE wHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		//int width = 80;
		//int height = 25;
		//int wbuffer = width + 200;
		//int hbuffer = height + 1000;
		//COORD coord = { wbuffer, hbuffer };
		//SMALL_RECT rect = { 0, 0, width -1, height -1 };
		//bool tab = SetConsoleScreenBufferSize(wHandle, coord);
		//bool tpo = SetConsoleWindowInfo(wHandle, TRUE, &rect);
		SetConsoleTitle(L"Ang3DEngine_DebugLog");

		m_pInstance->PrintDebug("DebugConsole" ,L"디버그콘솔 생성 완료.");
    }

    return m_pInstance;
}

void hos::cg::DebugConsole::Shutdown()
{
	if (m_pInstance)
	{
		m_pInstance->PrintDebug("DebugConsole", L"디버그콘솔 제거 완료(사실은 이 메시지 후에 종료됩니다.)");

		FreeConsole();
		fclose(stream);
		SAFE_DELETE(m_pInstance);
	}
}

void hos::cg::DebugConsole::PrintDebug(const char* className, std::wstring message)
{
	PrintTime(className);

	wprintf(L"%s\n", message.c_str());

	return;
}

void hos::cg::DebugConsole::PrintDebug(const char* className, std::string message)
{
	PrintTime(className);

	printf("%s\n", message.c_str());

	return;
}

void hos::cg::DebugConsole::PrintDebug_TargetInfo(const char* className, std::string targetName, std::string message)
{
	PrintTime(className);

	printf("%s : %s\n", targetName.c_str(), message.c_str());
}

void hos::cg::DebugConsole::PrintDebug_TargetInfo(const char* className, std::string targetName, std::wstring message)
{
	PrintTime(className);

	std::wstring _targetName(targetName.begin(), targetName.end());

	wprintf(L"%s : %s\n", _targetName.c_str(), message.c_str());
}

void hos::cg::DebugConsole::PrintDebug_TargetInfo(const char* className, std::wstring targetName, std::wstring message)
{
	PrintTime(className);

	wprintf(L"%s : %s\n", targetName.c_str(), message.c_str());

	return;
}

void hos::cg::DebugConsole::PrintDebug_TargetInfo(const char* className, std::wstring targetName, std::string message)
{
	PrintTime(className);

	std::wstring _message(message.begin(), message.end());

	wprintf(L"%s : %s\n", targetName.c_str(), _message.c_str());

	return;
}

void hos::cg::DebugConsole::PrintTime(const char* className)
{
	struct tm newtime;
	__time64_t long_time;
	char timebuf[26];
	errno_t err;

	// Get time as 64-bit integer.
	_time64(&long_time);
	// Convert to local time.
	err = _localtime64_s(&newtime, &long_time);
	if (err)
		printf("Invalid argument to _localtime64_s.");

	// Convert to an ASCII representation.
	err = asctime_s(timebuf, 26, &newtime);
	if (err)
		printf("Invalid argument to asctime_s.");

	//printf("%.19s\n", timebuf);

	printf("[%04d.%02d.%02d %02d:%02d:%02d] [%-50s] ",
		newtime.tm_year + 1900, newtime.tm_mon + 1, newtime.tm_mday,
		newtime.tm_hour, newtime.tm_min, newtime.tm_sec, className);
}

void hos::cg::DebugConsole::WcharToChar(const WCHAR* wcharStr, CHAR* dest)
{
	int strSize = WideCharToMultiByte(CP_ACP, 0, wcharStr, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, wcharStr, -1, dest, strSize, 0, 0);

	return;
}

void hos::cg::DebugConsole::CharToWChar(const CHAR* charStr, WCHAR* dest)
{
	int strSize = MultiByteToWideChar(CP_ACP, 0, charStr, -1, NULL, NULL);
	MultiByteToWideChar(CP_ACP, 0, charStr, (int)strlen(charStr) + 1, dest, strSize);

	return;
}
