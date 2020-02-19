
// MyChatAppDlg.cpp : implementation file

#include "pch.h"
#include "framework.h"
#include "MyChatApp.h"
#include "MyChatAppDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


HANDLE ThreadHandle;
DWORD ThreadID;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMyChatAppDlg dialog



CMyChatAppDlg::CMyChatAppDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MYCHATAPP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyChatAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_MESSAGE, m_editmsg);
}

BEGIN_MESSAGE_MAP(CMyChatAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEND, &CMyChatAppDlg::OnBnClickedSend)
END_MESSAGE_MAP()


// CMyChatAppDlg message handlers
DWORD WINAPI receiveMessage(LPVOID param) {

	CMyChatAppDlg* dlg = (CMyChatAppDlg*)param;

	SOCKET RecvSocket;
	CString chatMessage;
	CString user = "Friend: ";

	sockaddr_in RecvAddr;
	sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);

	char buffer[1024] = { 0 };
	int bufferLen = 1024;

	RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (RecvSocket == INVALID_SOCKET) {
		AfxMessageBox("Create socket failed.");
	}

	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(3515);
	RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int iRecResult = bind(RecvSocket, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));

	if (iRecResult != 0) {
		AfxMessageBox("bind failed");
		return 1;
	}
	
	while (1) {

		int recvMsgSize = recvfrom(RecvSocket, buffer, bufferLen, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);

		if (recvMsgSize < 0) {
			AfxMessageBox("Recvfrom() failed.");
		}

		buffer[recvMsgSize] = '\0';

		chatMessage = buffer;
		
		int chatLength = dlg->m_editmsg.GetWindowTextLengthA();
		dlg->m_editmsg.SetSel(chatLength, chatLength);
		dlg->m_editmsg.ReplaceSel(user + chatMessage);
		dlg->m_editmsg.ReplaceSel("\r\n");
		
		memset(buffer, 0, strlen(buffer));
	}

	closesocket(RecvSocket);

	return 0;
}


BOOL CMyChatAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	ThreadHandle = CreateThread(NULL, 0, receiveMessage, this, 0, &ThreadID);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CMyChatAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMyChatAppDlg::OnPaint()
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
HCURSOR CMyChatAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMyChatAppDlg::OnBnClickedSend()
{
	// TODO: Add your control notification handler code here
	
	SOCKET SendSocket;
	char buf[1024];

	sockaddr_in SendAddr;

	SendAddr.sin_family = AF_INET;
	SendAddr.sin_port = htons(3514);
	InetPton(AF_INET, _T("127.0.0.1"), &SendAddr.sin_addr.s_addr);
	
	SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (SendSocket == INVALID_SOCKET) {
		AfxMessageBox("Create SendSocket failed");
	}

	CString chatMessage;
	CString user = "Me: ";

	GetDlgItemText(IDC_EDIT_CHAT, buf, 1024);
	GetDlgItemText(IDC_EDIT_MESSAGE, chatMessage);

	chatMessage += user + buf + "\r\n";

	SetDlgItemText(IDC_EDIT_MESSAGE, chatMessage);
	SetDlgItemText(IDC_EDIT_CHAT, "");

	int len = chatMessage.GetLength();
	buf[len] = '\0';

	int iSendRes = sendto(SendSocket, buf, strlen(buf), 0, (SOCKADDR*)&SendAddr, sizeof(SendAddr));
	
	if (iSendRes == SOCKET_ERROR) {
		AfxMessageBox("sendto failed.");
	}

	memset(buf, 0, strlen(buf));

	closesocket(SendSocket);

}