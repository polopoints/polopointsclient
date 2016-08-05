// Send.cpp : 实现文件
//

#include "stdafx.h"
#include "polopointclient.h"
#include "Send.h"
#include "AddrBook.h"
#include "afxdialogex.h"


// CSend 对话框

IMPLEMENT_DYNAMIC(CSend, CDialogBar)

CSend::CSend()
{
	m_pBmp = NULL ;
}

CSend::~CSend()
{
	if( NULL != m_pBmp ) {
		DeleteObject(m_pBmp) ;
		m_pBmp = NULL ;
	}
}

void CSend::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_SEND , m_rBtnSend);
	DDX_Control(pDX, IDC_STATIC_XM , m_strTx1);
	DDX_Control(pDX, IDC_COMBO_PAY_ADDRESS , m_addrbook);
	DDX_Control(pDX, IDC_BUTTON_SELECT , m_rBtnAddbook);
	DDX_Control(pDX, IDC_COMBO_UNIT , m_cboUnit);
}


BEGIN_MESSAGE_MAP(CSend, CDialogBar)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_SELECT, &CSend::OnBnClickedButtonSelect)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CSend::OnBnClickedButtonSend)
	ON_CBN_SELCHANGE(IDC_COMBO_PAY_ADDRESS, &CSend::OnCbnSelchangeComboPayAddress)
	ON_MESSAGE(MSG_USER_SEND_UI , &CSend::OnShowListaddrData  )
END_MESSAGE_MAP()


// CSend 消息处理程序
void CSend::SetBkBmpNid( UINT nBitmapIn ) 
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

BOOL CSend::OnEraseBkgnd(CDC* pDC)
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


int CSend::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	SetBkBmpNid( IDB_BITMAP_SEND_BALCK ) ;
	return 0;
}


void CSend::OnSize(UINT nType, int cx, int cy)
{
	CDialogBar::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	
	if( NULL != GetSafeHwnd() ) {
		CRect rect ; 
		this->GetWindowRect(rect);

		CWnd *pst = GetDlgItem( IDC_COMBO_PAY_ADDRESS ) ;
		if ( NULL != pst ) {
			pst->GetClientRect( rect ) ;
			pst->SetWindowPos( NULL ,220 , 70 , 334,  rect.Height()  ,SWP_SHOWWINDOW ) ; 
		}
		pst = GetDlgItem( IDC_STATIC_XM ) ;
		if ( NULL != pst ) {
		pst->GetClientRect( rect ) ;
		pst->SetWindowPos( NULL ,  220 + 333 + 80, 75 , rect.Width(), rect.Height()  ,SWP_SHOWWINDOW ) ; 
		}
		pst = GetDlgItem( IDC_EDIT_AIM_ADDRESS ) ;
		if ( NULL != pst ) {
			pst->GetClientRect( rect ) ;
			pst->SetWindowPos( NULL ,220 , 121 , 334, /*rect.Height()*/25  ,SWP_SHOWWINDOW ) ; 
		}
		pst = GetDlgItem( IDC_BUTTON_SELECT ) ;
		if ( NULL != pst ) {
			pst->GetWindowRect( rect ) ;
			pst->SetWindowPos( NULL ,220 + 333 + 10 , 117 , rect.Width(), rect.Height(),SWP_SHOWWINDOW ) ; 
		}
		pst = GetDlgItem( IDC_EDIT_Lebel ) ;
		if ( NULL != pst ) {
			pst->GetClientRect( rect ) ;
			pst->SetWindowPos( NULL ,220 , 168 , 334, 25  ,SWP_SHOWWINDOW ) ; 
		}
		pst = GetDlgItem( IDC_EDIT_MONEY ) ;
		if ( NULL != pst ) {
			pst->GetClientRect( rect ) ;
			pst->SetWindowPos( NULL ,220 , 217 , 260, 25  ,SWP_SHOWWINDOW ) ; 
		}
		pst = GetDlgItem( IDC_COMBO_UNIT ) ;
		if ( NULL != pst ) {
			pst->GetClientRect( rect ) ;
			pst->SetWindowPos( NULL ,480 + 8 , 217 , rect.Width(), rect.Height()  ,SWP_SHOWWINDOW ) ; 
		}
		pst = GetDlgItem( IDC_BUTTON_SEND ) ;
		if ( NULL != pst ) {
			pst->GetWindowRect(rect);
			pst->SetWindowPos( NULL ,220 , 260 , rect.Width(), rect.Height()  ,SWP_SHOWWINDOW ) ; 
		}
	}
}


