#include "WindowsApplication.h"
#include "WindowsInput.h"

using namespace hos;

hos::WindowsApplication::WindowsApplication(string_view Name) :
	H_Ins(NULL), H_Wnd(NULL),
	ClassName(Name)
{

}

hos::WindowsApplication::~WindowsApplication()
{

}

bool hos::WindowsApplication::Create(HINSTANCE hInstance, int width, int height, int x, int y, DWORD style, HICON icon, HCURSOR cursor)
{
	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = Proc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = icon;
	wndclass.hCursor = cursor;
	wndclass.hbrBackground = NULL;
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = ClassName.c_str();

	RegisterClass(&wndclass);

	WS_Style = style;
	WorkSpaceSize = { x,y,width,height };
	ClientSize = WorkSpaceSize;
	AdjustWindowRect(&ClientSize, WS_Style, false);

	H_Wnd = CreateWindow(ClassName.c_str(), ClassName.c_str(), WS_Style,
		CW_USEDEFAULT, CW_USEDEFAULT, ClientSize.right - ClientSize.left, ClientSize.bottom - ClientSize.top, NULL, NULL, hInstance, NULL);

	if (!H_Wnd)
	{
		return false;
	}
	H_Ins = hInstance;

	ShowWindow(H_Wnd, SW_SHOW);
	UpdateWindow(H_Wnd);

	return true;
}

void hos::WindowsApplication::Destory()
{
	SendMessage(H_Wnd, WM_DESTROY, NULL, NULL);
}

HRESULT hos::WindowsApplication::Run()
{
	MSG msg;

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}

			DispatchMessage(&msg);
		}
		else
		{
		}
	}
	return (int)msg.wParam;
}

LRESULT hos::WindowsApplication::Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool Lock = false;
	switch (message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		break;
	case WM_ACTIVATEAPP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		[[fallthrough]];
	case WM_MOUSEMOVE:
	case WM_INPUT:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_SETCURSOR:
		if (Cursor != NULL)
		{
			SetCursor(Cursor);
		}
		return true;
		break;
	case WM_USER + 1:
		Cursor = (HCURSOR)wParam;
		SetCursor(Cursor);
		break;
	case WM_USER + 2:
	{
		int visiable = ShowCursor((BOOL)wParam);
	}
		break;
	case WM_ACTIVATE:
	{
		unsigned short state = LOWORD(wParam);
		switch (state)
		{
		case WA_ACTIVE:
			[[fallthrough]];
		case WA_CLICKACTIVE:
			if (Lock)
			{
				Input->SetMouseMode(hos::WindowsInput::eMouseMode::LOCKED);
			}
			break;
		case WA_INACTIVE:
			if (Input->GetMouseMode() == hos::WindowsInput::eMouseMode::LOCKED)
			{
				Lock = true;
				Input->SetMouseMode(hos::WindowsInput::eMouseMode::NONE);
			}
			else
			{
				Lock = false;
			}
			break;
		default:
			break;
		}
	}
		break;
	case WM_SIZE:
	{
		WorkSpaceSize.left = 0;
		WorkSpaceSize.right = LOWORD(lParam);
		WorkSpaceSize.top = 0;
		WorkSpaceSize.bottom = HIWORD(lParam);
		ClientSize = WorkSpaceSize;
		bool bMenu = WS_Style & WS_SYSMENU;
		AdjustWindowRect(&ClientSize, WS_Style, bMenu);
	}
		break;
	case WM_CLOSE:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void hos::WindowsApplication::SetName(string name)
{
	ClassName = name;
	SetWindowText(H_Wnd, ClassName.c_str());
}

void hos::WindowsApplication::SetSize(RECT rect)
{
	WorkSpaceSize = rect;
	ClientSize = rect;
	bool bMenu = WS_Style & WS_SYSMENU;
	AdjustWindowRect(&ClientSize, WS_Style, bMenu);
	SetWindowPos(H_Wnd, HWND_TOP, 0, 0, ClientSize.right - ClientSize.left, ClientSize.bottom - ClientSize.top, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
}

HWND hos::WindowsApplication::GetH_Wnd() const
{
	return H_Wnd;
}

HINSTANCE hos::WindowsApplication::GetH_Ins() const
{
	return H_Ins;
}

string hos::WindowsApplication::GetName() const
{
	return ClassName;
}

RECT hos::WindowsApplication::GetClientSize() const
{
	return ClientSize;
}

RECT hos::WindowsApplication::GetSize() const
{
	return WorkSpaceSize;
}