
// AutoSwipeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AutoSwipe.h"
#include "AutoSwipeDlg.h"
#include "afxdialogex.h"
#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IDT_SLIDE_TIMER  1001

// 热键 ID
#define HOTKEY_START  100
#define HOTKEY_STOP   101

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
public:
	CMFCLinkCtrl m_btnLinkEmail;
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCLINK1, m_btnLinkEmail);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_btnLinkEmail.SetURL(_T("mailto:yubsh@163.com"));
	m_btnLinkEmail.SetTooltip(_T("点击发送邮件"));
	m_btnLinkEmail.SizeToContent();

	return TRUE; 
}

// CAutoSwipeDlg 对话框

CAutoSwipeDlg::CAutoSwipeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAutoSwipeDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAutoSwipeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_APP, m_cmbApp);
	DDX_Control(pDX, IDC_COMBO_CONTROL, m_cmbControl);
	DDX_Control(pDX, IDC_COMBO_TIME_INTRERVAL, m_cmbTimeInterval);
	DDX_Control(pDX, IDC_EDIT_POS_RANGE, m_editPosRange);
	DDX_Control(pDX, IDC_BUTTON_START, m_btnStart);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_staStatus);
	DDX_Control(pDX, IDC_EDIT_TARGET_COUNT, m_editTargetCount);
	DDX_Control(pDX, IDC_EDIT_INTERVAL_RANGE, m_editSlideDuration);
}

BEGIN_MESSAGE_MAP(CAutoSwipeDlg, CDialogEx)

	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &CAutoSwipeDlg::OnBnClickedButtonStart)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_HOTKEY()
END_MESSAGE_MAP()


// CAutoSwipeDlg 消息处理程序

BOOL CAutoSwipeDlg::OnInitDialog()
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CString strIniFile = GetExeName() + _T(".ini");
	m_ini.SetPathName(strIniFile);

	m_cmbApp.AddString(_T("跨屏协作"));
	CString strTemp = m_ini.GetString(_T("Setup"), _T("TargetWindow"), _T("跨屏协作"));
	m_cmbApp.SelectString(0, strTemp);

	m_cmbControl.AddString(_T("上滑"));
	m_cmbControl.AddString(_T("下滑"));
	m_cmbControl.AddString(_T("上8~10, 下1"));
	int nSel = m_ini.GetInt(_T("Setup"), _T("Direction"), 0);
	m_cmbControl.SetCurSel(nSel);

	m_cmbTimeInterval.AddString(_T("1"));		// s
	m_cmbTimeInterval.AddString(_T("3"));
	m_cmbTimeInterval.AddString(_T("5"));
	m_cmbTimeInterval.AddString(_T("10"));
	m_cmbTimeInterval.AddString(_T("20"));
	m_cmbTimeInterval.AddString(_T("60"));
	strTemp = m_ini.GetString(_T("Setup"), _T("IntervalBase"), _T("3"));
	m_cmbTimeInterval.SelectString(0, strTemp);

	// 滑动时间范围
	strTemp = m_ini.GetString(_T("Setup"), _T("SlideDuration"), _T("1"));
	m_editSlideDuration.SetWindowText(strTemp);	// s

	// x/y方向上的偏移像素值
	strTemp = m_ini.GetString(_T("Setup"), _T("PosRange"), _T("100"));
	m_editPosRange.SetWindowText(strTemp);	// pt
	m_nOffsetX = m_nOffsetY = 100;

	strTemp = m_ini.GetString(_T("Setup"), _T("TargetCount"), _T("100"));
	m_editTargetCount.SetWindowText(strTemp);

	m_bRunning       = FALSE;
	m_nCurrentCount  = 0;

	// 注册全局热键：F9 开始，F10 停止
	::RegisterHotKey(GetSafeHwnd(), HOTKEY_START, 0, VK_F9);
	::RegisterHotKey(GetSafeHwnd(), HOTKEY_STOP,  0, VK_F10);

	return TRUE; 
}

void CAutoSwipeDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAutoSwipeDlg::OnPaint()
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
HCURSOR CAutoSwipeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 静态回调函数
BOOL CALLBACK CAutoSwipeDlg::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	CAutoSwipeDlg* pThis = (CAutoSwipeDlg*)lParam;
	if (pThis == NULL) return TRUE;

	TCHAR szTitle[512] = { 0 };
	::GetWindowText(hWnd, szTitle, 512);

	// 用成员变量 m_strSearchKeyword 做匹配
	if (_tcsstr(szTitle, pThis->m_strSearchKeyword) != NULL
		&& ::IsWindowVisible(hWnd))
	{
		pThis->m_hFoundWnd = hWnd;
		return FALSE;  // 找到就停止枚举
	}
	return TRUE;
}

