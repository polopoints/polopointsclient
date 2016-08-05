// Ipo.cpp : 实现文件
//

#include "stdafx.h"
#include "polopointclient.h"
#include "Ipo.h"
#include "afxdialogex.h"


// CIpo 对话框

IMPLEMENT_DYNAMIC(CIpo, CDialogBar)

CIpo::CIpo()
{
   m_pBmp = NULL ;
}

CIpo::~CIpo()
{
	if( NULL != m_pBmp ) {
		DeleteObject(m_pBmp) ;
		m_pBmp = NULL ;
	}
}

void CIpo::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_listCtrl);
	DDX_Control(pDX, IDC_BUTTON_SELECT, m_rBtnSelect);
	DDX_Control(pDX, IDC_BUTTON_DRAW, m_rBtnDraw);
	DDX_Control(pDX, IDC_COMBO_APPID , m_listapp);
}


BEGIN_MESSAGE_MAP(CIpo, CDialogBar)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_SELECT, &CIpo::OnBnClickedButtonSelect)
	ON_BN_CLICKED(IDC_BUTTON_DRAW, &CIpo::OnBnClickedButtonDraw)
	ON_CBN_SELCHANGE(IDC_COMBO_APPID, &CIpo::OnCbnSelchangeComboAppid)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CIpo 消息处理程序
void CIpo::SetBkBmpNid( UINT nBitmapIn ) 
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

BOOL CIpo::OnEraseBkgnd(CDC* pDC)
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


int CIpo::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	SetBkBmpNid( IDB_BITMAP_BLACK ) ;
	return 0;
}


void CIpo::OnSize(UINT nType, int cx, int cy)
{
	CDialogBar::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if( NULL != GetSafeHwnd() ) {
		/*
		CRect rect ; 
		this->GetWindowRect(rect);

		CWnd *pst = GetDlgItem( IDC_STATIC_ADDRESS ) ;
		if ( NULL != pst ) {
			pst->SetWindowPos( NULL ,15 , 18 , 30, 30  ,SWP_SHOWWINDOW ) ; 
		}
		pst = GetDlgItem( IDC_EDIT_ADDRESS ) ;
		if ( NULL != pst ) {
			pst->GetClientRect( &rect ) ;
			pst->SetWindowPos( NULL ,15+30 , 15 , 308, 30  ,SWP_SHOWWINDOW ) ; 
		}
		pst = GetDlgItem( IDC_STATIC_APPID ) ;
		if ( NULL != pst ) {
			pst->SetWindowPos( NULL ,15+30+308+90 , 18 , 60, 30  ,SWP_SHOWWINDOW ) ; 
		}
		pst = GetDlgItem( IDC_COMBO_APPID ) ;
		if ( NULL != pst ) {
			pst->GetClientRect( &rect ) ;
			pst->SetWindowPos( NULL ,15+30+308+90+60 , 15 ,rect.Width(), rect.Height() ,SWP_SHOWWINDOW ) ; 
		}
		pst = GetDlgItem( IDC_BUTTON_SELECT ) ;
		if ( NULL != pst ) {
			pst->SetWindowPos( NULL ,15+30+308+90+60+160+15 , 15 , 81, 30  ,SWP_SHOWWINDOW ) ; 
		}
		pst = GetDlgItem( IDC_BUTTON_DRAW ) ;
		if ( NULL != pst ) {
			pst->SetWindowPos( NULL ,15+30+308+90+60+160+15+81+15 , 15 , 81, 30  ,SWP_SHOWWINDOW ) ; 
		}
		pst = GetDlgItem( IDC_LIST ) ;
		if ( NULL != pst ) {
			pst->SetWindowPos( NULL ,15 , 15+30+15 , 870, 600-30-30-30-72-32  ,SWP_SHOWWINDOW ) ; 
		}
		*/

		CRect rc ;
		GetClientRect( rc ) ;


		CWnd *pst = GetDlgItem( IDC_STATIC_ADDRESS ) ;
		if ( NULL != pst ) {
			CRect rect ;
			pst->GetClientRect( rect ) ;
			pst->SetWindowPos( NULL , (rc.Width()/100)*3+2, 40, rect.Width(), rect.Height()  ,SWP_SHOWWINDOW ) ; 
		}
		int curwith = 0;
		pst = GetDlgItem( IDC_EDIT_ADDRESS ) ;
		if ( NULL != pst ) {
			CRect rect ;
			pst->GetClientRect( rect ) ;
			pst->SetWindowPos( NULL ,(rc.Width()/100)*8 ,40 , (rc.Width()/100)*40, /*(rc.Height()/100)*6+2*/rect.Height() ,SWP_SHOWWINDOW ); 
			curwith = (rc.Width()/100)*8+5+(rc.Width()/100)*40 + 20;
		}

		pst = GetDlgItem( IDC_STATIC_APPID ) ;
		if ( NULL != pst ) {
			CRect rect ;
			pst->GetClientRect( rect ) ;
			pst->SetWindowPos( NULL ,curwith,40  , rect.Width(), rect.Height(),SWP_SHOWWINDOW ); 
			curwith+=rect.Width();
			curwith += 5;
		}
		pst = GetDlgItem( IDC_COMBO_APPID ) ;
		if ( NULL != pst ) {
			CRect rect ;
			pst->GetClientRect( rect ) ;
			pst->SetWindowPos( NULL ,curwith ,36  , rect.Width(), rect.Height(),SWP_SHOWWINDOW ); 
		}

		pst = GetDlgItem( IDC_BUTTON_SELECT ) ;
		if ( NULL != pst ) {
			CRect rect ;
			pst->GetWindowRect( rect ) ;
			pst->SetWindowPos( NULL ,(rc.Width()/100)*83 ,36, rect.Width(), rect.Height(), SWP_SHOWWINDOW ) ; 
		}

		//
		//	
		pst = GetDlgItem( IDC_BUTTON_DRAW ) ;
		if ( NULL != pst ) {
			CRect rect ;
			pst->GetClientRect(rect) ;
			pst->SetWindowPos( NULL ,(rc.Width()/100)*97 ,36, rect.Width(), rect.Height(), SWP_SHOWWINDOW ) ; 
		}

		pst = GetDlgItem(IDC_STATIC_IPO_GROUP);
		if(NULL != pst) {
			 pst->SetWindowPos( NULL , 10, 6, 880, 80 , SWP_SHOWWINDOW ) ;
		}

		pst = GetDlgItem( IDC_LIST ) ;
		if ( NULL != pst ) {
			CRect rect ;
			pst->GetClientRect( rect ) ;
			pst->SetWindowPos( NULL ,9 , 100 , 880, 360,SWP_SHOWWINDOW ) ; 
		}
	}
}


