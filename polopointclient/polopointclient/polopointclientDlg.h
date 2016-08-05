
// polopointclientDlg.h : 头文件
//

#pragma once

#include "IndTitleBar.h"
#include "ProgStatusBar.h"
#include "MainDlg.h"
#include "Send.h"
#include "Rece.h"
#include "Deal.h"
#include "Ipo.h"
#include "Set.h"
#include "BalloonTip.h"
#include "RPCDlg.h"
#include "OutGifDlg.h"

// ChuobiDlg 对话框
class CpolopointclientDlg : public CDialogEx
{
// 构造
public:
	CpolopointclientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_POLOPOINTCLIENT_DIALOG };

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
	CIndTitleBar		*m_pTitleBar;//*m_pIndToolbar;
	CProgStatusBar      *m_pStatusBar;//状态栏

	CMainDlg            *m_pMainDlg;
	CSend               *m_pSendDlg;
	CRece               *m_pReceDlg;
	CDeal               *m_pDealDlg;
	CIpo                *m_pIpoDlg;
	CSet                *m_pSetDlg;
	CRPCDlg             *m_pRPCDlg;
	COutGifDlg          *m_pOutGifDlg;
public:
	std::map< UINT , CDialogBar * >  m_dlgMap ; 
	void       ShowDialog(UINT dlgid)  ;
	void       DestroyDlg();
	void       ClosWallet();
	void       CloseThread();
	void       StopSever();
	void       CloseApp();
	void       DeleteTray();
	CDialogBar *p_CurSelDlg   ;   //当前选择对话框指针
	int        dlgType;
public:
	static void  BlockDataProc(char *pRecBuffer, int nRecLen ,WPARAM wParam);
public:
	afx_msg void OnBnClickedButtonMain();
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnBnClickedButtonRece();
	afx_msg void OnBnClickedButtonDeal();
	afx_msg void OnBnClickedButtonIpo();
	afx_msg void OnBnClickedButtonSet();

	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonMin();
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnShowTask(WPARAM wParam,LPARAM lParam) ;
	afx_msg LRESULT OnPopupBar(WPARAM wParam,LPARAM lParam) ;
public:
	void InitialRpcCmd();      //初始化RPC调用命令
	void LoadWalletAddress();  //加载钱包地址 
	bool GetFileName(CString &fileName,CString strexe );
	void  ClosWalletWind();
	void OnCloseWriteAppFee();
	void ToTray();
	void ShowStateTip(UINT nButtonID);

	CBalloonTip*  m_pBalloonTip; //冒泡提示
	CMyQueue m_barpoomesg;
	CMenu menu;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
