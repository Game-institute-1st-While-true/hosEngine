// <Concept>
// Source Name		: DebugConsole.h
// Creator			: Hosik_Shin / shin0394g@gmail.com
// Create Date		: 2021.02.04

// <Explanation>
// �׷��� �������� �ʱ�ȭ�� Ȯ���ϱ� ���� Console Debuger Ŭ����.
// �̱������� �۵��մϴ�.

// <Working Log>
// 2021.02.04.Hosik	: ���� AngD3DEnine_Mk1���� Import.
// 2021.03.25.Hosik : message �Ű������� ���� ���̽��� Ȯ��, �Ķ���͸� ��� std�� string, wstring���� ��ü.

#pragma once

#include "framework.h"
#include <string>
#include <iostream>

namespace hos::cg
{
	class DebugConsole
	{
	private:
		DebugConsole();		// ������
		~DebugConsole();	// �Ҹ���

	private:
		static DebugConsole* m_pInstance;	// �̱��� �ν��Ͻ� �ּ�
		static FILE* stream;				// ��½�Ʈ��

	public:
		static DebugConsole* GetInstance();

		void Shutdown();

		void PrintDebug(const char* className, std::wstring message);
		void PrintDebug(const char* className, std::string message);

		void PrintDebug_TargetInfo(const char* className, std::string targetName, std::string message);
		void PrintDebug_TargetInfo(const char* className, std::string targetName, std::wstring message);

		void PrintDebug_TargetInfo(const char* className, std::wstring targetName, std::wstring message);
		void PrintDebug_TargetInfo(const char* className, std::wstring targetName, std::string message);

	private:
		void PrintTime(const char* className);

		void WcharToChar(const WCHAR* wcharStr, CHAR* dest);
		void CharToWChar(const CHAR* charStr, WCHAR* dest);
	};
}

// ����� �ܼ�
#ifdef _DEBUG
#define DEBUG_CONSOLE(message) DebugConsole::GetInstance()->PrintDebug(__FUNCTION__, message)
#define DEBUG_CONSOLET(targetName, message) DebugConsole::GetInstance()->PrintDebug_TargetInfo(__FUNCTION__, targetName, message)
#else
#define DEBUG_CONSOLE(message)
#define DEBUG_CONSOLET(targetName, message)
#endif