// 查找目标窗口
BOOL CAutoSwipeDlg::FindTargetWindow(const CString& strKeyword)
{
	m_hFoundWnd = NULL;
	m_strSearchKeyword = strKeyword;

	::EnumWindows(CAutoSwipeDlg::EnumWindowsProc, (LPARAM)this);

	m_hTargetWnd = m_hFoundWnd;
	return (m_hTargetWnd != NULL);
}

// ============================================
// 获取窗口下半部分的随机起点
// ============================================
POINT CAutoSwipeDlg::GetRandomStartPoint(HWND hWnd)
{
	RECT rc;
	::GetClientRect(hWnd, &rc);

	// 窗口宽度和高度
	int nWidth  = rc.right - rc.left;
	int nHeight = rc.bottom - rc.top;

	// 起点：水平居中偏左/右随机，垂直在下半部分（60% ~ 90% 之间）
	int nX = nWidth / 2 + (rand() % 101 - 50);          // 中心 ±50 像素
	int nY = nHeight * 60 / 100 + (rand() % (nHeight * 30 / 100)); // 60%~90%

	POINT pt = { nX, nY };
	::ClientToScreen(hWnd, &pt);
	return pt;
}

// 在指定窗口上执行一次上滑（或下滑）操作
// 参数：
//   hWnd     目标窗口
//   nDirction 0-上滑（内容向上翻），1-下滑，2，上滑偶尔下滑
//   nDuration 滑动总耗时（毫秒）
void CAutoSwipeDlg::DoOneSlide(HWND hWnd, SlideDirection dir, int nDuration)
{
	if (!::IsWindow(hWnd)) return;

	RECT rc;
	::GetClientRect(hWnd, &rc);
	int nWidth  = rc.right - rc.left;
	int nHeight = rc.bottom - rc.top;
	if (nWidth <= 0 || nHeight <= 0) return;

	int nCenterX = nWidth / 2;
	int nStartX, nStartY, nEndX, nEndY;

	// 判断这次实际是上滑还是下滑
	BOOL bUp = TRUE;
	if (dir == SCROLL_UP)
	{
		bUp = TRUE;
	}
	else if (dir == SCROLL_DN)
	{
		bUp = FALSE;
	}
	else // SCROLL_RANDOM
	{
		// 初始化本轮目标
		if (m_nRandomUpTarget == 0 && m_nRandomDnCount == 0)
		{
			m_nRandomUpTarget = 8 + (rand() % 3); // 8~10
			m_nRandomUpCount  = 0;
		}

		if (m_nRandomUpCount < m_nRandomUpTarget)
		{
			bUp = TRUE;
			m_nRandomUpCount++;
		}
		else
		{
			bUp = FALSE;
			m_nRandomDnCount++;

			// 本轮结束，重置，准备下一轮
			if (m_nRandomDnCount >= 1)
			{
				m_nRandomUpCount  = 0;
				m_nRandomUpTarget = 0;
				m_nRandomDnCount  = 0;
			}
		}
	}

	// ---------- 根据方向计算起点和终点 ----------
	if (bUp)
	{
		// 上滑：起点在下半部分，终点在上半部分
		nStartX = nCenterX + (rand() % (m_nOffsetX * 2 + 1)) - m_nOffsetX;
		nStartY = nHeight * 70 / 100 + (rand() % (nHeight * 10 / 100)); // 70%~80%

		nEndX = nStartX + (rand() % (m_nOffsetX * 2 + 1)) - m_nOffsetX;
		nEndY = nHeight * 15 / 100 + (rand() % (nHeight * 10 / 100));   // 15%~25%
	}
	else
	{
		// 下滑：起点在上半部分，终点在下半部分
		nStartX = nCenterX + (rand() % (m_nOffsetX * 2 + 1)) - m_nOffsetX;
		nStartY = nHeight * 25 / 100 + (rand() % (nHeight * 10 / 100)); // 25%~35%

		nEndX = nStartX + (rand() % (m_nOffsetX * 2 + 1)) - m_nOffsetX;
		nEndY = nHeight * 80 / 100 + (rand() % (nHeight * 10 / 100));   // 80%~90%
	}

	// 垂直方向的额外随机偏移
	nStartY += (rand() % (m_nOffsetY * 2 + 1)) - m_nOffsetY;
	nEndY   += (rand() % (m_nOffsetY * 2 + 1)) - m_nOffsetY;

	// 边界保护
	if (nStartY < 0) nStartY = 0;
	if (nStartY > nHeight - 1) nStartY = nHeight - 1;
	if (nEndY < 0) nEndY = 0;
	if (nEndY > nHeight - 1) nEndY = nHeight - 1;

	POINT ptStart = { nStartX, nStartY };
	POINT ptEnd   = { nEndX,   nEndY   };
	::ClientToScreen(hWnd, &ptStart);
	::ClientToScreen(hWnd, &ptEnd);

	// ---------- 执行快速甩动 ----------
	::SetCursorPos(ptStart.x, ptStart.y);
	::Sleep(20);
	::mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);

	const int nSteps = 28;
	double dStepX = (double)(ptEnd.x - ptStart.x) / nSteps;
	double dStepY = (double)(ptEnd.y - ptStart.y) / nSteps;
	double dCurX = ptStart.x;
	double dCurY = ptStart.y;

	int nStepDelay = nDuration / nSteps;
	if (nStepDelay < 1) nStepDelay = 1;

	for (int i = 0; i < nSteps; i++)
	{
		dCurX += dStepX;
		dCurY += dStepY;
		::SetCursorPos((int)dCurX, (int)dCurY);
		::Sleep(nStepDelay);
	}

	::SetCursorPos(ptEnd.x, ptEnd.y);
	::Sleep(5);
	::mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

	// 打印信息
	TRACE(_T("mouse move: (%d, %d) -> (%d, %d)\n"),  ptStart.x,  ptStart.y, ptEnd.x, ptEnd.y);
}

