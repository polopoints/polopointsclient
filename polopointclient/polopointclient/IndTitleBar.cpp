// IndTitleBar.cpp : 实现文件
//

#include "stdafx.h"
#include "polopointclient.h"
#include "IndTitleBar.h"
#include "afxdialogex.h"


// CIndTitleBar 对话框

IMPLEMENT_DYNAMIC(CIndTitleBar, CDialogBar)


CIndTitleBar::CIndTitleBar()
{
	m_pBmp = NULL ;
	m_nButtonID = CMainDlg::IDD ;
}

CIndTitleBar::~CIndTitleBar()
{
	if( NULL != m_pBmp ) {
		DeleteObject(m_pBmp) ;
		m_pBmp = NULL ;
	}
}

void CIndTitleBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_BUTTON_LOGO    , m_sBtnLogo );
	//DDX_Control(pDX, IDC_BUTTON_FONT    , m_sBtnFont );
	DDX_Control(pDX, IDC_BUTTON_MAIN	, m_rBtnMain );
	DDX_Control(pDX, IDC_BUTTON_SEND	, m_rBtnSend );
	DDX_Control(pDX, IDC_BUTTON_RECE	, m_rBtnRece );
	DDX_Control(pDX, IDC_BUTTON_DEAL	, m_rBtnDeal );
	DDX_Control(pDX, IDC_BUTTON_APP	    , m_rBtnApp );
	DDX_Control(pDX, IDC_BUTTON_SET	    , m_rBtnSet );
	DDX_Control(pDX, IDC_BUTTON_CLOSE	, m_rBtnClose );
	DDX_Control(pDX, IDC_BUTTON_MIN	    , m_rBtnMin );
}


BEGIN_MESSAGE_MAP(CIndTitleBar, CDialogBar)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(MSG_USER_MOBILE_TIP, &CIndTitleBar::OnShowStateTip )
	//ON_BN_CLICKED(IDC_BUTTON_MIN, &CIndTitleBar::OnBnClickedButtonMin)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CIndTitleBar 消息处理程序
void CIndTitleBar::SetBkBmpNid( UINT nBitmapIn ) 
{
	if( NULL != m_pBmp ) {
		::DeleteObject( m_pBmp ) ;
		m_pBmp = NULL ;
	}
	m_pBmp	= NULL ;
	HINSTANCE	hInstResource = NULL;	
	hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(nBitmapIn), RT_BITMAP);
	if( NULL != hInstResource ) {
		m_pBmp = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(nBitmapIn), IMAGE_BITMAP, 0, 0, 0);
	}
}

BOOL CIndTitleBar::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect   rect; 
	GetClientRect(&rect); 

	if(m_pBmp   !=   NULL) { 
		BITMAP   bm; 
		CDC   dcMem; 
		::GetObject(m_pBmp,sizeof(BITMAP),   (LPVOID)&bm); 
		dcMem.CreateCompatibleDC(NULL); 
		HBITMAP     pOldBitmap   =(HBITMAP   )   dcMem.SelectObject(m_pBmp); 
		pDC-> StretchBlt(rect.left,rect.top-1,rect.Width(),rect.Height(),   &dcMem,   0,   0,bm.bmWidth-1,bm.bmHeight-1,   SRCCOPY); 

		dcMem.SelectObject(pOldBitmap);
		dcMem.DeleteDC();
	} else  
		CWnd::OnEraseBkgnd(pDC); 

	return 1;
}


int CIndTitleBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	SetBkBmpNid( IDB_BITMAP_TITLE_BAR ) ;

	
	return 0;
}


