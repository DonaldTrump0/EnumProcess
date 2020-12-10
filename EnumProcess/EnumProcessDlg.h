
// EnumProcessDlg.h : header file
//

#pragma once


// CEnumProcessDlg dialog
class CEnumProcessDlg : public CDialogEx
{
// Construction
public:
	CEnumProcessDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ENUMPROCESS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	void EnumProcess();
	void EnumModules(DWORD dwPID);

private:
	CImageList m_ImageList;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ProcessListCtrl;
	afx_msg void OnRclickListProcess(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnumThread();
	afx_msg void OnEnumModule();
	afx_msg void OnEnumHeap();
	afx_msg void OnEnumWindow();
};
