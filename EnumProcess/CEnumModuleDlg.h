#pragma once

// CEnumModuleDlg dialog

class CEnumModuleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEnumModuleDlg)

public:
	CEnumModuleDlg(DWORD dwPID, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CEnumModuleDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ENUMPROCESS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	DWORD m_dwPID;
	CListCtrl m_ProcessListCtrl;

	void EnumModules();

	virtual BOOL OnInitDialog();
};
