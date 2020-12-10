
// EnumProcessDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "EnumProcess.h"
#include "EnumProcessDlg.h"
#include "afxdialogex.h"
#include "CEnumModuleDlg.h"
#include <tlhelp32.h>
#include <Psapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEnumProcessDlg dialog



CEnumProcessDlg::CEnumProcessDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ENUMPROCESS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEnumProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_ProcessListCtrl);
}

// 遍历进程
void CEnumProcessDlg::EnumProcess()
{
    // Take a snapshot of all processes in the system.
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        TRACE("CreateToolhelp32Snapshot (of processes)");
        return;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process, and exit if unsuccessful
    if (!Process32First(hProcessSnap, &pe32))
    {
        TRACE("Process32First");
        CloseHandle(hProcessSnap);
        return;
    }

    // Now walk the snapshot of processes, and display information about each process in turn
    int i = 0;
    do
    {
        char szFilePath[MAX_PATH] = { 0 };
        int nIdx = 0;

        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        if (hProcess == NULL)
        {
            TRACE("OpenProcess");
        }
        else
        {
            GetModuleFileNameEx(hProcess, NULL, szFilePath, sizeof(szFilePath));

            HICON hIcon = ::ExtractIcon(AfxGetInstanceHandle(), szFilePath, 0);
            if (hIcon != NULL)
            {
                nIdx = m_ImageList.Add(hIcon);
            }

            CloseHandle(hProcess);
        }

        CString str;
        m_ProcessListCtrl.InsertItem(i, NULL, nIdx);
        m_ProcessListCtrl.SetItemText(i, 0, pe32.szExeFile);
        str.Format("%d", pe32.th32ProcessID);
        m_ProcessListCtrl.SetItemText(i, 1, str);
        str.Format("%d", pe32.th32ParentProcessID);
        m_ProcessListCtrl.SetItemText(i, 2, str);
        str.Format("%d", pe32.cntThreads);
        m_ProcessListCtrl.SetItemText(i, 3, str);
        m_ProcessListCtrl.SetItemText(i, 4, szFilePath);

		i++;
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
}

BEGIN_MESSAGE_MAP(CEnumProcessDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_NOTIFY(NM_RCLICK, IDC_LIST_PROCESS, &CEnumProcessDlg::OnRclickListProcess)
    ON_COMMAND(ID_ENUM_THREAD, &CEnumProcessDlg::OnEnumThread)
    ON_COMMAND(ID_ENUM_MODULE, &CEnumProcessDlg::OnEnumModule)
    ON_COMMAND(ID_ENUM_HEAP, &CEnumProcessDlg::OnEnumHeap)
    ON_COMMAND(ID_ENUM_WINDOW, &CEnumProcessDlg::OnEnumWindow)
END_MESSAGE_MAP()

BOOL EnableDebugPrivilege()
{
    HANDLE hToken;
    LUID DebugNameValue;
    TOKEN_PRIVILEGES Privileges;
    DWORD dwRet;

    //获取当前进程的令牌权限
    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

    //查询调试令牌权限的luid
    LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &DebugNameValue);

    //调整进程权限为调试令牌权限
    Privileges.PrivilegeCount = 1;
    Privileges.Privileges[0].Luid = DebugNameValue;
    Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    AdjustTokenPrivileges(hToken, FALSE, &Privileges, sizeof(Privileges), NULL, &dwRet);
    CloseHandle(hToken);

    return TRUE;
}

// CEnumProcessDlg message handlers

BOOL CEnumProcessDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    // 设置图标列表
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    m_ImageList.Create(cxIcon / 2, cyIcon / 2, ILC_COLOR32 | ILC_MASK, 0, 2);
    HICON hIcon = ::LoadIcon(NULL, IDI_WINLOGO);
    m_ImageList.Add(hIcon);
    m_ProcessListCtrl.SetImageList(&m_ImageList, LVSIL_SMALL);

    // 初始化进程列表
	m_ProcessListCtrl.InsertColumn(0, "Process name", LVCFMT_LEFT, 150);
	m_ProcessListCtrl.InsertColumn(1, "PID", LVCFMT_LEFT, 100);
	m_ProcessListCtrl.InsertColumn(2, "Parent PID", LVCFMT_LEFT, 100);
	m_ProcessListCtrl.InsertColumn(3, "Thread count", LVCFMT_LEFT, 100);
	m_ProcessListCtrl.InsertColumn(4, "Process Path", LVCFMT_LEFT, 300);

    m_ProcessListCtrl.SetExtendedStyle(m_ProcessListCtrl.GetExtendedStyle()
        | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    EnableDebugPrivilege();
	EnumProcess();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEnumProcessDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEnumProcessDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// 右键弹菜单
void CEnumProcessDlg::OnRclickListProcess(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    *pResult = 0;

    CMenu popMenu;
    popMenu.LoadMenu(IDR_MENU_ENUM);
    CMenu* subMenu = popMenu.GetSubMenu(0);

    CPoint pt;
    ::GetCursorPos(&pt);

    subMenu->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
}


void CEnumProcessDlg::OnEnumThread()
{
    AfxMessageBox("EnumThread");
}


void CEnumProcessDlg::OnEnumModule()
{
    POSITION pos = m_ProcessListCtrl.GetFirstSelectedItemPosition();
    CString str = m_ProcessListCtrl.GetItemText((int)pos - 1, 1);

    DWORD dwPID = 0;
    sscanf(str, "%d", &dwPID);

    //EnumModules(dwPID);
    CEnumModuleDlg dlg(dwPID);
    dlg.DoModal();
}


void CEnumProcessDlg::OnEnumHeap()
{
    // TODO: Add your command handler code here
}


void CEnumProcessDlg::OnEnumWindow()
{
    // TODO: Add your command handler code here
}
