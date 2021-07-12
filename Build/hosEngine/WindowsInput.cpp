#include "WindowsInput.h"

using namespace hos;
using namespace DirectX;

hos::WindowsInput* Input = nullptr;

hos::WindowsInput::WindowsInput(HWND hWnd)
	: H_Wnd(hWnd)
{
	gpDevice = new GamePad();
	kbDevice = new Keyboard();
	mDevice = new Mouse();

	gpTracker = new GamePad::ButtonStateTracker();
	kbTracker = new Keyboard::KeyboardStateTracker();
	mTracker = new Mouse::ButtonStateTracker();

	mDevice->SetWindow(H_Wnd);
}

hos::WindowsInput::~WindowsInput()
{
	SafeDelete(gpTracker);
	SafeDelete(kbTracker);
	SafeDelete(mTracker);

	SafeDelete(gpDevice);
	SafeDelete(kbDevice);
	SafeDelete(mDevice);
}

void hos::WindowsInput::Update()
{
	if (MouseMode != eMouseMode::NONE)
	{
		GetClientRect(H_Wnd, &rect);

		POINT ul;
		ul.x = rect.left;
		ul.y = rect.top;

		POINT lr;
		lr.x = rect.right;
		lr.y = rect.bottom;

		MapWindowPoints(H_Wnd, nullptr, &ul, 1);
		MapWindowPoints(H_Wnd, nullptr, &lr, 1);

		rect.left = ul.x;
		rect.top = ul.y;

		rect.right = lr.x;
		rect.bottom = lr.y;

		switch (MouseMode)
		{
		case hos::WindowsInput::eMouseMode::LOCKED:
			SetCursorPos((rect.right - rect.left) / 2 + rect.left, (rect.bottom - rect.top) / 2 + rect.top);
			break;
		case hos::WindowsInput::eMouseMode::CONFINED:
			ClipCursor(&rect);
			break;
		}
	}

	DirectX::GamePad::State gpState = gpDevice->GetState(0);
	if (gpState.IsConnected())
	{
		gpTracker->Update(gpState);
	}
	else
	{
		gpTracker->Reset();
	}

	if (kbDevice->IsConnected())
	{
		kbTracker->Update(kbDevice->GetState());
	}
	else
	{
		kbTracker->Reset();
	}

	if (mDevice->IsConnected())
	{
		mTracker->Update(mDevice->GetState());
	}
	else
	{
		mTracker->Reset();
	}
}

bool hos::WindowsInput::AnyKey()
{
	if (kbDevice->IsConnected())
	{
		Keyboard::State t = kbDevice->GetState();
		U32* ks = reinterpret_cast<U32*>(&t);
		for (int i = 0; i < 8; i++)
		{
			if (*(ks + i) ^ 0x0)
			{
				return true;
			}
		}
	}
	
	if (mDevice->IsConnected())
	{
		Mouse::State mt = mDevice->GetState();
		if (mt.leftButton || mt.middleButton || mt.rightButton || mt.xButton1 || mt.xButton2)
		{
			return true;
		}
	}
	
	DirectX::GamePad::State gpState = gpDevice->GetState(0);
	if (gpState.IsConnected())
	{
		if (gpState.buttons.a || gpState.buttons.b || gpState.buttons.x || gpState.buttons.y 
			|| gpState.buttons.back || gpState.buttons.view || gpState.buttons.leftShoulder || gpState.buttons.rightShoulder 
			|| gpState.buttons.leftStick || gpState.buttons.rightStick || gpState.IsLeftTriggerPressed() || gpState.IsRightTriggerPressed() 
			|| gpState.dpad.down || gpState.dpad.left || gpState.dpad.right || gpState.dpad.up)
		{
			return true;
		}
	}

	return false;
}