BOOL CIpo::Create(CWnd* pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID)
{
	// TODO: 在此添加专用代码和/或调用基类

	BOOL bRes = CDialogBar::Create(pParentWnd, nIDTemplate, nStyle, nID);
	if ( bRes ) {
		UpdateData(FALSE);
		struct LISTCol {
			CString		name ;
			UINT		size ;
		} listcol[3]  = {
			{"序号" ,      89},
			{"金额" ,      320}, 
			{"解冻高度" ,  300}
		};
		m_listCtrl.SetBkColor(RGB(240,240,240));       
		m_listCtrl.SetRowHeigt(23);               
		m_listCtrl.SetHeaderHeight(1.5);         
		m_listCtrl.SetHeaderFontHW(15,0);
		m_listCtrl.SetHeaderBKColor(/*224,65,18,*/ 31, 92, 155, 0); 
		m_listCtrl.SetHeaderTextColor(RGB(255,255,255)); 
		m_listCtrl.SetTextColor(RGB(0,0,0));  
		for( int i = 0 ; i <3 ; i++  ) {
			m_listCtrl.InsertColumn(i,listcol[i].name,LVCFMT_CENTER,listcol[i].size);
		}
		m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP );// |LVS_SINGLESEL  );

		m_rBtnSelect.SetBitmaps( IDB_BITMAP_BUT1 , RGB(255, 255, 0) , IDB_BITMAP_BUT2 , RGB(255, 255, 255) );
		m_rBtnSelect.SetAlign(CButtonST::ST_ALIGN_OVERLAP);
		m_rBtnSelect.SetWindowText("查询") ;
		m_rBtnSelect.SetFontEx(-12 , _T("微软雅黑"));
		m_rBtnSelect.SetColor(CButtonST::BTNST_COLOR_FG_OUT , RGB(255,255,255));
		m_rBtnSelect.SetColor(CButtonST::BTNST_COLOR_FG_IN , RGB(200, 75, 60));
		m_rBtnSelect.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(255, 255,255));
		m_rBtnSelect.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(255,255,255));
		m_rBtnSelect.SizeToContent();

		m_rBtnDraw.SetBitmaps( IDB_BITMAP_ORANGE_BUT1 , RGB(255, 255, 0) , IDB_BITMAP_ORANGE_BUT2 , RGB(255, 255, 255) );
		m_rBtnDraw.SetAlign(CButtonST::ST_ALIGN_OVERLAP);
		m_rBtnDraw.SetWindowText("提现") ;
		m_rBtnDraw.SetFontEx(-12 , _T("微软雅黑"));
		m_rBtnDraw.SetColor(CButtonST::BTNST_COLOR_FG_OUT , RGB(255,255,255));
		m_rBtnDraw.SetColor(CButtonST::BTNST_COLOR_FG_IN , RGB(200, 75, 60));
		m_rBtnDraw.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(255, 255,255));
		m_rBtnDraw.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(255,255,255));
		m_rBtnDraw.SizeToContent();

		InitCombox();
		m_listapp.SetCurSel(0);
		int sel =m_listapp.GetCurSel();
		if (sel != -1)
		{
			CString curText =_T("");
			m_listapp.GetLBText(sel,curText);
			string appname =strprintf("%s",curText);
			m_appid = GetAppID(appname);
		}

	}
	return bRes ;
}

