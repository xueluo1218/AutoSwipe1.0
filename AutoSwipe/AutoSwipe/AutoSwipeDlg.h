
// AutoSwipeDlg.h : 头文件
//

#pragma once

#include "MyIniFile.h"

// CAutoSwipeDlg 对话框
class CAutoSwipeDlg : public CDialogEx
{
// 构造
public:
	CAutoSwipeDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_AUTOSWIPE_DIALOG };

private:
	CIni	m_ini;

	HWND    m_hTargetWnd;          // 目标窗口句柄
	BOOL    m_bRunning;            // 是否正在运行
	int     m_nCurrentCount;       // 当前已滑动次数
	int     m_nTargetCount;        // 目标次数
	int     m_nSlideDuration;      // 单次滑动耗时（秒）
	int     m_nIntervalBase;       // 基础间隔（秒）
	int		m_nDirection;          // 滑动方向

	static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
	CString m_strSearchKeyword;   // 当前搜索的关键字
	HWND    m_hFoundWnd;          // 查找到的窗口句柄

	// 滑动方向枚举
	enum SlideDirection
	{
		SCROLL_UP = 0,		// 上滑
		SCROLL_DN,			// 下滑
		SCROLL_RANDOM		// 上滑8~10次，下滑1次 
	};

	// 内部函数
	BOOL FindTargetWindow(const CString& strKeyword);
	void DoOneSlide(HWND hWnd, SlideDirection dir, int nDuration);
	POINT GetRandomStartPoint(HWND hWnd);
	void UpdateStatus();
	void StartAutomation();
	void StopAutomation();
	void SetWindowTopMost(HWND hWnd, BOOL bTopMost);
	void GetInputParameters(BOOL bSave = FALSE);

	int m_nOffsetX;        // 水平方向随机偏移范围（±像素）
	int m_nOffsetY;        // 垂直方向随机偏移范围（±像素）
	int m_nSlideDistance;  // 滑动距离（像素），0 表示按窗口高度比例计算
	CEdit m_editPosRange;  // 水平、垂直偏移输入框
	//CEdit m_edtSlideDist;  // 滑动距离输入框

	// SCROLL_RANDOM 模式的内部计数器
	int m_nRandomUpCount;    // 本轮已完成的上滑次数
	int m_nRandomUpTarget;   // 本轮上滑的目标次数（8~10 随机）
	int m_nRandomDnCount;    // 本轮已完成的下滑次数（0 或 1）

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CComboBox m_cmbApp;
	CComboBox m_cmbControl;
	CComboBox m_cmbTimeInterval;
	CButton m_btnStart;
	CStatic m_staStatus;
	CEdit m_editTargetCount;
	CEdit m_editSlideDuration;

	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);

};
