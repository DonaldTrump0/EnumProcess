// CEnumModuleDlg.cpp : implementation file
//

#include "pch.h"
#include "EnumProcess.h"
#include "CEnumModuleDlg.h"
#include "afxdialogex.h"
#include <tlhelp32.h>
#include <Psapi.h>

// CEnumModuleDlg dialog

IMPLEMENT_DYNAMIC(CEnumModuleDlg, CDialogEx)

CEnumModuleDlg::CEnumModuleDlg(DWORD dwPID, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ENUMPROCESS_DIALOG, pParent)
{
	m_dwPID = dwPID;
}

CEnumModuleDlg::~CEnumModuleDlg()
{
}

void CEnumModuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_ProcessListCtrl);
}


BEGIN_MESSAGE_MAP(CEnumModuleDlg, CDialogEx)
END_MESSAGE_MAP()


// CEnumModuleDlg message handlers

// 遍历模块
void CEnumModuleDlg::EnumModules()
{
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;

    // Take a snapshot of all modules in the specified process.
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_dwPID);
    if (hModuleSnap == INVALID_HANDLE_VALUE)
    {
        TRACE(TEXT("CreateToolhelp32Snapshot (of modules)"));
        return;
    }

    // Set the size of the structure before using it.
    me32.dwSize = sizeof(MODULEENTRY32);

    // Retrieve information about the first module,
    // and exit if unsuccessful
    if (!Module32First(hModuleSnap, &me32))
    {
        TRACE(TEXT("Module32First"));  // show cause of failure
        CloseHandle(hModuleSnap);           // clean the snapshot object
        return;
    }

    // Now walk the module list of the process,
    // and display information about each module
    int i = 0;
    CString str;
    do
    {
        m_ProcessListCtrl.InsertItem(i, NULL);
        m_ProcessListCtrl.SetItemText(i, 0, me32.szModule);
        m_ProcessListCtrl.SetItemText(i, 1, me32.szExePath);
        str.Format("%d", me32.th32ProcessID);
        m_ProcessListCtrl.SetItemText(i, 2, str);
        str.Format("%d", me32.GlblcntUsage);
        m_ProcessListCtrl.SetItemText(i, 3, str);
        str.Format("%d", me32.ProccntUsage);
        m_ProcessListCtrl.SetItemText(i, 4, str);
        str.Format("%X", (DWORD)me32.modBaseAddr);
        m_ProcessListCtrl.SetItemText(i, 5, str);
        str.Format("%d", me32.modBaseSize);
        m_ProcessListCtrl.SetItemText(i, 6, str);

        i++;
    } while (Module32Next(hModuleSnap, &me32));

    CloseHandle(hModuleSnap);
}

BOOL CEnumModuleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 初始化模块列表
	m_ProcessListCtrl.InsertColumn(0, "Module name", LVCFMT_LEFT, 150);
	m_ProcessListCtrl.InsertColumn(1, "Module path", LVCFMT_LEFT, 150);
	m_ProcessListCtrl.InsertColumn(2, "PID", LVCFMT_LEFT, 100);
	m_ProcessListCtrl.InsertColumn(3, "Ref count(g)", LVCFMT_LEFT, 100);
	m_ProcessListCtrl.InsertColumn(4, "Ref count(p)", LVCFMT_LEFT, 100);
	m_ProcessListCtrl.InsertColumn(5, "Base address", LVCFMT_LEFT, 100);
	m_ProcessListCtrl.InsertColumn(6, "Base size", LVCFMT_LEFT, 100);

	m_ProcessListCtrl.SetExtendedStyle(m_ProcessListCtrl.GetExtendedStyle()
		| LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    EnumModules();

	return TRUE;
}