void CIndTitleBar::OnSize(UINT nType, int cx, int cy)
{
	CDialogBar::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if( NULL != GetSafeHwnd() ) {
		CRect rect ; 
		this->GetWindowRect(rect);
		//CStatic *p_Logo = (CStatic*)GetDlgItem(IDC_STATIC_LOGO);
		//if ( NULL != p_Logo ) {
		//	p_Logo->SetWindowPos( NULL ,0 , 0 , 80, 30  ,SWP_SHOWWINDOW ) ; 
		//}

		//CStatic *p_LogoFont = (CStatic*)GetDlgItem(IDC_STATIC_FONT);
		//if ( NULL != p_Logo ) {
		//	p_Logo->SetWindowPos( NULL ,0 , 0 , 126, 30  ,SWP_SHOWWINDOW ) ; 
		//}
		//CButton *p_ButtonLogo = (CButton*)GetDlgItem(IDC_BUTTON_LOGO);
		//if( NULL != p_ButtonLogo ) {
		//	p_ButtonLogo->SetWindowPos( NULL ,0 , 0 , 130, 71  ,SWP_SHOWWINDOW ) ; 
		//}
		//
		//CButton *p_ButtonFont = (CButton*)GetDlgItem(IDC_BUTTON_FONT);
		//if( NULL != p_ButtonFont ) {
		//	p_ButtonFont->SetWindowPos( NULL ,130 , -1 , 80, 72  ,SWP_SHOWWINDOW ) ; 
		//}

		CButton *p_Button = (CButton*)GetDlgItem(IDC_BUTTON_MAIN);
		if( NULL != p_Button ) {
			p_Button->SetWindowPos( NULL ,259 , 20 , 60, 60  ,SWP_SHOWWINDOW ) ; 
		}
		p_Button = (CButton*)GetDlgItem(IDC_BUTTON_SEND);
		if( NULL != p_Button ) {
			p_Button->SetWindowPos( NULL ,349 , 20 , 60, 60  ,SWP_SHOWWINDOW ) ; 
		}
		p_Button = (CButton*)GetDlgItem(IDC_BUTTON_RECE);
		if( NULL != p_Button ) {
			p_Button->SetWindowPos( NULL ,439 , 20 , 60, 60  ,SWP_SHOWWINDOW ) ; 
		}
		p_Button = (CButton*)GetDlgItem(IDC_BUTTON_DEAL);
		if( NULL != p_Button ) {
			p_Button->SetWindowPos( NULL ,529 , 20 , 60, 60  ,SWP_SHOWWINDOW ) ; 
		}
		p_Button = (CButton*)GetDlgItem(IDC_BUTTON_APP);
		if( NULL != p_Button ) {
			p_Button->SetWindowPos( NULL ,619 , 20 , 60, 60  ,SWP_SHOWWINDOW ) ; 
		}
		p_Button = (CButton*)GetDlgItem(IDC_BUTTON_SET);
		if( NULL != p_Button ) {
			p_Button->SetWindowPos( NULL ,709 , 20 , 60, 60  ,SWP_SHOWWINDOW ) ; 
		}

		CButton *p_Close = (CButton*)GetDlgItem(IDC_BUTTON_CLOSE);
		if( NULL != p_Close ) {	
			p_Close->SetWindowPos(NULL ,900 - 24 - 10 + 5, 0 ,18 , 18 , SWP_SHOWWINDOW);
		}
		CButton * p_Min = (CButton*)GetDlgItem(IDC_BUTTON_MIN);
		if( NULL != p_Min ) {	
			p_Min->SetWindowPos(NULL ,900 - 48 - 10 - 5 + 5, 0 , 18 , 18 , SWP_SHOWWINDOW);
		}
	}
}


