
// hosEditorDlg.h: 헤더 파일
//

#pragma once
#include "EditorManager.h"

class SceneView;
class TransformView;
class BoxCollisionView;
class SphereCollisionView;
class CapsuleCollisionView;
class CameraView;
class RigidbodyView;
class ModelView;
class AudioListenerView;
class AudioSourceView;
class ScriptView;
class LIghtView;
class MeshFilterView;
class MeshRendererView;
class SkinnedMeshRendererView;
class AnimationView;
class UIImageView;
class UITextView;
class UIButtonView;
class UIInputFieldView;
class SceneInfoView;
class NavInfoView;

// ChosEditorDlg 대화 상자
class ChosEditorDlg : public CDialogEx
{
	// 생성입니다.
public:
	ChosEditorDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HOSEditor_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	TransformView* m_TransformView;
	BoxCollisionView* m_BoxCollisionView;
	SphereCollisionView* m_SphereCollisionView;
	CapsuleCollisionView* m_CapsuleCollisionView;
	CameraView* m_CameraView;
	RigidbodyView* m_RigidbodyView;
	ModelView* m_ModelView;
	AudioListenerView* m_AudioListenerView;
	AudioSourceView* m_AudioSourceView;
	ScriptView* m_ScriptView;
	LIghtView* m_LightView;
	MeshFilterView* m_MeshFilterView;
	MeshRendererView* m_MeshRendererView;
	SkinnedMeshRendererView* m_SkinnedMeshRendererView;
	AnimationView* m_AnimationView;
	UIImageView* m_UIImageView;
	UITextView* m_UITextView;
	UIButtonView* m_UIButtonView;
	UIInputFieldView* m_UIInputFieldView;
	SceneInfoView* m_SceneInfoView;
	NavInfoView* m_NavInfoView;

public:
	SceneView* m_SceneView;

private:
	void ResetComponentList(CString* gameObject);

public:
	void HideAllComponentView();

private:
	void ResetAllComponentView();
	void UpdateGameObjectHierarchy();
	void MakeGameObjectHierarchy(hos::com::GameObject* gameObject, HTREEITEM* treeItem);

public:
	CTreeCtrl GameObjectHierachy;
	afx_msg void OnBnClickedButtonCreateGameObject();
	afx_msg void OnBnClickedButtonChangeGameObjectName();
//	afx_msg void OnClose();
	afx_msg void OnTvnSelchangedTreeGameObjectHierarchy(NMHDR* pNMHDR, LRESULT* pResult);
	CEdit GameObjectName;
	afx_msg void OnBnClickedButtonRemoveGameObject();
	CListBox ComponentList;
	afx_msg void OnNMRClickTreeGameObjectHierarchy(NMHDR* pNMHDR, LRESULT* pResult);
	CComboBox ComponentSelectList;
	afx_msg void OnBnClickedButtonAddComponent();
	afx_msg void OnBnClickedButtonRemoveComponent();
	afx_msg void OnLbnSelchangeListComponentList();
	afx_msg void OnEnChangeEditSceneName();
	afx_msg void OnBnClickedButtonSceneSave();
	afx_msg void OnBnClickedButtonSceneLoad();
	CEdit EditSceneName;
	afx_msg void OnBnClickedButtonGameObjectSave();
	afx_msg void OnBnClickedButtonGameObjectLoad();
	afx_msg void OnTvnBegindragTreeGameObjectHierarchy(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButtonAddGameData();
	afx_msg void OnBnClickedButtonLoadGameData();
	CListBox ListPrefabList;
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void ExpandAll(HTREEITEM hUserRoot);
	void CollapseAll(HTREEITEM hUserRoot);
	afx_msg void OnNMDblclkTreeGameObjectHierarchy(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButtonSceneInfo();
	afx_msg void OnBnClickedButtonNavInfo();
};
