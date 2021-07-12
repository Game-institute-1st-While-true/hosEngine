#include "Console.h"

hos::ut::DebugConsole::DebugConsole()
{
#if defined(DEBUG) || defined(_DEBUG)

	if (AllocConsole())		//if Fail is FALSE(0)
	{
		WriteHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		Hwnd = GetConsoleWindow();

		COORD _size;
		_size.X = static_cast<SHORT>(DEFAULT_WIDTH);
		_size.Y = static_cast<SHORT>(DEFAULT_HEIGHT);
		SetConsoleScreenBufferSize(WriteHandle, _size);

		SetConsoleTitleW(L"DEBUG");
	}

	ShowWindow(Hwnd, SW_SHOW);
	SetForegroundWindow(Hwnd);
#endif // DEBUG
}

hos::ut::DebugConsole::~DebugConsole()
{
	if (WriteHandle)
	{
		fclose(stdout);
		WriteHandle = NULL;
		FreeConsole();
	}
}

hos::ut::DebugConsole& hos::ut::DebugConsole::operator<<(string_view string)
{
	if (WriteHandle)
	{
		DWORD nb;
		WriteConsoleW(WriteHandle, string.data(), (DWORD)string.length(), &nb, NULL);
	}

	return *this;
}

hos::ut::DebugConsole& hos::ut::DebugConsole::operator<<(mbstring_view string)
{
	if (WriteHandle)
	{
		DWORD nb;
		WriteConsoleA(WriteHandle, string.data(), (DWORD)string.length(), &nb, NULL);
	}

	return *this;
}

void hos::ut::DebugConsole::Reset()
{
	WriteHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	Hwnd = GetConsoleWindow();

	COORD _size;
	_size.X = static_cast<SHORT>(DEFAULT_WIDTH);
	_size.Y = static_cast<SHORT>(DEFAULT_HEIGHT);
	SetConsoleScreenBufferSize(WriteHandle, _size);

	SetConsoleTitleW(L"DEBUG");
}

void hos::ut::DebugConsole::Show()
{
	if (Hwnd)
	{
		ShowWindow(Hwnd, SW_SHOW);
		SetForegroundWindow(Hwnd);
	}
}

void hos::ut::DebugConsole::Hide()
{
	if (Hwnd)
	{
		ShowWindow(Hwnd, SW_HIDE);
	}
}

hos::ut::SubConsole::SubConsole() : DebugConsole()
{
#if defined(DEBUG) || defined(_DEBUG)

	ReadHandle = GetStdHandle(STD_INPUT_HANDLE);
#endif // DEBUG
}

hos::ut::SubConsole::~SubConsole()
{
	if (ReadHandle)
	{
		fclose(stdin);
		ReadHandle = NULL;
	}
}

hos::ut::SubConsole& hos::ut::SubConsole::operator>>(string_view& string)
{
	if (ReadHandle)
	{
		DWORD nb;
		wchar_t _string[DEFAULT_READ_SIZE + 1]{};
		ReadConsoleW(ReadHandle, _string, DEFAULT_READ_SIZE, &nb, NULL);
		string = _string;
	}

	return *this;
}

hos::ut::SubConsole& hos::ut::SubConsole::operator>>(string& string)
{
	if (ReadHandle)
	{
		DWORD nb;
		wchar_t _string[DEFAULT_READ_SIZE + 1]{};
		ReadConsoleW(ReadHandle, _string, DEFAULT_READ_SIZE, &nb, NULL);
		string = _string;
	}

	return *this;
}