BOOL CSend::Create(CWnd* pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID)
{
	// TODO: 在此添加专用代码和/或调用基类

	BOOL bRes = CDialogBar::Create(pParentWnd, nIDTemplate, nStyle, nID);
	if ( bRes ){

		m_rBtnAddbook.LoadBitmaps(IDB_BITMAP_ADDBOOK,IDB_BITMAP_ADDBOOK,IDB_BITMAP_ADDBOOK,IDB_BITMAP_ADDBOOK);
		UpdateData(0);
		m_strTx1.SetFont(120, _T("黑体"));				//设置显示字体和大小
		m_strTx1.SetTextColor(RGB(255, 0, 0));
		m_strTx1.SetWindowText(_T(""));
		AddListaddrDataBox();

		UpdateData(FALSE);
		m_rBtnSend.SetBitmaps( IDB_BITMAP_BUT1 , RGB(255, 255, 0) , IDB_BITMAP_BUT2 , RGB(255, 255, 255) );
		m_rBtnSend.SetAlign(CButtonST::ST_ALIGN_OVERLAP);
		m_rBtnSend.SetWindowText("发送") ;
		m_rBtnSend.SetFontEx(-12 , _T("微软雅黑"));
		m_rBtnSend.SetColor(CButtonST::BTNST_COLOR_FG_OUT , RGB(255, 255, 255));
		m_rBtnSend.SetColor(CButtonST::BTNST_COLOR_FG_IN , RGB(200, 75, 60));
		m_rBtnSend.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(255, 255, 255));
		m_rBtnSend.SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(255, 255, 255));
		m_rBtnSend.SizeToContent();

		theApp.SubscribeMsg( theApp.GetMtHthrdId() , GetSafeHwnd() , MSG_USER_SEND_UI ) ;
		((CComboBox*)GetDlgItem(IDC_COMBO_UNIT))->AddString(_T("PLP"));
		((CComboBox*)GetDlgItem(IDC_COMBO_UNIT))->SetCurSel(0);
	}
	return bRes ;
}

BOOL CSend::AddListaddrDataBox(){

	theApp.m_SqliteDeal.GetWalletAddressList(_T(" 1=1 "), &m_mapAddrInfo);

	if ( 0 == m_mapAddrInfo.size() ) return FALSE ;

	//清除ComBox控件
	((CComboBox*)GetDlgItem(IDC_COMBO_PAY_ADDRESS))->ResetContent();
	//加载到ComBox控件
	int nItem = 0;
	std::map<string,uistruct::LISTADDR_t>::const_iterator const_it;
	for ( const_it = m_mapAddrInfo.begin() ; const_it != m_mapAddrInfo.end() ; const_it++ ) {

		((CComboBox*)GetDlgItem(IDC_COMBO_PAY_ADDRESS))->InsertString(nItem , const_it->first.c_str() );
		//((CComboBox*)GetDlgItem(IDC_COMBO_ADDR_OUT))->SetItemData(nItem, (DWORD_PTR)&(*const_it));
		nItem++;
	}
	((CComboBox*)GetDlgItem(IDC_COMBO_PAY_ADDRESS))->SetCurSel(0);

	CString address;
	int sel = m_addrbook.GetCurSel();
	if (sel < 0)
	{
		return FALSE;
	}
	m_addrbook.GetLBText(sel,address);
	string strAddr =strprintf("%s",address);
	std::map<string,uistruct::LISTADDR_t>::const_iterator item = m_mapAddrInfo.find(strAddr);

	if (m_mapAddrInfo.count(strAddr)>0 ) {
		uistruct::LISTADDR_t addrstruc = m_mapAddrInfo[strAddr];
		string strshow;
		strshow = strprintf("%.8f",addrstruc.fMoney);
		m_strTx1.SetWindowText(strshow.c_str());
		Invalidate();
	}
	return TRUE ;
}


