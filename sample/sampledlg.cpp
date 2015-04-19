
// sampledlg.cpp : implementation file
//

#include "stdafx.h"
#include "sample.h"
#include "sampledlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

 #pragma warning( disable : 4995)

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CSampleDlg dialog

CSampleDlg::CSampleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSampleDlg::IDD, pParent)

{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLAY_BTN, m_btnPlay);
	DDX_Control(pDX, IDC_PAUSE_BTN, m_btnPause);
	DDX_Control(pDX, IDC_STOP_BTN, m_btnStop);
	DDX_Control(pDX, IDC_WIDTH_STATIC, m_stcWidth);
	DDX_Control(pDX, IDC_HEIGHT_STATIC, m_stcHeight);
	DDX_Control(pDX, IDC_WIDTH_EDIT, m_edtWidth);
	DDX_Control(pDX, IDC_HEIGHT_EDIT, m_edtHeight);
	DDX_Control(pDX, IDC_SET_BTN, m_btnSet);
	DDX_Control(pDX, IDC_FILE_STATIC, m_stcPptx);
	DDX_Control(pDX, IDC_FILE_EDIT, m_edtFile);
	DDX_Control(pDX, IDC_BROWSE_BTN, m_btnBrowse);
	DDX_Control(pDX, IDC_FPS_SPIN, m_spnFps);
	DDX_Control(pDX, IDC_FPS_EDIT, m_edtFps);
	DDX_Control(pDX, IDC_FPS_STATIC, m_stcFps);
	DDX_Control(pDX, IDC_SHOW_STAT_CHECK, m_chkShowStat);
}

BEGIN_MESSAGE_MAP(CSampleDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PLAY_BTN, &CSampleDlg::OnBnClickedPlayBtn)
	ON_BN_CLICKED(IDC_PAUSE_BTN, &CSampleDlg::OnBnClickedPauseBtn)
	ON_BN_CLICKED(IDC_STOP_BTN, &CSampleDlg::OnBnClickedStopBtn)
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_SET_BTN, &CSampleDlg::OnBnClickedSetBtn)
	ON_BN_CLICKED(IDC_BROWSE_BTN, &CSampleDlg::OnBnClickedBrowseBtn)
	ON_WM_CLOSE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


// CSampleDlg message handlers

BOOL CSampleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CoInitialize(NULL);
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CRect clrect;
	GetClientRect(&clrect);
	m_playerWnd.Create(_T("STATIC"), L"", WS_CHILD | WS_VISIBLE,
		CRect(clrect.left+10, clrect.top+10, clrect.right-10, clrect.bottom-60), this, 1234);


	m_edtWidth.SetWindowText(L"1024");
	m_edtHeight.SetWindowText(L"768");
	m_edtFile.SetWindowText(L"C:\\test2.pptx");
	m_spnFps.SetBuddy(&m_edtFps);
	m_spnFps.SetRange(1,60);
	m_edtFps.SetWindowText(L"30");
	m_chkShowStat.SetCheck(true);

	m_btnPlay.EnableWindow(TRUE);
	m_btnPause.EnableWindow(FALSE);
	m_btnStop.EnableWindow(FALSE);
	m_edtWidth.EnableWindow(TRUE);
	m_edtHeight.EnableWindow(TRUE);
	m_btnSet.EnableWindow(TRUE);
	m_edtFile.EnableWindow(TRUE);
	m_btnBrowse.EnableWindow(TRUE);
	m_spnFps.EnableWindow(TRUE);
	m_chkShowStat.EnableWindow(TRUE);

	InitPlayer();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSampleDlg::InitPlayer()
{
	CString filename;
	GetDlgItemText(IDC_FILE_EDIT, filename);
	m_player.SetFile(filename.GetBuffer());
	m_player.SetFps(GetDlgItemInt(IDC_FPS_EDIT)); 
	m_player.ShowStat( m_chkShowStat.GetCheck()); 
	m_player.ChangeResolution(GetDlgItemInt(IDC_WIDTH_EDIT)
		, GetDlgItemInt(IDC_HEIGHT_EDIT));
	m_player.Init(m_playerWnd.m_hWnd);
}

void CSampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSampleDlg::OnPaint()
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
		m_player.RepaintVW();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HBRUSH CSampleDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}


void CSampleDlg::OnBnClickedPlayBtn()
{
	//InitPlayer();
	m_player.Play();
	m_btnPlay.EnableWindow(FALSE);
	m_btnPause.EnableWindow(TRUE);
	m_btnStop.EnableWindow(TRUE);
	m_edtWidth.EnableWindow(FALSE);
	m_edtHeight.EnableWindow(FALSE);
	m_btnSet.EnableWindow(FALSE);
	m_edtFile.EnableWindow(FALSE);
	m_btnBrowse.EnableWindow(FALSE);
	m_spnFps.EnableWindow(FALSE);
	m_chkShowStat.EnableWindow(FALSE);

}


void CSampleDlg::OnBnClickedPauseBtn()
{
	m_player.Pause();
	m_btnPlay.EnableWindow(TRUE);
	m_btnPause.EnableWindow(FALSE);
	m_btnStop.EnableWindow(TRUE);
	m_edtWidth.EnableWindow(FALSE);
	m_edtHeight.EnableWindow(FALSE);
	m_btnSet.EnableWindow(FALSE);
	m_edtFile.EnableWindow(FALSE);
	m_btnBrowse.EnableWindow(FALSE);
	m_spnFps.EnableWindow(FALSE);
	m_chkShowStat.EnableWindow(FALSE);
}