bool hos::WindowsInput::AnyKeyDown()
{
	if (kbDevice->IsConnected())
	{
		Keyboard::State t = kbTracker->pressed;
		U32* ks = reinterpret_cast<U32*>(&t);
		for (int i = 0; i < 8; i++)
		{
			if (*(ks + i) ^ 0x0)
			{
				return true;
			}
		}
	}
	
	if (mDevice->IsConnected())
	{
		if (mTracker->leftButton == Mouse::ButtonStateTracker::PRESSED
			|| mTracker->middleButton == Mouse::ButtonStateTracker::PRESSED
			|| mTracker->rightButton == Mouse::ButtonStateTracker::PRESSED
			|| mTracker->xButton1 == Mouse::ButtonStateTracker::PRESSED
			|| mTracker->xButton2 == Mouse::ButtonStateTracker::PRESSED)
		{
			return true;
		}
	}

	DirectX::GamePad::State gpState = gpDevice->GetState(0);
	if (gpState.IsConnected())
	{
		if (gpTracker->a == GamePad::ButtonStateTracker::PRESSED
			|| gpTracker->b == GamePad::ButtonStateTracker::PRESSED
			|| gpTracker->x == GamePad::ButtonStateTracker::PRESSED
			|| gpTracker->y == GamePad::ButtonStateTracker::PRESSED
			|| gpTracker->back == GamePad::ButtonStateTracker::PRESSED
			|| gpTracker->view == GamePad::ButtonStateTracker::PRESSED
			|| gpTracker->leftShoulder == GamePad::ButtonStateTracker::PRESSED
			|| gpTracker->rightShoulder == GamePad::ButtonStateTracker::PRESSED
			|| gpTracker->leftStick == GamePad::ButtonStateTracker::PRESSED
			|| gpTracker->rightStick == GamePad::ButtonStateTracker::PRESSED
			|| gpTracker->leftTrigger == GamePad::ButtonStateTracker::PRESSED
			|| gpTracker->rightTrigger == GamePad::ButtonStateTracker::PRESSED
			|| gpTracker->dpadDown == GamePad::ButtonStateTracker::PRESSED
			|| gpTracker->dpadLeft == GamePad::ButtonStateTracker::PRESSED
			|| gpTracker->dpadRight == GamePad::ButtonStateTracker::PRESSED
			|| gpTracker->dpadUp == GamePad::ButtonStateTracker::PRESSED)
		{
			return true;
		}
	}

	return false;
}

bool hos::WindowsInput::AnyKeyUp()
{
	if(kbDevice->IsConnected())
	{
		Keyboard::State t = kbTracker->released;
		U32* ks = reinterpret_cast<U32*>(&t);
		for (int i = 0; i < 8; i++)
		{
			if (*(ks + i) ^ 0x0)
			{
				return true;
			}
		}
	}

	if (mDevice->IsConnected())
	{
		if (mTracker->leftButton == Mouse::ButtonStateTracker::RELEASED
			|| mTracker->middleButton == Mouse::ButtonStateTracker::RELEASED
			|| mTracker->rightButton == Mouse::ButtonStateTracker::RELEASED
			|| mTracker->xButton1 == Mouse::ButtonStateTracker::RELEASED
			|| mTracker->xButton2 == Mouse::ButtonStateTracker::RELEASED)
		{
			return true;
		}
	}

	DirectX::GamePad::State gpState = gpDevice->GetState(0);
	if (gpState.IsConnected())
	{
		if (gpTracker->a == GamePad::ButtonStateTracker::RELEASED
			|| gpTracker->b == GamePad::ButtonStateTracker::RELEASED
			|| gpTracker->x == GamePad::ButtonStateTracker::RELEASED
			|| gpTracker->y == GamePad::ButtonStateTracker::RELEASED
			|| gpTracker->back == GamePad::ButtonStateTracker::RELEASED
			|| gpTracker->view == GamePad::ButtonStateTracker::RELEASED
			|| gpTracker->leftShoulder == GamePad::ButtonStateTracker::RELEASED
			|| gpTracker->rightShoulder == GamePad::ButtonStateTracker::RELEASED
			|| gpTracker->leftStick == GamePad::ButtonStateTracker::RELEASED
			|| gpTracker->rightStick == GamePad::ButtonStateTracker::RELEASED
			|| gpTracker->leftTrigger == GamePad::ButtonStateTracker::RELEASED
			|| gpTracker->rightTrigger == GamePad::ButtonStateTracker::RELEASED
			|| gpTracker->dpadDown == GamePad::ButtonStateTracker::RELEASED
			|| gpTracker->dpadLeft == GamePad::ButtonStateTracker::RELEASED
			|| gpTracker->dpadRight == GamePad::ButtonStateTracker::RELEASED
			|| gpTracker->dpadUp == GamePad::ButtonStateTracker::RELEASED)
		{
			return true;
		}
	}

	return false;
}

