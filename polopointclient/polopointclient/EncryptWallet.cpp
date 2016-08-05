// EncryptWallet.cpp : 实现文件
//

#include "stdafx.h"
#include "polopointclient.h"
#include "EncryptWallet.h"
#include "afxdialogex.h"
#include "polopointclientDlg.h"

// CEncryptWallet 对话框

IMPLEMENT_DYNAMIC(CEncryptWallet, CDialogBase)

CEncryptWallet::CEncryptWallet(CWnd* pParent /*=NULL*/)
	: CDialogBase(CEncryptWallet::IDD, pParent)
{

}

CEncryptWallet::~CEncryptWallet()
{
}

void CEncryptWallet::DoDataExchange(CDataExchange* pDX)
{
	CDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_INPUTMIMA, m_password);
	DDX_Control(pDX, IDC_STATIC_CONFIREDMIMA, m_confiredpassword);	
	DDX_Control(pDX, IDC_HEAD, m_headText);	
	DDX_Control(pDX, IDC_CLOSE, m_rBtnClose);
	DDX_Control(pDX, IDCANCEL, m_rBtnCancel);
	DDX_Control(pDX, IDOK, m_rBtnOk);
}


BEGIN_MESSAGE_MAP(CEncryptWallet, CDialogBase)
	ON_BN_CLICKED(IDOK, &CEncryptWallet::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CLOSE, &CEncryptWallet::OnBnClickedClose)
END_MESSAGE_MAP()


// CEncryptWallet 消息处理程序
BOOL CEncryptWallet::OnInitDialog()
{
	CDialogBase::OnInitDialog();

	m_headText.SetFont(100, _T("微软雅黑"));
	m_headText.SetTextColor(RGB(255,255,255));	
	m_headText.SetWindowText(_T("钱包加密"));
	m_headText.SetWindowPos( NULL , 5 , 4 , 100, 25  ,SWP_SHOWWINDOW ) ; 

	//m_rBtnClose.SetBitmaps( IDB_BITMAP_CLOSE , RGB(255, 255, 0) , IDB_BITMAP_CLOSE2 , RGB(255, 255, 255) );
	
	m_rBtnClose.SetBitmaps( IDB_BITMAP_CLOSE2 , RGB(0, 0, 0) , IDB_BITMAP_CLOSE1 , RGB(0, 0, 0) );
	m_rBtnClose.SetAlign(CButtonST::ST_ALIGN_OVERLAP);
	m_rBtnClose.SetWindowText("") ;
	m_rBtnClose.SetFontEx(-12 , _T("微软雅黑"));
	m_rBtnClose.SetColor(CButtonST::BTNST_COLOR_FG_OUT , RGB(0, 0, 0));
	m_rBtnClose.SetColor(CButtonST::BTNST_COLOR_FG_IN , RGB(200, 75, 60));
	m_rBtnClose.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0, 0, 0));
	m_rBtnClose.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(0, 0, 0));
	m_rBtnClose.SizeToContent();
	
	/*
	m_rBtnClose.SetIcon(IDI_ICON_CLOSE3, 0, 0, IDI_ICON_CLOSE1, 0, 0);
	m_rBtnClose.DrawBorder(FALSE); 
	m_rBtnClose.DrawTransparent(TRUE);
	m_rBtnClose.SizeToContent();
	*/
	CRect rect ;
	m_rBtnClose.GetClientRect(rect);

	RECT ret;
	GetWindowRect(&ret);
	m_rBtnClose.SetWindowPos(NULL ,(ret.right-ret.left)-rect.Width() - 4 , 2 , 0 , 0 , SWP_NOSIZE); 

	m_rBtnOk.SetBitmaps( IDB_BITMAP_BUT2 , RGB(255, 255, 0) , IDB_BITMAP_BUT1 , RGB(255, 255, 255) );
	m_rBtnOk.SetAlign(CButtonST::ST_ALIGN_OVERLAP);
	m_rBtnOk.SetWindowText(_T("确认")) ;