void CSampleDlg::OnBnClickedStopBtn()
{
	m_player.Stop();
	m_btnPlay.EnableWindow(TRUE);
	m_btnPause.EnableWindow(FALSE);
	m_btnStop.EnableWindow(FALSE);
	m_edtWidth.EnableWindow(TRUE);
	m_edtHeight.EnableWindow(TRUE);
	m_btnSet.EnableWindow(TRUE);
	m_edtFile.EnableWindow(TRUE);
	m_btnBrowse.EnableWindow(TRUE);
	m_spnFps.EnableWindow(TRUE);
	m_chkShowStat.EnableWindow(TRUE);
}


void CSampleDlg::OnBnClickedSetBtn()
{
	InitPlayer();
}

void CSampleDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	MoveControls();	
}


void CSampleDlg::MoveControls()
{
	CRect r;
	GetClientRect(&r);

	if (m_playerWnd.m_hWnd)
		m_playerWnd.MoveWindow(r.left+10, r.top+10, r.right-20, r.bottom-70);
	m_player.UpdateVWPos();

	CRect r1;
	if (m_btnPlay.m_hWnd != NULL)
	{
		m_btnPlay.GetClientRect(&r1);
		r1.MoveToY(r.bottom-51);
		r1.MoveToX(11);
		m_btnPlay.MoveWindow(&r1);
	}

	if (m_btnPause.m_hWnd != NULL)
	{
		m_btnPause.GetClientRect(&r1);
		r1.MoveToY(r.bottom-51);
		r1.MoveToX(92);
		m_btnPause.MoveWindow(&r1);
	}

	if (m_btnStop.m_hWnd != NULL)
	{
		m_btnStop.GetClientRect(&r1);
		r1.MoveToY(r.bottom-51);
		r1.MoveToX(173);
		m_btnStop.MoveWindow(&r1);
	}

	if (m_stcWidth.m_hWnd != NULL)
	{
		m_stcWidth.GetClientRect(&r1);
		r1.MoveToY(r.bottom-46);
		r1.MoveToX(258);
		m_stcWidth.MoveWindow(&r1);
	}

	if (m_edtWidth.m_hWnd != NULL)
	{
		m_edtWidth.GetClientRect(&r1);
		r1.SetRect(296,r.bottom - 49, 356, r.bottom-28);
		m_edtWidth.MoveWindow(&r1);
	}

	if (m_stcHeight.m_hWnd != NULL)
	{
		m_stcHeight.GetClientRect(&r1);
		r1.MoveToY(r.bottom-46);
		r1.MoveToX(365);
		m_stcHeight.MoveWindow(&r1);
	}

	if (m_edtHeight.m_hWnd != NULL)
	{
		m_edtHeight.GetClientRect(&r1);
		r1.SetRect(404,r.bottom - 49, 464, r.bottom-28);
		m_edtHeight.MoveWindow(&r1);
	}

	if (m_btnSet.m_hWnd != NULL)
	{
		m_btnSet.GetClientRect(&r1);
		r1.MoveToY(r.bottom-51);
		r1.MoveToX(479);
		m_btnSet.MoveWindow(&r1);
	}

	if ( m_stcPptx.m_hWnd)
	{
		m_stcPptx.GetClientRect(&r1);
		r1.SetRect(12,r.bottom - 20, 100, r.bottom-5);
		m_stcPptx.MoveWindow(&r1);
	}

	if (m_edtFile.m_hWnd)
	{
		m_edtFile.GetClientRect(&r1);
		r1.SetRect(60,r.bottom - 23, 248, r.bottom-3);
		m_edtFile.MoveWindow(&r1);
	}	

	if (m_btnBrowse.m_hWnd)
	{
		m_btnBrowse.GetClientRect(&r1);
		r1.SetRect(248,r.bottom - 23, 274, r.bottom-3);
		m_btnBrowse.MoveWindow(&r1);
	}

	if (m_stcFps.m_hWnd)
	{
		m_stcFps.GetClientRect(&r1);
		r1.SetRect(297,r.bottom - 20, 319, r.bottom-3);
		m_stcFps.MoveWindow(&r1);
	}

	if (m_edtFps.m_hWnd)
	{
		m_edtFps.GetClientRect(&r1);
		r1.SetRect(323,r.bottom - 23, 366, r.bottom-4);
		m_edtFps.MoveWindow(&r1);
	}

	if (m_spnFps.m_hWnd)
	{
		m_spnFps.GetClientRect(&r1);
		r1.SetRect(365,r.bottom - 23, 383, r.bottom-3);
		m_spnFps.MoveWindow(&r1);
	}

	if (m_chkShowStat.m_hWnd != NULL)
	{
		m_chkShowStat.GetClientRect(&r1);
		r1.SetRect(404,r.bottom - 24, 564, r.bottom-5);
		m_chkShowStat.MoveWindow(&r1);
	}

	RedrawWindow();
}

void CSampleDlg::Finalize()
{
	m_player.Reset();
}

void CSampleDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);
	m_player.UpdateVWPos();
}

void CSampleDlg::OnBnClickedBrowseBtn()
{
	//CFile f;

	WCHAR filter[] = { L"Pptx Files (*.pptx)|*.pptx||" };

	CFileDialog dlg(TRUE, L".pptx", NULL, 0, filter);

	if( dlg.DoModal() == IDOK )
	{
		m_edtFile.SetWindowText(dlg.GetFolderPath() +  dlg.GetFileName());
	}
}


void CSampleDlg::OnClose()
{
	m_player.Reset();
	CDialogEx::OnClose();
}


void CSampleDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_player.NextSlide();
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CSampleDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_player.PrevSlide();
	CDialogEx::OnRButtonUp(nFlags, point);
}
