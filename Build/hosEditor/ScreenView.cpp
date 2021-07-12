// ScreenView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditer.h"
#include "ScreenView.h"
#include "afxdialogex.h"


// ScreenView 대화 상자

IMPLEMENT_DYNAMIC(ScreenView, CDialogEx)

ScreenView::ScreenView(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ScreenView, pParent)
{

}

ScreenView::~ScreenView()
{
}

void ScreenView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ScreenView, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// ScreenView 메시지 처리기


BOOL ScreenView::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::Create(lpszTemplateName, pParentWnd);
}


BOOL ScreenView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	//EditerManager::GetIns()->Initialize(this->GetSafeHwnd());

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void ScreenView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
}