//	m_rBtnOk.SetFontEx(20 , _T("微软雅黑"));
	/*
	m_rBtnOk.SetColor(CButtonST::BTNST_COLOR_FG_OUT , RGB(0, 0, 0));
	m_rBtnOk.SetColor(CButtonST::BTNST_COLOR_FG_IN , RGB(200, 75, 60));
	m_rBtnOk.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0, 0, 0));
	m_rBtnOk.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(0, 0, 0));
	*/
	m_rBtnOk.SetColor(CButtonST::BTNST_COLOR_FG_OUT , RGB(255, 255, 255));
	m_rBtnOk.SetColor(CButtonST::BTNST_COLOR_FG_IN , RGB(200, 75, 60));
	m_rBtnOk.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(255, 255, 255));
	m_rBtnOk.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(255, 255, 255));
	m_rBtnOk.SizeToContent();

	m_rBtnCancel.SetBitmaps( IDB_BITMAP_ORANGE_BUT2 , RGB(255, 255, 0) , IDB_BITMAP_ORANGE_BUT1 , RGB(255, 255, 255) );
	m_rBtnCancel.SetAlign(CButtonST::ST_ALIGN_OVERLAP);
	m_rBtnCancel.SetWindowText(_T("取消")) ;
	//m_rBtnCancel.SetFontEx(20 , _T("微软雅黑"));
	/*
	m_rBtnCancel.SetColor(CButtonST::BTNST_COLOR_FG_OUT , RGB(0, 0, 0));
	m_rBtnCancel.SetColor(CButtonST::BTNST_COLOR_FG_IN , RGB(200, 75, 60));
	m_rBtnCancel.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(0, 0, 0));
	m_rBtnCancel.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(0, 0, 0));
	*/
	m_rBtnCancel.SetColor(CButtonST::BTNST_COLOR_FG_OUT , RGB(255, 255, 255));
	m_rBtnCancel.SetColor(CButtonST::BTNST_COLOR_FG_IN , RGB(200, 75, 60));
	m_rBtnCancel.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(255, 255, 255));
	m_rBtnCancel.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(255, 255, 255));
	m_rBtnCancel.SizeToContent();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CEncryptWallet::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString inputE = _T("");
	CString confiedE = _T("");
	GetDlgItem(IDC_INPUTMIMA)->GetWindowText(inputE);
	GetDlgItem(IDC_CONFIREDMIMA)->GetWindowText(confiedE);

	int pos = inputE.Find(_T(" "));
	if (pos >=0)
	{
		UiFun::MessageBoxEx(_T("输入密码中不能有空格") , _T("提示"),MFB_OK|MFB_TIP );
		return;
	}
	if (inputE == _T(""))
	{
		UiFun::MessageBoxEx(_T("输入密码"),_T("提示"),MFB_OK|MFB_TIP );
		return;
	}
	if (confiedE == _T(""))
	{
		UiFun::MessageBoxEx(_T("请输入确认密码") ,_T("提示"),MFB_OK|MFB_TIP );
		return;
	}
	if (inputE.GetLength()<6)
	{
		UiFun::MessageBoxEx(_T("密码个数最少六位") ,_T("提示"),MFB_OK|MFB_TIP );
		return;
	}
	if (IsAllDigtal(inputE))
	{
		UiFun::MessageBoxEx(_T("密码不能全是数字") , _T("提示"),MFB_OK|MFB_TIP );
		return;
	}
	if (strcmp(inputE,confiedE) != 0)
	{
		UiFun::MessageBoxEx(_T("确认密码和输入密码不一致") ,_T("提示"),MFB_OK|MFB_TIP );
		return;
	}

	string strCommand;
	strCommand = strprintf("%s %s",_T("encryptwallet"),inputE);
	string strShowData ="";
	Json::Value root;
	if(!CSoyPayHelp::getInstance()->SendRpc(strCommand,root))
	{
		TRACE(" rpccmd encryptwallet error");
		UiFun::MessageBoxEx(_T("钱包加密失败，请重新加密") , _T("提示") ,MFB_OK|MFB_TIP );
		return;
	}

		
	bool isClose = FALSE; 
	bool isEntryp = root["encrypt"].asBool();
	isClose = isEntryp;
	if (isEntryp)
	{
		UiFun::MessageBoxEx(_T("钱包加密成功请重新启动钱包"),_T("提示"),MFB_OK|MFB_TIP );
	}else
	{
		UiFun::MessageBoxEx(_T("钱包加密失败") , _T("提示") ,MFB_OK|MFB_TIP );
	}

	CDialogBase::OnOK();
	if (isClose)
	{
		((CpolopointclientDlg*)(theApp.m_pMainWnd))->ClosWalletWind();
	}
}


void CEncryptWallet::OnBnClickedClose()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogBase::OnCancel();
}