void CSend::OnBnClickedButtonSelect()
{
	// TODO: 在此添加控件通知处理程序代码
	CAddrBook addrbook;
	INT_PTR nResponse = addrbook.DoModal();
	if (nResponse == IDOK)
	{
		uistruct::ADDRBOOK_t addr;
		addrbook.GetAddrbook(addr);
		GetDlgItem(IDC_EDIT_AIM_ADDRESS)->SetWindowTextA(addr.address.c_str());
		GetDlgItem(IDC_EDIT_Lebel)->SetWindowTextA(addr.label.c_str());
	}
}


BOOL CSend::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	
	if (pMsg->message==WM_PASTE)
	{
		int b =5;
	}
	if (pMsg->message==WM_KEYDOWN)
	{
		BOOL bCtrl=::GetKeyState(VK_CONTROL)&0x8000;
		BOOL bShift=::GetKeyState(VK_SHIFT)&0x8000;

		// only gets here if CTRL key is pressed
		BOOL bAlt=::GetKeyState(VK_MENU)&0x8000;

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

		switch( pMsg->wParam )
		{
			if (GetDlgItem(IDC_COMBO_PAY_ADDRESS)->SetFocus() == this->GetFocus())
			{
				int b = 6;
			}
		case 'V':
			{
				if (GetDlgItem(IDC_EDIT_AIM_ADDRESS) == this->GetFocus())
				{
					HWND hWnd = GetSafeHwnd(); // 获取安全窗口句柄
					::OpenClipboard(hWnd); // 打开剪贴板
					HANDLE hClipMemory = ::GetClipboardData(CF_TEXT);// 获取剪贴板数据句柄
					DWORD dwLength = GlobalSize(hClipMemory); // 返回指定内存区域的当前大小
					LPBYTE lpClipMemory = (LPBYTE)GlobalLock(hClipMemory); // 锁定内存
					CString message = CString(lpClipMemory); // 保存得到的文本数据
					GlobalUnlock(hClipMemory); // 内存解锁
					::CloseClipboard(); // 关闭剪贴板
					if (bCtrl)
					{
						uistruct::ADDRBOOK_t addrBook;
						string strCond;
						strCond = strprintf(" address='%s' ", message);
						int nItem =  theApp.m_SqliteDeal.GetAddressBookItem( strCond, &addrBook) ;
						if (addrBook.label != _T(""))
						{
							GetDlgItem(IDC_EDIT_Lebel)->SetWindowText(addrBook.label.c_str());
						}

					}

				}else if (GetDlgItem(IDC_COMBO_PAY_ADDRESS)->SetFocus() == this->GetFocus())
				{
					HWND hWnd = GetSafeHwnd(); // 获取安全窗口句柄
					::OpenClipboard(hWnd); // 打开剪贴板
					HANDLE hClipMemory = ::GetClipboardData(CF_TEXT);// 获取剪贴板数据句柄
					DWORD dwLength = GlobalSize(hClipMemory); // 返回指定内存区域的当前大小
					LPBYTE lpClipMemory = (LPBYTE)GlobalLock(hClipMemory); // 锁定内存
					CString message = CString(lpClipMemory); // 保存得到的文本数据
					GlobalUnlock(hClipMemory); // 内存解锁
					::CloseClipboard(); // 关闭剪贴板
					if (bCtrl)
					{
						string addr = strprintf("%s",message);
						if(m_mapAddrInfo.count(addr)<=0)
						{
							MessageBox(_T("复制的地址有误"),_T("提示"), MB_OK);
							//UiFun::MessageBoxEx(UiFun::UI_LoadString("SEND_MODULE" , "SEND_SOPY_ADDR_ERROR" ,theApp.gsLanguage) , UiFun::UI_LoadString("COMM_MODULE" , "COMM_TIP" ,theApp.gsLanguage) ,MFB_OK|MFB_TIP );
						}else{
							//uistruct::LISTADDR_t te = m_pListaddrInfo[text];
							CString strshow;
							strshow.Format(_T("%.8f"),m_mapAddrInfo[addr].fMoney);
							((CStatic*)GetDlgItem(IDC_STATIC_XM))->SetWindowText(strshow);
							Invalidate();
						}

					}
				}

			}
			break;
		}
	}
	
	return CDialogBar::PreTranslateMessage(pMsg);
}


