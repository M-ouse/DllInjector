
// DLLinjectorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DLLinjector.h"
#include "DLLinjectorDlg.h"
#include "afxdialogex.h"
#include <windows.h>
#include <Tlhelp32.h>
#include <afxpriv.h>
#include <atlconv.h> 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString DllPath;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDLLinjectorDlg 对话框



CDLLinjectorDlg::CDLLinjectorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDLLinjectorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDLLinjectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDLLinjectorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, &CDLLinjectorDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CDLLinjectorDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDLLinjectorDlg 消息处理程序

BOOL CDLLinjectorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDLLinjectorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDLLinjectorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDLLinjectorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int Inject(int Pid)
{
	// path to our dll
	//LPCSTR DllPath = "";
	CT2A DllPathFormat(DllPath.GetString());//Convert CString to char*,fuck
	//MessageBoxA(NULL,DllPathFormat,NULL,0);
	//return 0;
	// Open a handle to target process
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);

	// Allocate memory for the dllpath in the target process
	// length of the path string + null terminator
	LPVOID pDllPath = VirtualAllocEx(hProcess, 0, strlen(DllPathFormat) + 1,
		MEM_COMMIT, PAGE_READWRITE);

	// Write the path to the address of the memory we just allocated
	// in the target process
	WriteProcessMemory(hProcess, pDllPath, (LPVOID)DllPathFormat,
		strlen(DllPathFormat) + 1, 0);

	// Create a Remote Thread in the target process which
	// calls LoadLibraryA as our dllpath as an argument -> program loads our dll
	HANDLE hLoadThread = CreateRemoteThread(hProcess, 0, 0,
		(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"),
		"LoadLibraryA"), pDllPath, 0, 0);

	// Wait for the execution of our loader thread to finish
	//WaitForSingleObject(hLoadThread, INFINITE);
	//WaitForSingleObject(hLoadThread, 1000);
	//std::cout << "Dll path allocated at: " << std::hex << pDllPath << std::endl;
	//std::cin.get();

	// Free the memory allocated for our dll path
	VirtualFreeEx(hProcess, pDllPath, strlen(DllPathFormat) + 1, MEM_RELEASE);
	return 1;
}

void CDLLinjectorDlg::OnBnClickedButton2()
{
	// TODO:  在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE, NULL, NULL, NULL, NULL);
	if (dlg.DoModal() == IDOK)//
	{
		CString str, str1;
		str = dlg.GetPathName();
		str1 = dlg.GetFileName();
		CEdit* cfolder;
		cfolder = (CEdit*)GetDlgItem(IDC_EDIT2);
		cfolder->SetWindowText(str);
		DllPath = str;
		// + 1
	}
}


void CDLLinjectorDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	CString ProcessName;
	CString Success, Fail0("Can't find the process."),Fail1("Pid is invalid.");
	int Pid=-1;
	GetDlgItem(IDC_EDIT1)->GetWindowText(ProcessName);

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE != hSnapshot)
	{
		PROCESSENTRY32 pi;
		pi.dwSize = sizeof(PROCESSENTRY32);
		BOOL bRet = Process32First(hSnapshot, &pi);
		while (bRet)
		{
			WCHAR *tmp = pi.szExeFile;
			CString cstring(tmp);
			//Success.Format(_T("Inject successfully\nPid : %d"), pi.th32ProcessID);
			if (ProcessName == tmp){
				Pid = pi.th32ProcessID;
				break;
			}
			//printf("进程ID = %d ,进程路径 = %s\r\n", pi.th32ProcessID, pi.szExeFile);
			bRet = Process32Next(hSnapshot, &pi);
		}
	}
	if (Pid != -1)
		Inject(Pid);
	else{
		MessageBeep(0);
		MessageBox(Fail1);
	}
}