void CIpo::OnShowListCtrol(CString addr)
{
	string strCommand,strShowData ="";
	strCommand =strprintf("%s %s %s","getappaccinfo" , m_appid ,addr);
	Json::Value root; 
	if(!CSoyPayHelp::getInstance()->SendRpc(strCommand,root))
	{
		TRACE("OnShowListCtrol rpccmd getappaccinfo error");
		return;
	}
	strShowData = root.toStyledString();

	m_listCtrl.DeleteAllItems();

	int pos = strShowData.find("FreeValues");
	INT64 nMoney = 0;
	if (pos >0)
	{
		nMoney = root["FreeValues"].asInt64() ;
	}
	double money = 0.0;
	int coulum = 0;
	int index = 0;
	if (nMoney != 0)
	{
		money = (nMoney*1.0/COIN);
		strShowData = strprintf("%.8f",money);
		string strOrder ="";
		int nSubIdx = 0;
		strOrder= strprintf("%d", 1);
		m_listCtrl.InsertItem(coulum,strOrder.c_str());
		m_listCtrl.SetItemText( coulum , ++nSubIdx, strShowData.c_str()) ;
		strOrder= "0";
		m_listCtrl.SetItemText(coulum , ++nSubIdx , strOrder.c_str() ) ;
		coulum = 1;
		index =1;
	}


	Json::Value valuearray = root["vFreezedFund"]; 
	for(unsigned int i =0;i<valuearray.size();i++)
	{
		int nSubIdx = 0;
		string strOrder ="";
		strOrder= strprintf("%d", index+1);
		m_listCtrl.InsertItem(coulum,strOrder.c_str());

		nMoney = valuearray[i]["value"].asInt64() ;
		money = (nMoney*1.0/COIN);
		strShowData = strprintf("%.8f",money);
		m_listCtrl.SetItemText( coulum , ++nSubIdx, strShowData.c_str()) ;

		strShowData =strprintf("%d" , valuearray[i]["nHeight"].asInt()) ;
		m_listCtrl.SetItemText(coulum , ++nSubIdx , strShowData.c_str() ) ;
		coulum++;
		index++;
	}
}

void CIpo::OnBnClickedButtonSelect()
{
	// TODO: 在此添加控件通知处理程序代码
	CString addr;
	GetDlgItem(IDC_EDIT_ADDRESS)->GetWindowText(addr);
	if (addr == _T(""))
	{
		UiFun::MessageBoxEx(_T("地址不能为空") ,  _T("提示") ,MFB_OK|MFB_TIP );
		return;
	}
	OnShowListCtrol(addr);
}

double CIpo::GetFreeMoney(CString addr)
{
	string strCommand,strShowData ="";
	strCommand =strprintf("%s %s %s","getappaccinfo" , m_appid ,addr);
	Json::Value root; 
	if(!CSoyPayHelp::getInstance()->SendRpc(strCommand,root))
	{
		TRACE("GetFreeMoney rpccmd getappaccinfo error");
		return 0.0;
	}

	INT64 nMoney = 0;

	nMoney = root["FreeValues"].asInt64() ;

	double money = (nMoney*1.0/COIN);
	return money;
}