BOOL CIndTitleBar::Create(CWnd* pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID)
{
	// TODO: 在此添加专用代码和/或调用基类

	BOOL bRes =  CDialogBar::Create(pParentWnd, nIDTemplate, nStyle, nID);
	if ( bRes ) {
		
		//m_rBtnMain.LoadBitmaps(IDB_BITMAP_MAIN1 ,IDB_BITMAP_MAIN3 , IDB_BITMAP_MAIN2 , IDB_BITMAP_MAIN1 );
		//m_rBtnSend.LoadBitmaps(IDB_BITMAP_SEND1 ,IDB_BITMAP_SEND3 , IDB_BITMAP_SEND2 , IDB_BITMAP_SEND1 );
		//m_rBtnRece.LoadBitmaps(IDB_BITMAP_REV1 ,IDB_BITMAP_REV3 , IDB_BITMAP_REV2 , IDB_BITMAP_REV1 );
		//m_rBtnDeal.LoadBitmaps(IDB_BITMAP_TRAD1 ,IDB_BITMAP_TRAD3 , IDB_BITMAP_TRAD2 , IDB_BITMAP_TRAD1 );
		//m_rBtnApp.LoadBitmaps(IDB_BITMAP_IPO1 ,IDB_BITMAP_IPO3 , IDB_BITMAP_IPO2 , IDB_BITMAP_IPO1 );
		//m_rBtnSet.LoadBitmaps(IDB_BITMAP_SET1 ,IDB_BITMAP_SET3 , IDB_BITMAP_SET2 , IDB_BITMAP_SET1 );

		//m_rBtnClose.LoadBitmaps(IDB_BITMAP_CLOSE1 , IDB_BITMAP_CLOSE2 ,IDB_BITMAP_CLOSE3,IDB_BITMAP_CLOSE1);
		//m_rBtnMin.LoadBitmaps(IDB_BITMAP_MIN1 , IDB_BITMAP_MIN2  ,IDB_BITMAP_MIN3,IDB_BITMAP_MIN1);
		UpdateData(FALSE);

		m_rBtnClose.SetIcon(IDI_ICON_CLOSE3, 0, 0, IDI_ICON_CLOSE1, 0, 0);
		m_rBtnClose.DrawBorder(FALSE); 
		m_rBtnClose.DrawTransparent(TRUE);

		m_rBtnMin.SetIcon(IDI_ICON_MIN3, 0, 0, IDI_ICON_MIN1, 0, 0);
		m_rBtnMin.DrawBorder(FALSE); 
		m_rBtnMin.DrawTransparent(TRUE);

		theApp.SubscribeMsg( theApp.GetMtHthrdId() , GetSafeHwnd() , MSG_USER_MOBILE_TIP ) ; 
		MobileTip(CMainDlg::IDD , TRUE );
	}
	return bRes ;
}
LRESULT CIndTitleBar::OnShowStateTip( WPARAM wParam, LPARAM lParam )
{
	MobileTip( (UINT)wParam , (BOOL)lParam );
	return 1;
}
void CIndTitleBar::MobileTip(UINT nButtonID , BOOL bFlag )
{
	if ( bFlag ) { 
		HCURSOR cur = ::LoadCursor(NULL, IDC_HAND);
		m_rBtnMain.SetIcon(IDI_MAIN_LOGO_ICON2, 0, 0, IDI_MAIN_LOGO_ICON1, 0, 0);
		m_rBtnMain.DrawBorder(FALSE); 
		m_rBtnMain.DrawTransparent(TRUE);
		//m_rBtnMain.SetBtnCursor(IDC_CURSOR1);
		m_rBtnMain.SetCursor(cur);
		
		m_rBtnSend.SetIcon(IDI_SEND_LOGO_ICON2, 0, 0, IDI_SEND_LOGO_ICON1, 0, 0);
		m_rBtnSend.DrawBorder(FALSE); 
		m_rBtnSend.DrawTransparent(TRUE);
		//m_rBtnSend.SetBtnCursor(IDC_CURSOR1);
		m_rBtnSend.SetCursor(cur);
		
		m_rBtnRece.SetIcon(IDI_RECV_LOGO_ICON2, 0, 0, IDI_RECV_LOGO_ICON1, 0, 0);
		m_rBtnRece.DrawBorder(FALSE); 
		m_rBtnRece.DrawTransparent(TRUE);
		//m_rBtnRece.SetBtnCursor(IDC_CURSOR1);
		m_rBtnRece.SetCursor(cur);
		
		m_rBtnDeal.SetIcon(IDI_TXDETAIL_ICON2, 0, 0, IDI_TXDETAIL_ICON1, 0, 0);
		m_rBtnDeal.DrawBorder(FALSE); 
		m_rBtnDeal.DrawTransparent(TRUE);
		//m_rBtnDeal.SetBtnCursor(IDC_CURSOR1);
		m_rBtnDeal.SetCursor(cur);
		
		m_rBtnApp.SetIcon(IDI_YUE_ICON2, 0, 0, IDI_YUE_ICON1, 0, 0);
		m_rBtnApp.DrawBorder(FALSE); 
		m_rBtnApp.DrawTransparent(TRUE);
		//m_rBtnApp.SetBtnCursor(IDC_CURSOR1);
		m_rBtnApp.SetCursor(cur);
		
		m_rBtnSet.SetIcon(IDI_SET_ICON2, 0, 0, IDI_SET_ICON1, 0, 0);
		m_rBtnSet.DrawBorder(FALSE); 
		m_rBtnSet.DrawTransparent(TRUE);
		//m_rBtnSet.SetBtnCursor(IDC_CURSOR1);
		m_rBtnSet.SetCursor(cur);

//		m_rBtnMain.LoadBitmaps(IDB_BITMAP_MAIN3 ,IDB_BITMAP_MAIN2 , IDB_BITMAP_MAIN3 , IDB_BITMAP_MAIN1 );
	}
	if ( m_nButtonID == nButtonID  && !bFlag ) {
		return ;
	}
	//m_rBtnMain.SetIcon(IDI_MAIN_LOGO_ICON2);
	InitBtStatus(m_nButtonID);
	switch(nButtonID)
	{
	case CMainDlg::IDD:
		m_rBtnMain.SetIcon(IDI_MAIN_LOGO_ICON2);
		 break;
	case CSend::IDD:
		m_rBtnSend.SetIcon(IDI_SEND_LOGO_ICON2);
		break;
	case CRece::IDD:
		m_rBtnRece.SetIcon(IDI_RECV_LOGO_ICON2);
		break;
	case CDeal::IDD:
		m_rBtnDeal.SetIcon(IDI_TXDETAIL_ICON2);
		break;
	case CIpo::IDD:
		m_rBtnApp.SetIcon(IDI_YUE_ICON2);
		break;
	case CSet::IDD:	
		m_rBtnSet.SetIcon(IDI_SET_ICON2);
		break;
	}
	m_nButtonID = nButtonID ;

	this->Invalidate();
}