// ============================================
// 更新状态显示
// ============================================
void CAutoSwipeDlg::UpdateStatus()
{
	CString strStatus;
	if (m_bRunning)
	{
		strStatus.Format(_T("运行中：%d / %d 次"), m_nCurrentCount, m_nTargetCount);
	}
	else
	{
		strStatus.Format(_T("已停止：共完成 %d 次"), m_nCurrentCount);
	}
	m_staStatus.SetWindowText(strStatus);
}

void CAutoSwipeDlg::GetInputParameters(BOOL bSave)
{
	// 读取参数
	CString strTemp;
	m_editTargetCount.GetWindowText(strTemp);
	m_nTargetCount = _ttoi(strTemp);
	if (m_nTargetCount <= 0) m_nTargetCount = 100;
	if(bSave)m_ini.WriteString(_T("Setup"), _T("TargetCount"), strTemp);

	// 时间随机加减值
	m_editSlideDuration.GetWindowText(strTemp);
	m_nSlideDuration = _ttoi(strTemp);
	if (m_nSlideDuration <= 0) m_nSlideDuration = 1;
	if(bSave)m_ini.WriteString(_T("Setup"), _T("SlideDuration"), strTemp);

	// 基本时间间隔
	m_cmbTimeInterval.GetWindowText(strTemp);
	m_nIntervalBase = _ttoi(strTemp);
	if (m_nIntervalBase <= 0) m_nIntervalBase = 1;
	if(bSave)m_ini.WriteString(_T("Setup"), _T("IntervalBase"), strTemp);

	m_editPosRange.GetWindowText(strTemp);	// pt
	m_nOffsetX = m_nOffsetY = _ttoi(strTemp);
	if(bSave)m_ini.WriteString(_T("Setup"), _T("PosRange"), strTemp);
	m_nSlideDistance = 0;

	// 滚动方向
	int nSel = m_cmbControl.GetCurSel();
	switch (nSel)
	{
	case 0: m_nDirection = SCROLL_UP;     break;
	case 1: m_nDirection = SCROLL_DN;     break;
	case 2: m_nDirection = SCROLL_RANDOM; break;
	default: m_nDirection = SCROLL_UP;    break;
	}
	if(bSave)m_ini.WriteInt(_T("Setup"), _T("Direction"), m_nDirection);
}