void CSend::OnBnClickedButtonSend()
{
	// TODO: 在此添加控件通知处理程序代码
	if (theApp.IsLockWallet())
	{
		return ;
	}

	if (m_mapAddrInfo.size() == 0)
	{
		UiFun::MessageBoxEx(_T("发送地址不存在") , _T("提示") ,MFB_OK|MFB_TIP );
		return;
	}
	CString text =_T("");
	//int sel = m_addrbook.GetCurSel();
	//if (sel < 0)
	//{
	//	return ;
	//}
	//m_addrbook.GetLBText(sel,text);
	m_addrbook.GetWindowText(text);
	if (text == _T(""))
	{
		return;
	}
	uistruct::LISTADDR_t data;
	string strCommand  ;CString strMoney,strMaddress;
	if(text!=_T(""))
	{
		string strAddr =strprintf("%s",text);
		strAddr =UiFun::trimleft(strAddr);
		strAddr =UiFun::trimright(strAddr);
		if(m_mapAddrInfo.count(strAddr)<=0)
		{
			UiFun::MessageBoxEx(_T("发送地址不是钱包地址") , _T("提示") ,MFB_OK|MFB_TIP );
			return;
		}
		//uistruct::LISTADDR_t te = m_pListaddrInfo[text];
		data = m_mapAddrInfo[strAddr];

	}

	if(!data.bSign) 
	{
		UiFun::MessageBoxEx(_T("发送地址未激活") ,  _T("提示"),MFB_OK|MFB_TIP );
		return;
	}

	GetDlgItem(IDC_EDIT_AIM_ADDRESS)->GetWindowTextA(strMaddress);
	if (strMaddress == _T(""))
	{
		UiFun::MessageBoxEx(_T("接受地址不能为空") , _T("提示") ,MFB_OK|MFB_TIP );
		return;
	}
	if(!strcmp(strMaddress.GetString(), data.address.c_str()))
	{
		UiFun::MessageBoxEx(_T("发送地址和目的地址不能相同") , _T("提示") ,MFB_OK|MFB_TIP );
		return;
	}

	GetDlgItem(IDC_EDIT_MONEY)->GetWindowTextA(strMoney);
	double dSendMoney = strtod(strMoney,NULL);
	if(dSendMoney > data.fMoney || ( data.fMoney>-0.0000001 && data.fMoney< 0.000001 )) 
	{
		UiFun::MessageBoxEx(_T("账户余额不足") , _T("提示") ,MFB_OK|MFB_TIP );
		return;
	}

	if(_T("") == strMoney.GetString() || (dSendMoney >-0.0000001 && dSendMoney< 0.000001))
	{
		UiFun::MessageBoxEx(_T("发送金额不能为0") ,_T("提示") ,MFB_OK|MFB_TIP );
		return;
	}
	
	if (!theApp.m_bIsSyncBlock )
	{
		UiFun::MessageBoxEx(_T("同步未完成,不能发送交易") , _T("提示") ,MFB_OK|MFB_TIP );
		return;
	}
	

	strCommand = strprintf("%s %s %s %lld","sendtoaddress" ,data.address.c_str() ,strMaddress ,REAL_MONEY(dSendMoney));
	string strShowData ;

	CString strDisplay;
	strDisplay.Format(_T("%s%.4lfPLP%s%s"),_T("转账"), dSendMoney, _T("至") ,strMaddress);

	if (IDCANCEL == UiFun::MessageBoxEx(strDisplay , _T("提示") , MFB_OKCANCEL|MFB_TIP ) )
	{
		return;
	}

	strShowData = _T("");
	Json::Value root;
	if(!CSoyPayHelp::getInstance()->SendRpc(strCommand,root))
	{
		TRACE("OnBnClickedSendtrnsfer rpccmd sendtoaddress error");
		return;
	}
	BOOL bRes = FALSE ;
	strShowData = root.toStyledString();
	int pos = strShowData.find("hash");
	if ( pos >=0 ) {
		//插入到数据库
		string strHash = root["hash"].asString() ;
		string strCond;
		strCond = strprintf(" hash='%s' ", strHash.c_str());
		int nItem =  theApp.m_SqliteDeal.GetTableCountItem(_T("t_transaction") , strCond) ;

		if ( 0 == nItem ) {

			CPostMsg postmsg(MSG_USER_GET_UPDATABASE,WM_REVTRANSACTION);
			postmsg.SetData(strHash);
			theApp.m_msgQueue.push(postmsg);
		}
	}

	string strData;
	if ( pos >=0 ) {
		strData = strprintf(" %s\n\n %s:%s\n\n %s:%s\n\n %s：%.4lf\n  hash:%s",_T("转账交易发送成功"),_T("源地址"), data.address.c_str(),_T("目的地址"),strMaddress,_T("金额"), dSendMoney,root["hash"].asCString()) ;
	}else{
		strData = _T("转账失败!");
	}
	UiFun::MessageBoxEx(strData.c_str() , _T("") ,MFB_OK|MFB_TIP );


	//// 插入数据库,将收款人添加到地址簿
	CString label;
	GetDlgItem(IDC_EDIT_Lebel)->GetWindowTextA(label);
	CPostMsg postmsg(MSG_USER_GET_UPDATABASE,WM_UP_ADDRBOOK);
	uistruct::ADDRBOOK_t addr;
	addr.address = strMaddress;
	addr.label = label;
	string temp =addr.ToJson();
	postmsg.SetData(temp);
	theApp.m_msgQueue.push(postmsg);

}


