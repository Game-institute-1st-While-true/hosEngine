#include "Dump.h"
#include <tchar.h>
LPCTSTR Dump::AppName;

using MINIDUMPWRITEDUMP = BOOL(WINAPI*)(HANDLE hProcess,
	DWORD ProcessId,
	HANDLE hFile,
	MINIDUMP_TYPE DumpType,
	PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

LONG __stdcall Dump::TopLevelFilter(_EXCEPTION_POINTERS* pException)
{
	LONG retval = EXCEPTION_CONTINUE_SEARCH;
	HWND hParent = NULL;      // find a better value for your app

	// firstly see if dbghelp.dll is around and has the function we need
	// look next to the EXE first, as the one in System32 might be old
	// (e.g. Windows 2000)
	HMODULE hDll = NULL;
	WCHAR szDbgHelpPath[MAX_PATH];

	if (GetModuleFileName(NULL, szDbgHelpPath, MAX_PATH))
	{
		WCHAR* pSlash = _tcsrchr(szDbgHelpPath, '\\');
		if (pSlash)
		{
			_tcscpy(pSlash + 1, _T("DBGHELP.DLL"));
			hDll = ::LoadLibrary(szDbgHelpPath);
		}
	}

	if (hDll == NULL)
	{
		// load any version we can
		hDll = ::LoadLibrary(_T("DBGHELP.DLL"));
	}

	LPCTSTR szResult = NULL;

	if (hDll)
	{
		MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress(hDll, "MiniDumpWriteDump");
		if (pDump)
		{
			WCHAR szDumpPath[MAX_PATH];
			WCHAR szScratch[MAX_PATH];

			// work out a good place for the dump file
			if (!GetTempPath(MAX_PATH, szDumpPath))
				_tcscpy(szDumpPath, _T("c:\\temp\\"));

			_tcscat(szDumpPath, AppName);
			_tcscat(szDumpPath, _T(".dmp"));

			// ask the user if they want to save a dump file
			if (::MessageBox(NULL, _T("Something bad happened in your program, would you like to save a diagnostic file?"), AppName, MB_YESNO) == IDYES)
			{
				// create the file
				HANDLE hFile = ::CreateFile(szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
					FILE_ATTRIBUTE_NORMAL, NULL);

				if (hFile != INVALID_HANDLE_VALUE)
				{
					_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

					ExInfo.ThreadId = ::GetCurrentThreadId();
					ExInfo.ExceptionPointers = pException;
					ExInfo.ClientPointers = NULL;

					int type;
					// write the dump
				#if defined(_DEBUG)||defined(DEBUG)
					type = MiniDumpWithFullMemory | MiniDumpWithFullMemoryInfo | MiniDumpWithHandleData | MiniDumpWithThreadInfo | MiniDumpWithUnloadedModules;
				#else
					type = MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory;
				#endif
					BOOL bOK = pDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, (MINIDUMP_TYPE)type, &ExInfo, NULL, NULL);
					if (bOK)
					{
						swprintf(szScratch, _T("Saved dump file to '%s'"), szDumpPath);
						szResult = szScratch;
						retval = EXCEPTION_EXECUTE_HANDLER;
					}
					else
					{
						swprintf(szScratch, _T("Failed to save dump file to '%s' (error %d)"), szDumpPath, GetLastError());
						szResult = szScratch;
					}
					::CloseHandle(hFile);
				}
				else
				{
					swprintf(szScratch, _T("Failed to create dump file '%s' (error %d)"), szDumpPath, GetLastError());
					szResult = szScratch;
				}
			}
		}
		else
		{
			szResult = _T("DBGHELP.DLL too old");
		}
	}
	else
	{
		szResult = _T("DBGHELP.DLL not found");
	}

	if (szResult)
		::MessageBox(NULL, szResult, AppName, MB_OK);

	return retval;
}

Dump::Dump(LPCTSTR name)
{
	AppName = name ? name : _T("APP");

	::SetUnhandledExceptionFilter(TopLevelFilter);
}