// ============================================
// 开始自动化
// ============================================
void CAutoSwipeDlg::StartAutomation()
{
	CString strKeyword;
	m_cmbApp.GetWindowText(strKeyword);
	m_ini.WriteString(_T("Setup"), _T("TargetWindow"), strKeyword);

	if (!FindTargetWindow(strKeyword))
	{
		AfxMessageBox(_T("未找到目标窗口，请确认窗口已打开！"));
		return;
	}

	GetInputParameters();

	// 将目标窗口和自己的对话框置顶
	SetWindowTopMost(m_hTargetWnd, TRUE);      // "跨屏协作"窗口置顶
	SetWindowTopMost(GetSafeHwnd(), TRUE);     // 自己的对话框置顶

	// 重新计数
	m_nCurrentCount = 0;
	m_bRunning = TRUE;
	m_btnStart.SetWindowText(_T("停止"));

	// 重置 SCROLL_RANDOM 的内部计数器
	m_nRandomUpCount  = 0;
	m_nRandomUpTarget = 0;
	m_nRandomDnCount  = 0;

	// 立即执行一次，然后启动定时器
	DoOneSlide(m_hTargetWnd, (SlideDirection)m_nDirection, m_nSlideDuration);
	m_nCurrentCount++;
	UpdateStatus();

	// 计算下一次间隔：
	// 每次间隔 = m_nIntervalBase ± m_nSlideDuration
	// 即随机范围是 [m_nIntervalBase - m_nSlideDuration, m_nIntervalBase + m_nSlideDuration]
	int nRange = m_nSlideDuration * 1000;
	int nNextInterval = m_nIntervalBase * 1000 + (rand() % (nRange * 2 + 1)) - nRange;

	// 下限保护
	if (nNextInterval < 1000) nNextInterval = 1000;

	// 显示定时器间隔
	TRACE(_T("next timer interval: %.2f s\n"), nNextInterval * 0.001F);

	SetTimer(IDT_SLIDE_TIMER, nNextInterval, NULL);
}

// ============================================
// 停止自动化
// ============================================
void CAutoSwipeDlg::StopAutomation()
{
	KillTimer(IDT_SLIDE_TIMER);

	// 取消置顶
	SetWindowTopMost(m_hTargetWnd, FALSE);
	SetWindowTopMost(GetSafeHwnd(), FALSE);

	m_bRunning = FALSE;
	m_btnStart.SetWindowText(_T("开始"));

	UpdateStatus();
}

// ============================================
// 开始/停止按钮
// ============================================
void CAutoSwipeDlg::OnBnClickedButtonStart()
{
	if (m_bRunning)
	{
		StopAutomation();
	}
	else
	{
		StartAutomation();
	}
}

// ============================================
// 定时器：执行下一次滑动
// ============================================
void CAutoSwipeDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == IDT_SLIDE_TIMER)
	{
		// 先杀掉旧定时器
		KillTimer(IDT_SLIDE_TIMER);

		if (!m_bRunning) return;

		// 检查是否达到目标次数
		if (m_nCurrentCount >= m_nTargetCount)
		{
			StopAutomation();
			AfxMessageBox(_T("已完成设定的滑动次数！"));
			return;
		}

		GetInputParameters();

		// 将目标窗口和自己的对话框置顶
		SetWindowTopMost(m_hTargetWnd, TRUE);      // "跨屏协作"窗口置顶
		SetWindowTopMost(GetSafeHwnd(), TRUE);     // 自己的对话框置顶

		// 执行一次上滑
		DoOneSlide(m_hTargetWnd, (SlideDirection)m_nDirection, m_nSlideDuration);
		m_nCurrentCount++;
		UpdateStatus();

		// 再次检查
		if (m_nCurrentCount >= m_nTargetCount)
		{
			StopAutomation();
			AfxMessageBox(_T("已完成设定的滑动次数！"));
			return;
		}

		// 计算下一次间隔：
		// 每次间隔 = m_nIntervalBase ± m_nSlideDuration
		// 即随机范围是 [m_nIntervalBase - m_nSlideDuration, m_nIntervalBase + m_nSlideDuration]
		int nRange = m_nSlideDuration * 1000;
		int nNextInterval = m_nIntervalBase * 1000 + (rand() % (nRange * 2 + 1)) - nRange;
		if (nNextInterval < 1000) nNextInterval = 1000;

		// 显示定时器间隔
		TRACE(_T("next timer interval: %.2f s\n"), nNextInterval * 0.001F);

		SetTimer(IDT_SLIDE_TIMER, nNextInterval, NULL);
	}

	CDialogEx::OnTimer(nIDEvent);
}

// ============================================
// 销毁时清理
// ============================================
void CAutoSwipeDlg::OnDestroy()
{
	StopAutomation();

	::UnregisterHotKey(GetSafeHwnd(), HOTKEY_START);
	::UnregisterHotKey(GetSafeHwnd(), HOTKEY_STOP);

	// 保存参数
	GetInputParameters(TRUE);

	CDialogEx::OnDestroy();
}


void CAutoSwipeDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	if (nHotKeyId == HOTKEY_START)
	{
		if (!m_bRunning)
			StartAutomation();
	}
	else if (nHotKeyId == HOTKEY_STOP)
	{
		if (m_bRunning)
			StopAutomation();
	}

	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}

// 将窗口置顶
void CAutoSwipeDlg::SetWindowTopMost(HWND hWnd, BOOL bTopMost)
{
	if (!::IsWindow(hWnd)) return;

	::SetWindowPos(hWnd,
		bTopMost ? HWND_TOPMOST : HWND_NOTOPMOST,
		0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