void CIndTitleBar::InitBtStatus(UINT nButtonID) {
	switch(nButtonID) 
	{
		case CMainDlg::IDD:
			m_rBtnMain.SetIcon(IDI_MAIN_LOGO_ICON2, IDI_MAIN_LOGO_ICON1);
			break;
		case CSend::IDD:
			m_rBtnSend.SetIcon(IDI_SEND_LOGO_ICON2, IDI_SEND_LOGO_ICON1);
			break;
		case CRece::IDD:
			m_rBtnRece.SetIcon(IDI_RECV_LOGO_ICON2, IDI_RECV_LOGO_ICON1);
			break;
		case CDeal::IDD:
			m_rBtnDeal.SetIcon(IDI_TXDETAIL_ICON2, IDI_TXDETAIL_ICON1);
			break;
		case CIpo::IDD:
			m_rBtnApp.SetIcon(IDI_YUE_ICON2, IDI_YUE_ICON1);
			break;
		case CSet::IDD:
			m_rBtnSet.SetIcon(IDI_SET_ICON2, IDI_SET_ICON1);
			break;
	}
}

/*
void CIndTitleBar::OnBnClickedButtonMin()
{
	// TODO: 在此添加控件通知处理程序代码
	GetParent()->SendMessage(WM_SYSCOMMAND,SC_MINIMIZE,   0); 
}
*/

void CIndTitleBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	GetParent()->PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x, point.y));
	CDialogBar::OnLButtonDown(nFlags, point);
}


BOOL CIndTitleBar::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message == WM_KEYDOWN) 
	{  

		switch(pMsg->wParam) 
		{  

		case VK_RETURN: //回车   
			return TRUE;  

		case VK_ESCAPE: //ESC  
			return TRUE;  

		}  

	}  
	return CDialogBar::PreTranslateMessage(pMsg);
}