bool hos::WindowsInput::GetKey(KeyCode key)
{
	if (key < KeyCode::KeyBoardKeys)
	{
		return kbDevice->GetState().IsKeyDown(static_cast<DirectX::Keyboard::Keys>(key));
	}
	else
	{
		switch (key)
		{
		case KeyCode::MouseLeft:
			return (mTracker->leftButton == Mouse::ButtonStateTracker::HELD);
			break;
		case KeyCode::MouseMiddle:
			return (mTracker->middleButton == Mouse::ButtonStateTracker::HELD);
			break;
		case KeyCode::MouseRight:
			return (mTracker->rightButton == Mouse::ButtonStateTracker::HELD);
			break;
		case KeyCode::MouseBtn1:
			return (mTracker->xButton1 == Mouse::ButtonStateTracker::HELD);
			break;
		case KeyCode::MouseBtn2:
			return (mTracker->xButton2 == Mouse::ButtonStateTracker::HELD);
			break;
		case KeyCode::GamePadA:
			return (gpTracker->a == GamePad::ButtonStateTracker::HELD);
			break;
		case KeyCode::GamePadB:
			return (gpTracker->b == GamePad::ButtonStateTracker::HELD);
			break;
		case KeyCode::GamePadX:
			return (gpTracker->x == GamePad::ButtonStateTracker::HELD);
			break;
		case KeyCode::GamePadY:
			return (gpTracker->y == GamePad::ButtonStateTracker::HELD);
			break;
		case KeyCode::GamePadLeftButton:
			return (gpTracker->leftShoulder == GamePad::ButtonStateTracker::HELD);
			break;
		case KeyCode::GamePadRightButton:
			return (gpTracker->rightShoulder == GamePad::ButtonStateTracker::HELD);
			break;
		case KeyCode::GamePadLeftStick:
			return (gpTracker->leftStick == GamePad::ButtonStateTracker::HELD);
			break;
		case KeyCode::GamePadRightStick:
			return (gpTracker->rightStick == GamePad::ButtonStateTracker::HELD);
			break;
		case KeyCode::GamePadBack:
			return (gpTracker->back == GamePad::ButtonStateTracker::HELD);
			break;
		case KeyCode::GamePadStart:
			return (gpTracker->start == GamePad::ButtonStateTracker::HELD);
			break;
		case KeyCode::GamePadUp:
			return (gpTracker->dpadUp == GamePad::ButtonStateTracker::HELD);
			break;
		case KeyCode::GamePadDown:
			return (gpTracker->dpadDown == GamePad::ButtonStateTracker::HELD);
			break;
		case KeyCode::GamePadRight:
			return (gpTracker->dpadRight == GamePad::ButtonStateTracker::HELD);
			break;
		case KeyCode::GamePadLeft:
			return (gpTracker->dpadLeft == GamePad::ButtonStateTracker::HELD);
			break;
		}
	}
	return false;
}

bool hos::WindowsInput::GetKeyDown(KeyCode key)
{
	if (key < KeyCode::KeyBoardKeys)
	{
		return kbTracker->IsKeyPressed(static_cast<DirectX::Keyboard::Keys>(key));
	}
	else
	{
		switch (key)
		{
		case KeyCode::MouseLeft:
			return (mTracker->leftButton == Mouse::ButtonStateTracker::PRESSED);
			break;
		case KeyCode::MouseMiddle:
			return (mTracker->middleButton == Mouse::ButtonStateTracker::PRESSED);
			break;
		case KeyCode::MouseRight:
			return (mTracker->rightButton == Mouse::ButtonStateTracker::PRESSED);
			break;
		case KeyCode::MouseBtn1:
			return (mTracker->xButton1 == Mouse::ButtonStateTracker::PRESSED);
			break;
		case KeyCode::MouseBtn2:
			return (mTracker->xButton2 == Mouse::ButtonStateTracker::PRESSED);
			break;
		case KeyCode::GamePadA:
			return (gpTracker->a == GamePad::ButtonStateTracker::PRESSED);
			break;
		case KeyCode::GamePadB:
			return (gpTracker->b == GamePad::ButtonStateTracker::PRESSED);
			break;
		case KeyCode::GamePadX:
			return (gpTracker->x == GamePad::ButtonStateTracker::PRESSED);
			break;
		case KeyCode::GamePadY:
			return (gpTracker->y == GamePad::ButtonStateTracker::PRESSED);
			break;
		case KeyCode::GamePadLeftButton:
			return (gpTracker->leftShoulder == GamePad::ButtonStateTracker::PRESSED);
			break;
		case KeyCode::GamePadRightButton:
			return (gpTracker->rightShoulder == GamePad::ButtonStateTracker::PRESSED);
			break;
		case KeyCode::GamePadLeftStick:
			return (gpTracker->leftStick == GamePad::ButtonStateTracker::PRESSED);
			break;
		case KeyCode::GamePadRightStick:
			return (gpTracker->rightStick == GamePad::ButtonStateTracker::PRESSED);
			break;
		case KeyCode::GamePadBack:
			return (gpTracker->back == GamePad::ButtonStateTracker::PRESSED);
			break;
		case KeyCode::GamePadStart:
			return (gpTracker->start == GamePad::ButtonStateTracker::PRESSED);
			break;
		case KeyCode::GamePadUp:
			return (gpTracker->dpadUp == GamePad::ButtonStateTracker::PRESSED);
			break;
		case KeyCode::GamePadDown:
			return (gpTracker->dpadDown == GamePad::ButtonStateTracker::PRESSED);
			break;
		case KeyCode::GamePadRight:
			return (gpTracker->dpadRight == GamePad::ButtonStateTracker::PRESSED);
			break;
		case KeyCode::GamePadLeft:
			return (gpTracker->dpadLeft == GamePad::ButtonStateTracker::PRESSED);
			break;
		}
	}
	return false;
}

