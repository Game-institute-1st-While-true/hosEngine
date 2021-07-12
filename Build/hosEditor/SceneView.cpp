// SceneView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditor.h"
#include "EditorManager.h"
#include "SceneView.h"
#include "afxdialogex.h"
// SceneView 대화 상자

IMPLEMENT_DYNAMIC(SceneView, CDialogEx)

SceneView::SceneView(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SceneView, pParent)
{

}

SceneView::~SceneView()
{
}

void SceneView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SceneView, CDialogEx)
	ON_WM_PAINT()
	ON_WM_KILLFOCUS()
	//	ON_WM_MOUSEHOVER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// SceneView 메시지 처리기


BOOL SceneView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


//BOOL SceneView::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
//{
//	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
//
//	return CDialogEx::Create(lpszTemplateName, pParentWnd);
//}


void SceneView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.

	EditorManager::GetIns()->EditorLoop();

	IsDraw = false;

}


void SceneView::OnKillFocus(CWnd* pNewWnd)
{
	CDialogEx::OnKillFocus(pNewWnd);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	IsDraw = true;

	if (IsDraw)
	{
		SendMessage(WM_PAINT, 0, 0);
	}

}


//void SceneView::OnMouseHover(UINT nFlags, CPoint point)
//{
//	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
//
//
//	CDialogEx::OnMouseHover(nFlags, point);
//}


void SceneView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	float deltaX = 0.f;
	float deltaY = 0.f;
	if (IsMove)
	{
		deltaX = point.x - mousePosX;
		deltaY = point.y - mousePosY;
	}
	if (EditorManager::GetIns()->GetRenderer())
	{
		hos::com::Scene* nowscene = EditorManager::GetIns()->GetNowScene();
		if (nowscene)
		{
			hos::com::Camera* camera = nowscene->GetMainCamera();
			if (camera)
			{
				if (camera->m_GameObject)
				{
					hos::com::Transform* trans = camera->m_GameObject->transform;
					if (trans)
					{
						hos::Rotator rot = trans->GetRotation();
						trans->SetRotation({ rot.x + (deltaY * 0.5f), rot.y + (deltaX * 0.5f),rot.z });
					}
				}
			}
		}
	}

	mousePosX = point.x;
	mousePosY = point.y;
	CDialogEx::OnMouseMove(nFlags, point);
}


void SceneView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	bLBTDOWN = true;

	CDialogEx::OnLButtonDown(nFlags, point);
}


void SceneView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	bLBTDOWN = false;

	CDialogEx::OnLButtonUp(nFlags, point);
}


LRESULT SceneView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (EditorManager::GetIns()->GetRenderer())
	{
		hos::com::Scene* nowscene = EditorManager::GetIns()->GetNowScene();
		if (nowscene)
		{
			hos::com::Camera* camera = nowscene->GetMainCamera();
			if (camera)
			{
				if (camera->m_GameObject)
				{
					hos::com::Transform* trans = camera->m_GameObject->transform;
					if (trans)
					{
						static float speed = 100.f;
						float speedDelta = 0.f;
						static float wheelSpeed = 0.0f;
						hos::Vector3 pos = trans->GetPosition();
						switch (message)
						{
						case WM_RBUTTONDOWN:
							IsMove = true;
							SetCapture();
							break;
						case WM_RBUTTONUP:
							IsMove = false;
							ReleaseCapture();
							break;
						case WM_KEYDOWN:
							if (IsMove)
							{
								switch (wParam)
								{
								case 'W':
									trans->SetPosition(pos + (-trans->GetForward() * (speed * Time->DeltaTime())));
									break;
								case 'A':
									trans->SetPosition(pos + (trans->GetLeft() * (speed * Time->DeltaTime())));
									break;
								case 'S':
									trans->SetPosition(pos + (trans->GetForward() * (speed * Time->DeltaTime())));
									break;
								case 'D':
									trans->SetPosition(pos + (-trans->GetLeft() * (speed * Time->DeltaTime())));
									break;
								}
							}
							break;
						case WM_MOUSEWHEEL:

							if (GetKeyState(VK_LCONTROL)<0 || GetKeyState(VK_RCONTROL)<0)
							{
								wheelSpeed = 0.001;
							}
							else
							{
								if (IsMove)
								{
									wheelSpeed = 0.008;
								}
								else
								{
									wheelSpeed = 1.f;
								}
							}

							speedDelta = GET_WHEEL_DELTA_WPARAM(wParam) * wheelSpeed;
							speed += speedDelta;
							speed = std::clamp(speed, 0.1f, std::numeric_limits<float>::max());
							break;
						}
						if (g_Renderer)
						{
							g_Renderer->_DebugText(0, 25, 20, hos::Vector4(1, 1, 1, 1), L"Camera Speed :%.1f", speed);
						}
						g_Renderer->_DebugText(0, 60, 20, hos::Vector4(1, 1, 1, 1), L"MousePoint(%d, %d)", mousePosX, mousePosY);
					}
				}
			}
		}
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}


BOOL SceneView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN) // ENTER키 눌릴 시
			return TRUE;
		else if (pMsg->wParam == VK_ESCAPE) // ESC키 눌릴 시
			return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