void CIpo::OnBnClickedButtonDraw()
{
	// TODO: 在此添加控件通知处理程序代码
	if ( IDNO == UiFun::MessageBoxEx(_T("是否确定要提现") , _T("提示") , MFB_YESNO|MFB_TIP ) )
		return;

	if (!CheckRegIDValid( m_appid )) return ;

	string  strShowData = _T("");

	CString addr;
	GetDlgItem(IDC_EDIT_ADDRESS)->GetWindowText(addr);
	if (addr == _T(""))
	{
		UiFun::MessageBoxEx(_T("地址不能为空") ,  _T("提示") ,MFB_OK|MFB_TIP );
		return;
	}

	string strCond;
	strCond = strprintf(" address='%s' or  reg_id='%s'", addr,addr);
	uistruct::LISTADDR_t pAddr;
	int nItem =  theApp.m_SqliteDeal.GetWalletAddressItem(strCond, &pAddr) ;
	if (pAddr.address == "")
	{
		UiFun::MessageBoxEx(_T("此地址不是钱包地址,不能提现") , _T("提示") ,MFB_OK|MFB_TIP );
		return;
	}

	if (pAddr.bSign == 0 )
	{
		UiFun::MessageBoxEx(_T("此地址没有注册,不能提现") ,  _T("提示") ,MFB_OK|MFB_TIP );
		return;
	}
	double dmoney = GetFreeMoney(addr);
	if (dmoney <=0.0)
	{
		UiFun::MessageBoxEx(_T("此地址没有可提现的金额") ,  _T("提示") ,MFB_OK|MFB_TIP );
		return;
	}
	string strContractData ="";
	if (CheckRegIDValid( addr.GetString() )){
		strContractData=m_P2PBetHelp.GetAppAccountMoneyContract(addr.GetString(),1,1,REAL_MONEY(dmoney));
	}else{
		strContractData=m_P2PBetHelp.GetAppAccountMoneyContract(addr.GetString(),1,2,REAL_MONEY(dmoney));
	}


	CString strTxFee;
	INT64 minFee = theApp.m_AppBalanceCfg.WithdrawFee;
	double dnum = (minFee*1.0/COIN);
	strTxFee.Format(_T("%.8f"),dnum);

	if (  (INT64)REAL_MONEY(strtod(strTxFee,NULL)) < 10000  ) {
		UiFun::MessageBoxEx(_T("小费不足") ,  _T("提示") ,MFB_OK|MFB_TIP );
		return ;
	}

	string strData = CSoyPayHelp::getInstance()->CreateContractTx( m_appid,addr.GetString(),strContractData,0,(INT64)REAL_MONEY((strtod(strTxFee,NULL))),0);
	CSoyPayHelp::getInstance()->SendContacrRpc(strData,strShowData);

	if (strShowData =="")
	{
		return;
	}
	Json::Reader reader;  
	Json::Value root;
	if (!reader.parse(strShowData, root)) 
		return  ;
	BOOL bRes = FALSE ;
	string strTip;
	int pos = strShowData.find("hash");

	if ( pos >=0 ) {
		//插入到交易记录数据库
		string strHash = root["hash"].asString();
		CPostMsg postmsg(MSG_USER_GET_UPDATABASE,WM_REVTRANSACTION);
		postmsg.SetData(strHash);
		theApp.m_msgQueue.push(postmsg);
	}

	if ( pos >=0 ) {
		bRes = TRUE ;
		strTip = strprintf("%s\n%s" , _T("恭喜提现成功!"),root["hash"].asCString() ) ;
	}else{
		strTip = _T("提现失败") ;
	}
	UiFun::MessageBoxEx(strTip.c_str() , _T("提示") ,MFB_OK|MFB_TIP );
}

void CIpo::InitCombox()
{
	map<string,CONFIG_APP_DATA>::iterator it= theApp.m_listapp.begin();
	for(;it != theApp.m_listapp.end();it++)
	{
		CONFIG_APP_DATA data=it->second;
		CString temp;

		temp.Format(_T("%s"),data.appname.c_str());	

		m_listapp.AddString(temp);
	}

}

string CIpo::GetAppID(string AppName)
{
	map<string,CONFIG_APP_DATA>::iterator it= theApp.m_listapp.begin();
	for(;it != theApp.m_listapp.end();it++)
	{
		CONFIG_APP_DATA data=it->second;
		if (strcmp(data.appname.c_str(),AppName.c_str()) ==0)
		{
			return data.appid;
		}
	}
	return "";
}

void CIpo::OnCbnSelchangeComboAppid()
{
	// TODO: 在此添加控件通知处理程序代码
	int sel =m_listapp.GetCurSel();
	if (sel != -1)
	{
		CString curText =_T("");
		m_listapp.GetLBText(sel,curText);
		string appname =strprintf("%s",curText);
		m_appid = GetAppID(appname);
	}
}


BOOL CIpo::PreTranslateMessage(MSG* pMsg)
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


HBRUSH CIpo::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogBar::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (nCtlColor == CTLCOLOR_STATIC)
	{

		pDC->SetBkMode(TRANSPARENT);
		return HBRUSH(GetStockObject(HOLLOW_BRUSH));

	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