bool hos::WindowsInput::GetKeyUp(KeyCode key)
{
	if (key < KeyCode::KeyBoardKeys)
	{
		return kbTracker->IsKeyReleased(static_cast<DirectX::Keyboard::Keys>(key));
	}
	else
	{
		switch (key)
		{
		case KeyCode::MouseLeft:
			return (mTracker->leftButton == Mouse::ButtonStateTracker::RELEASED);
			break;
		case KeyCode::MouseMiddle:
			return (mTracker->middleButton == Mouse::ButtonStateTracker::RELEASED);
			break;
		case KeyCode::MouseRight:
			return (mTracker->rightButton == Mouse::ButtonStateTracker::RELEASED);
			break;
		case KeyCode::MouseBtn1:
			return (mTracker->xButton1 == Mouse::ButtonStateTracker::RELEASED);
			break;
		case KeyCode::MouseBtn2:
			return (mTracker->xButton2 == Mouse::ButtonStateTracker::RELEASED);
			break;
		case KeyCode::GamePadA:
			return (gpTracker->a == GamePad::ButtonStateTracker::RELEASED);
			break;
		case KeyCode::GamePadB:
			return (gpTracker->b == GamePad::ButtonStateTracker::RELEASED);
			break;
		case KeyCode::GamePadX:
			return (gpTracker->x == GamePad::ButtonStateTracker::RELEASED);
			break;
		case KeyCode::GamePadY:
			return (gpTracker->y == GamePad::ButtonStateTracker::RELEASED);
			break;
		case KeyCode::GamePadLeftButton:
			return (gpTracker->leftShoulder == GamePad::ButtonStateTracker::RELEASED);
			break;
		case KeyCode::GamePadRightButton:
			return (gpTracker->rightShoulder == GamePad::ButtonStateTracker::RELEASED);
			break;
		case KeyCode::GamePadLeftStick:
			return (gpTracker->leftStick == GamePad::ButtonStateTracker::RELEASED);
			break;
		case KeyCode::GamePadRightStick:
			return (gpTracker->rightStick == GamePad::ButtonStateTracker::RELEASED);
			break;
		case KeyCode::GamePadBack:
			return (gpTracker->back == GamePad::ButtonStateTracker::RELEASED);
			break;
		case KeyCode::GamePadStart:
			return (gpTracker->start == GamePad::ButtonStateTracker::RELEASED);
			break;
		case KeyCode::GamePadUp:
			return (gpTracker->dpadUp == GamePad::ButtonStateTracker::RELEASED);
			break;
		case KeyCode::GamePadDown:
			return (gpTracker->dpadDown == GamePad::ButtonStateTracker::RELEASED);
			break;
		case KeyCode::GamePadRight:
			return (gpTracker->dpadRight == GamePad::ButtonStateTracker::RELEASED);
			break;
		case KeyCode::GamePadLeft:
			return (gpTracker->dpadLeft == GamePad::ButtonStateTracker::RELEASED);
			break;
		}
	}
	return false;
}