void CSend::OnCbnSelchangeComboPayAddress()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_mapAddrInfo.size() == 0)
	{
		return;
	}
	CString text;
	int sel = m_addrbook.GetCurSel();
	if (sel < 0)
	{
		return ;
	}
	m_addrbook.GetLBText(sel,text);

	//m_addrbook.GetWindowText(text) ;
	string strAddr= strprintf("%s",text);
	if(text!=_T(""))
	{

		if(m_mapAddrInfo.count(strAddr)<=0)
		{
			TRACE("map OnCbnSelchangeCombo1 error");
			return;
		}
		//uistruct::LISTADDR_t te = m_pListaddrInfo[text];
		string strshow;
		strshow = strprintf("%.8f",m_mapAddrInfo[strAddr].fMoney);
		((CStatic*)GetDlgItem(IDC_STATIC_XM))->SetWindowText(strshow.c_str());
		Invalidate();
	}
}

void CSend::ModifyComboxItem(){
	CPostMsg postmsg;
	if (!theApp.m_msgUISendDlgQueue.pop(postmsg))
	{
		return ;
	}

	uistruct::LISTADDR_t addr; 
	string strTemp = postmsg.GetData();
	addr.JsonToStruct(strTemp.c_str());


	if(m_mapAddrInfo.count(addr.address)<=0)
	{
		TRACE("map ModifyComboxItem error");
		return;
	}
	m_mapAddrInfo[addr.address]=addr;


}
void CSend::InsertComboxIitem()
{
	CPostMsg postmsg;
	if (!theApp.m_msgUISendDlgQueue.pop(postmsg))
	{
		return ;
	}

	uistruct::LISTADDR_t addr; 
	string strTemp = postmsg.GetData();
	addr.JsonToStruct(strTemp.c_str());


	if(m_mapAddrInfo.count(addr.address)>0)
	{
		TRACE("map InsertComboxIitem error");
		return;
	}
	m_mapAddrInfo[addr.address]=addr;

	int item = m_addrbook.GetCount();
	m_addrbook.InsertString(item,addr.address.c_str());
}

LRESULT CSend::OnShowListaddrData( WPARAM wParam, LPARAM lParam ) 
{
	//更新数据
	int type = (int)wParam;
	switch(type)
	{
	case WM_UP_ADDRESS:
		{
			ModifyComboxItem();
			OnCbnSelchangeComboPayAddress();
			break;
		}
		break;
	case WM_UP_NEWADDRESS:
		{
			InsertComboxIitem();
			break;
		}
		break;
	default:
		break;

	}
	return 0 ;
}