float hos::WindowsInput::GetAxis(Axis axis)
{
	float delta = 0;
	switch (axis)
	{
	case MouseX:
		if (MouseMode != eMouseMode::LOCKED)
		{
			delta = (float)(mTracker->GetLastState().x - mDevice->GetState().x);
		}
		else
		{
			delta = (float)(((rect.right - rect.left) / 2) - mDevice->GetState().x);
		}
		return delta;
		break;
	case MouseY:
		if (MouseMode != eMouseMode::LOCKED)
		{
			delta = (float)(mTracker->GetLastState().y - mDevice->GetState().y);
		}
		else
		{
			delta = (float)(((rect.bottom - rect.top) / 2) - mDevice->GetState().y);
		}
		return delta;
		break;
	case MouseScroll:
	{
		float wheel = static_cast<float>(mDevice->GetState().scrollWheelValue);
		mDevice->ResetScrollWheelValue();
		return wheel;
		break;
	}
	case GamePadLeftX:
		return gpDevice->GetState(0).thumbSticks.leftX;
		break;
	case GamePadLeftY:
		return gpDevice->GetState(0).thumbSticks.leftY;
		break;
	case GamePadRightX:
		return gpDevice->GetState(0).thumbSticks.rightX;
		break;
	case GamePadRightY:
		return gpDevice->GetState(0).thumbSticks.rightY;
		break;
	case GamePadLeftTrigger:
		return gpDevice->GetState(0).triggers.left;
		break;
	case GamePadRightTrigger:
		return gpDevice->GetState(0).triggers.right;
		break;
	}
	return 0.f;
}

KeyCode hos::WindowsInput::GetInputKey()
{
	Keyboard::State t = kbTracker->pressed;
	U32* ks = reinterpret_cast<U32*>(&t);
	int checkbit = 0;
	int ksbit = 0;
	for (int i = 0; i < 256 / 32; i++)
	{
		ksbit = *(ks + i);
		for (int j = 0; j < 32; j++)
		{
			checkbit = 0x80000000 >> j;
			if (ksbit & checkbit)
			{
				return static_cast<KeyCode>((i * 32) + (31 - j));
			}
		}
	}

	return KeyCode::None;
}

bool hos::WindowsInput::IsNumLock()
{
	return GetKeyState(VK_NUMLOCK) < 0;
}

bool hos::WindowsInput::IsCapsLock()
{
	return GetKeyState(VK_CAPITAL) < 0;
}

bool hos::WindowsInput::IsScrLock()
{
	return GetKeyState(VK_SCROLL) < 0;
}

bool hos::WindowsInput::IsHangul()
{
	return GetKeyState(VK_HANGUL) < 0;
}

bool hos::WindowsInput::IsShift()
{
	return kbDevice->GetState().LeftShift || kbDevice->GetState().RightShift;
}

bool hos::WindowsInput::IsLShift()
{
	return kbDevice->GetState().LeftShift;
}

bool hos::WindowsInput::IsRShift()
{
	return kbDevice->GetState().RightShift;
}

bool hos::WindowsInput::IsCtrl()
{
	return kbDevice->GetState().LeftControl || kbDevice->GetState().RightControl;
}

bool hos::WindowsInput::IsLCtrl()
{
	return kbDevice->GetState().LeftControl;
}

bool hos::WindowsInput::IsRCtrl()
{
	return kbDevice->GetState().RightControl;
}

bool hos::WindowsInput::IsAlt()
{
	return kbDevice->GetState().LeftAlt || kbDevice->GetState().RightAlt;
}

bool hos::WindowsInput::IsLAlt()
{
	return kbDevice->GetState().LeftAlt;
}

bool hos::WindowsInput::IsRAlt()
{
	return kbDevice->GetState().RightAlt;
}

POINT hos::WindowsInput::GetMousePosition() const
{
	Mouse::State mouse = mDevice->GetState();
	return POINT{ mouse.x, mouse.y };
}

void hos::WindowsInput::SetMouseCursor(HCURSOR cur)
{
	SendMessageW(H_Wnd, WM_USER + 1, (WPARAM)cur, 0);
}

void hos::WindowsInput::SetMouseVisible(bool b)
{
	SendMessageW(H_Wnd, WM_USER + 2, (WPARAM)b, 0);
}

hos::WindowsInput::eMouseMode hos::WindowsInput::GetMouseMode() const
{
	return MouseMode;
}

void hos::WindowsInput::SetMouseMode(eMouseMode e)
{
	MouseMode = e;
	if (MouseMode == eMouseMode::NONE)
	{
		ClipCursor(nullptr);
	}
}
