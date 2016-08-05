
// polopointclient.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "polopointclient.h"
#include "polopointclientDlg.h"
#include "UiHead.h"
#include "SynchronousSocket.h"
#include "Log.h"
#include "PostMsg.h"
#include "CommonStruct.h"
#include "WalletPassPhrase.h"

#include"tlhelp32.h"
#include <afxsock.h>

#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#include "shlwapi.h"
#pragma comment(lib,"shlwapi.lib")
// ChuobiApp

BEGIN_MESSAGE_MAP(CpolopointclientApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// ChuobiApp 构造

CpolopointclientApp::CpolopointclientApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
	m_bReIndexServer = FALSE;
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	m_nLockStatus = STATUS_DECRYPTED;		//初始化为钱包无加密
	m_bIsDlgCreateFinished = FALSE;			//默认没有创建完成
	m_bIsSyncTx = FALSE;					//开启交易同步
	m_bIsUpdateCheckRunSwitch = TRUE;		//更新检查线程开关标志,默认开启状态  
	m_bIsProcessMsgRunSwitch = TRUE;		//处理消息线程开关标志,默认开启状态  
	m_bIsReceiveMsgRunSwitch = TRUE;		//接收服务端数据线程开关标志,默认开启状态
    m_bIsDlgCreateFinished = FALSE;			//主对话框框的所有dlg是否创建完
	m_bIsStartMainDlg = FALSE;				//是否启动主对话框
	m_bIsSyncTx = FALSE;					//是否同步交易
	m_nNetWorkType = REGTEST;				//初始化网络为本地局域网       
	m_strServerCfgFileName = "polopoints.conf";  //服务器配置文件名
	m_nSyncHight = 0;                       //同步高度初始化
	m_bIsCommitDb = FALSE;                  //是否提交执行到数据库
	m_bIsSyncBlock = FALSE;                 //从网络同步block标志
	m_bIsWalletLocked = FALSE;              //钱包加密标志
	m_bIsHaveLocked = FALSE;                //钱包是否锁定状态
	m_bIsPassLock = TRUE;

	m_reminder = 0;
	m_poptips = true;
}


// 唯一的一个 ChuobiApp 对象

CpolopointclientApp theApp;


// ChuobiApp 初始化

BOOL CpolopointclientApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	if (!RunOnlyOneApp())
	{
		return FALSE;
	}

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	if (!AfxSocketInit()){   //初始化SOCKET
		return FALSE ;
	}

	sSubscribeList.clear();
	v_ProcSubThrd.clear();
	InitializeCriticalSection(&(cs_MsgDisptch) ) ;
	GetMoFilename(m_strInsPath, m_strModuleFilename); //获取文件路径和文件名称

	//检查工作目录是否为非空格的目录
	CheckPathValid(m_strInsPath);

	//创建维护线程
	if( !CreateMaintainThrd() ) {
		return FALSE ;
	}

	/// 加载配置文件polopointsclient.conf
	ParseUIConfigFile(m_strInsPath);

	//初始化日志配置参数
	InitLogCfg();


	string strRemoteRpcPort = m_rpcPort;
	string strRemoteUIPort = m_uiPort;
	ProductHttpHead(m_strInsPath ,m_strServerCfgFileName, m_rpcPort, m_sendPreHeadstr, m_sendEndHeadstr, m_uiPort, m_nNetWorkType);

	string dbpath = strprintf("%s\\db",m_strInsPath);
	if (!PathIsDirectory(dbpath.c_str()))
	{
		::CreateDirectory(dbpath.c_str(), NULL);
	}
	//打开sqlite3数据库
	m_SqliteDeal.InitializationDB();

	//判断配置为远程访问
	if (strcmp(m_severIp.c_str(),_T("127.0.0.1")))
	{
		m_rpcPort = strRemoteRpcPort;
		m_uiPort = strRemoteUIPort;
	}

	//检查是否存在服务端主程序
	OnCheckSeverfile();

	//启动服务程序
	StartSeverProcess(m_strInsPath);

	//更新检查
	Update();

	Sleep(1000);
	//m_pSocket = NULL ;
	//m_pSocket = new CHSocket(_T("127.0.0.1") , 12345);
	//if ( NULL != m_pSocket ) {
	//	if ( !m_pSocket->OnConnnect()) {
	//		AfxMessageBox("服务器连接失败!");
	//		return FALSE ;
	//	}
	//}

	//连接到服务器
	CSynchronousSocket te;
	SOCKET nSocket = te.OnblockConnnect(m_severIp.c_str(),atoi(m_uiPort.c_str()) ) ;
	if ( INVALID_SOCKET != nSocket ){
		TRACE("nSocket OK\n");
		theApp.m_uiConnSocket = nSocket;
	}
	else
	{
		TRACE("nSocket Error\n");
	}

	theApp.StartWorkThread();  //启动接收消息、处理消息线程

	m_strProgressGifFile =   m_strInsPath + "\\gif\\progress.gif\0";
	m_strProgressOutGifFile =  m_strInsPath + "\\gif\\exit.gif\0";
	//必须加上此初始化操作，否则调用CGIFControl::Load(LPSTCR filePath)报错
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Status state = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) ;

	pSplashThread = (CSplashThread*) AfxBeginThread(RUNTIME_CLASS(CSplashThread),THREAD_PRIORITY_NORMAL,0, CREATE_SUSPENDED); 
	ASSERT(pSplashThread->IsKindOf(RUNTIME_CLASS(CSplashThread)));
	pSplashThread->ResumeThread(); 
	Sleep(1); 

	while(1)
	{
		if (m_bIsStartMainDlg)
		{
			break;
		}
		Sleep(100);
	}

	CpolopointclientDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

BOOL CpolopointclientApp::RunOnlyOneApp()
{
	HANDLE hMutex = CreateMutex(NULL, FALSE, _T("PoloPointClient"));
	// 检查错误代码
	if (GetLastError() == ERROR_ALREADY_EXISTS) 
	{
		CloseHandle(hMutex);
		AfxMessageBox(_T("程序已经在运行了") ,MB_OK);
		exit(1);  

		return FALSE;
	}

	return TRUE;
}

BOOL CpolopointclientApp::CreateMaintainThrd() 
{
	hMtThrd		= NULL ;
	nMtThrdID	= 0  ;
	hMtStartEvent = ::CreateEvent(0,FALSE,FALSE,0) ; 

	hMtThrd = (HANDLE)_beginthreadex( NULL, 0, &MtProc, this, 0, &nMtThrdID ) ;
	if( INVALID_HANDLE_VALUE == hMtThrd ) {
		::MessageBox( NULL , "维护线程创建失败!" , "出错" , MB_ICONERROR ) ;
		CloseHandle( hMtStartEvent ) ;
		hMtStartEvent = NULL ;
		return  FALSE;
	}

	::WaitForSingleObject( hMtStartEvent , 100000 ) ;
	CloseHandle( hMtStartEvent ) ;
	hMtStartEvent = NULL ;

	return TRUE ;	
}


UINT __stdcall CpolopointclientApp::MtProc(LPVOID pParam)
{
	MSG	msg , msgcpy ;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE) ;

	if(!SetEvent( ((CpolopointclientApp*)pParam)->hMtStartEvent ) ) {
		return 1;
	}

	while ( 1 )
	{	
		BOOL MsgReturn	= GetMessage( &msg , NULL , 0 , 0 );
		memset( &msgcpy , 0 , sizeof(MSG) ) ; 
		memcpy( &msgcpy , &msg , sizeof(MSG) );
		if ( MsgReturn )
		{
			switch ( msg.message )
			{
			case MSG_USER_MOBILE_STATEUS:
				{
                    ::PostThreadMessage( ((CpolopointclientApp*)pParam)->GetMtHthrdId() , MSG_USER_MOBILE_TIP , msgcpy.wParam , msgcpy.lParam ) ;
				}
				break;
			case MSG_USER_QUITTHREAD:
				{
					std::vector< sThrd >::iterator it ;
					CpolopointclientApp* pApp = ((CpolopointclientApp*)pParam) ;
					for( it = pApp->v_ProcSubThrd.begin() ; it != pApp->v_ProcSubThrd.end() ; it++ ) {
						::PostThreadMessage( it->hThrdId , MSG_USER_QUITTHREAD , 0 , 0 ) ;
						DWORD exc = 0xffff ;
						while( ::GetExitCodeThread( it->hThrd , &exc ) ) {
							if( STILL_ACTIVE == exc ) {
								;
							}else {
								TRACE( "EXC = %d \n" , exc ) ;
								break;
							}
						}
					}
					pApp->v_ProcSubThrd.clear() ;
					_endthreadex( 0 ) ; 
				}
				break;
			default:
				break;
			}
			((CpolopointclientApp*)pParam)->DispatchMsg( ((CpolopointclientApp*)pParam)->GetMtHthrdId() , msgcpy.message , msgcpy.wParam , msgcpy.lParam ) ;
		}
	}
}
void CpolopointclientApp::RegThrdToMtThrd( HANDLE hThread , unsigned int hThrdId )
{
	if( NULL == hThrdId || NULL == hThread ) {
		return ;
	}
	sThrd sthrd ;
	memset(  &sthrd , 0 , sizeof( sThrd ) ) ;
	sthrd.hThrd = hThread ;
	sthrd.hThrdId = hThrdId ;

	UnRegThrdToMtThrd( sthrd.hThrd , sthrd.hThrdId ) ;
	v_ProcSubThrd.push_back( sthrd ) ;
	TRACE("The thread id=%d RegThrdToMtThrd\n" , hThrdId ) ;
}
void CpolopointclientApp::UnRegThrdToMtThrd( HANDLE hThread , unsigned int hThrdId ) 
{
	if( NULL == hThrdId || NULL == hThread ) {
		return ;
	}
	std::vector< sThrd >::iterator it ;
	for( it = v_ProcSubThrd.begin() ; it != v_ProcSubThrd.end() ; it++ ) {
		if( hThrdId == it->hThrdId || 
			hThread == it->hThrd ) {
				TRACE("The thread id=%d UnRegThrdToMtThrd\n" , it->hThrdId ) ;
				v_ProcSubThrd.erase( it );
				break ;
		}
	}
}
void CpolopointclientApp::SubscribeMsg( DWORD nThreadId , HWND hWnd, UINT msg , void * relMsgMem )
{
	if( NULL == hWnd ) {
		return ;
	}
	//不重复订阅
	UnSubscribeMsg( nThreadId , hWnd , msg ) ;
	//
	MSG_NOTIFY msgNotify ;
	memset(  &msgNotify , 0 , sizeof( MSG_NOTIFY ) ) ;
	msgNotify.nThreadId = nThreadId ;
	msgNotify.hSubWnd   = hWnd		;
	msgNotify.msg		= msg		;
	msgNotify.relMsgMem	= relMsgMem	;

	EnterCriticalSection( &theApp.cs_MsgDisptch ) ;
	sSubscribeList.push_back( msgNotify ) ;
	LeaveCriticalSection (&theApp.cs_MsgDisptch ) ;
}

void CpolopointclientApp::UnSubscribeMsg( DWORD nThreadId , HWND hWnd, UINT msg )
{
	if( NULL == hWnd ) {
		return ;
	}
	std::vector<MSG_NOTIFY>::iterator it ;
	EnterCriticalSection( &theApp.cs_MsgDisptch ) ;
	for( it = sSubscribeList.begin() ; it != sSubscribeList.end() ; it++ ) {
		if( nThreadId == it->nThreadId && \
			hWnd == it->hSubWnd && \
			msg == it->msg ) {
				sSubscribeList.erase( it );
				break ;
		}
	}
	LeaveCriticalSection (&theApp.cs_MsgDisptch ) ;
}

void CpolopointclientApp::DispatchMsg( unsigned int threadID , UINT msg , WPARAM wParam , LPARAM lParam ) 
{
	BOOL flag = FALSE ;
	std::vector<MSG_NOTIFY>::iterator it ;

	EnterCriticalSection( &cs_MsgDisptch ) ;
	for( it = sSubscribeList.begin() ; it != sSubscribeList.end() ; it++ ) {
		if( threadID == it->nThreadId && msg == it->msg ) {
			if( NULL != it->relMsgMem ) {
				flag = TRUE ;
				::PostMessage( it->hSubWnd , msg , wParam , (LPARAM)it->relMsgMem ) ;
			} else {
				::PostMessage( it->hSubWnd , msg , wParam , lParam ) ;
			}
		}
	}
	LeaveCriticalSection (&cs_MsgDisptch ) ;

	if( flag ) {
		uiapi::ReleaseP( (void*)lParam );
	}
}

void CpolopointclientApp::GetMoFilename( string & path , string & filename ) 
{
	char strPath[MAX_PATH] ;
	GetModuleFileName( AfxGetApp()->m_hInstance , strPath , MAX_PATH );
	CString str=strPath;
	int i=str.ReverseFind('\\');
	path		= str.Left(i);
	filename	= strPath;	
}

void CpolopointclientApp::StartSeverProcess(const string& strdir)
{
	CloseProcess(_T("polopointsd.exe"));

	STARTUPINFOA si; 
	memset(&si, 0, sizeof(STARTUPINFO));  
	si.cb = sizeof(STARTUPINFO);  
	si.dwFlags = STARTF_USESHOWWINDOW;  
	si.wShowWindow =SW_HIDE;

//	CString str;
//	str.Format(_T("polopointsd.exe -datadir=%s -ui=1") ,strdir ) ;  //str = "polopointsd.exe -datadir=D:\code\polopoints\bin -ui=1"
	string startBat = strprintf("polopointsd -datadir=%s -ui=1", strdir);
	if(m_bReIndexServer)
		startBat += _T(" -reindex=1");

	if(!CreateProcessA(NULL,(LPSTR)(startBat.c_str()),NULL,NULL,FALSE,0,NULL,NULL,&si,&sever_pi)){
		int n = GetLastError();
		AfxMessageBox(_T("开启服务端程序失败\n"));
		exit(1);  
	}
	CloseHandle(sever_pi.hProcess);
	CloseHandle(sever_pi.hThread);
}

void CpolopointclientApp::CloseProcess(const string& exename)
{
	HANDLE SnapShot,ProcessHandle;  
	SHFILEINFO shSmall;  
	PROCESSENTRY32 ProcessInfo;   
	int count  = 0;
	while(TRUE) 
	{
		SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);  
		if(SnapShot!=NULL) {
			ProcessInfo.dwSize=sizeof(ProcessInfo);
			BOOL Status=Process32First(SnapShot,&ProcessInfo); 
			while(Status)
			{
				SHGetFileInfo(ProcessInfo.szExeFile,0,&shSmall, sizeof(shSmall),SHGFI_ICON|SHGFI_SMALLICON);
				USES_CONVERSION;
				CString str(ProcessInfo.szExeFile);
				char* pData = str.GetBuffer(str.GetLength());
				str.ReleaseBuffer();
				strlwr(pData);
				for(int i=0;i<3;i++)
					if(!strcmp(exename.c_str(),str)){
						ProcessHandle=OpenProcess(PROCESS_ALL_ACCESS,FALSE,ProcessInfo.th32ProcessID);  
						TerminateProcess(ProcessHandle,0);  
						return; 
					}
				    Status=Process32Next(SnapShot,&ProcessInfo);  
			}
			break;
		}
		Sleep(1000);  
	}
}

//todo 判断钱包是否已加密
bool CpolopointclientApp::IsLockWallet(){
	
	if (m_bIsPassLock)
	{
		while(m_bIsWalletLocked)
		{
			CWalletPassPhrase walletpassdlg;
			if (walletpassdlg.DoModal() == IDCANCEL)
			{
				break;
			}
			Sleep(100);
		}
	}

	return m_bIsWalletLocked;

}


void CpolopointclientApp::OnCheckSeverfile()
{
	string strSeverPath = strprintf("%s\\polopointsd.exe", m_strInsPath);

	if (PathIsDirectory(strSeverPath.c_str()))
	{
		::CreateDirectory(strSeverPath.c_str(), NULL);
		::RemoveDirectory(strSeverPath.c_str());
		if( (_access( strSeverPath.c_str(), 0 )) == -1 )
		{
			UiFun::MessageBoxEx(_T("可执行polopointsd.exe文件不存在,请重新启动钱包\r\n") , _T("Error") ,MFB_OK|MFB_ERROR );
			exit(1);
		}
	}

	strSeverPath = strprintf("%s\\temp\\polopointsd.exe",m_strInsPath);

	if (PathIsDirectory(strSeverPath.c_str()))
	{
		::CreateDirectory(strSeverPath.c_str(), NULL);
		::RemoveDirectory(strSeverPath.c_str());
	}
	//todo 升级文件检查
	///// 判断文件是否存在
	//if( (_access( strSeverPath.c_str(), 0 )) == -1 )
	//{
	//	UiFun::MessageBoxEx(_T("polopointsd 文件不存在,请重新下载\r\n") , _T("Error") ,MFB_OK|MFB_ERROR );
	//	exit(1);
	//}
}

//校验路径合法性
void CpolopointclientApp::CheckPathValid(const string& strDir)
{
	BOOL bExist = FALSE;
	for(int i = 0;i < (int)strDir.length();i++)
	{
		BYTE bchar = (BYTE)strDir.at(i);
		if(bchar == ' ')
		{
			bExist = TRUE;
			break;
		}
	}

	if (bExist)
	{
		UiFun::MessageBoxEx("程序不可以放在含有空格的目录下\r\n" , _T("Error") ,MFB_OK|MFB_ERROR );
		exit(0);
	}
}


void CpolopointclientApp::StartWorkThread() 
{
	//接收服务端发送过来message，并将其保存在
	m_hReceiveThread = (HANDLE)_beginthreadex(NULL,	0,	ReceiveMessageFromServer, this, 0, &m_uReceiveThreadId);
	//解析消息为实际格式并保存在消息队列中
	m_hPraseJsonToMsgThread = (HANDLE)_beginthreadex(NULL,	0,	PraseJsonToMessage, this, 0, &m_uPraseJsonToMsgThreadId);
	//根据不同消息，做出不同响应
	m_hProcessMsgThread = (HANDLE)_beginthreadex(NULL,	0,	ProcessMessage, this, 0, &m_uProcessMsgThreadId);
	//检查客户端是否有更新
	m_hCheckUpdateThread = (HANDLE)_beginthreadex(NULL,0, CheckUpdate, this, 0, &m_uCheckUpDateThreadId);
}


//校验json字符串的格式是否正确
bool JsonCheck(string strjson){

	strjson =  UiFun::trimleft(strjson);
	strjson =  UiFun::trimright(strjson);
	if (strjson.at(0) != '{')
	{
		return false;
	}
	int len = strjson.length();
	if (strjson.at(len-1) != '}')
	{
		return false;
	}
	return true;
}

UINT __stdcall CpolopointclientApp::ReceiveMessageFromServer(LPVOID pParam)
{
	bool bReConnect(false);
	int nMaxbufferLen(10*1024-1);
	CpolopointclientApp * pPolopointsApp  = (CpolopointclientApp*)pParam ;
	if ( NULL != pPolopointsApp ) {
		while (true)
		{
			Sleep(100);
			if (theApp.m_bIsCloseAppFlag)
			{
				return 1;
			}
			if(bReConnect) {

				CSynchronousSocket te;
				SOCKET nSocket = te.OnblockConnnect((char*)pPolopointsApp->m_severIp.c_str(),atoi(pPolopointsApp->m_uiPort.c_str()));
				if( INVALID_SOCKET != nSocket) {
					pPolopointsApp->m_uiConnSocket = nSocket;
					bReConnect = false;
					TRACE("retry nSocket OK\n");
				}
				else
				{
				TRACE("retry nSocket Error\n");
				}
			}
			else {
				if ( INVALID_SOCKET != pPolopointsApp->m_uiConnSocket ) {
					char  cRecvbuffer[10*1024];
					int nRecLen = recv( pPolopointsApp->m_uiConnSocket , cRecvbuffer , nMaxbufferLen , 0);

					if ( nRecLen > 0 ) {
						/*TRACE("recv len %d:",nRecLen);
						for(int i=0; i< nRecLen; ++i) {
							TRACE("%02X",cRecvbuffer[i]);
						}
						TRACE("\n");*/
						if(!pPolopointsApp->m_noUiMsgBuffer.AddBytesToBuffer(cRecvbuffer, nRecLen)) {
							if (INVALID_SOCKET != pPolopointsApp->m_uiConnSocket)
							{
								closesocket(pPolopointsApp->m_uiConnSocket);
								pPolopointsApp->m_uiConnSocket = INVALID_SOCKET;
							}
							pPolopointsApp->m_noUiMsgBuffer.ClearBuffer();
							bReConnect = true;
						}
					}
					else if(nRecLen == 0) 
					{
						TRACE0("noui socket has been closed\n");
						if (INVALID_SOCKET != pPolopointsApp->m_uiConnSocket)
						{
							closesocket(pPolopointsApp->m_uiConnSocket);
							pPolopointsApp->m_uiConnSocket = INVALID_SOCKET;
						}
						bReConnect = true;
					}
					else if(nRecLen < 0) 
					{
						 TRACE1("recv failed: %d\n", WSAGetLastError());
						 if (INVALID_SOCKET != pPolopointsApp->m_uiConnSocket)
						 {
							 closesocket(pPolopointsApp->m_uiConnSocket);
							 pPolopointsApp->m_uiConnSocket = INVALID_SOCKET;
						 }
						 bReConnect = true;
					}
				}
				else
				{
						bReConnect = true;
				}
			}
		}
	}
	return 1 ;
}

int GetMsgType(string const m_strData,Json::Value &root)
{
	string m_strType;
	int pos = m_strData.find("type");
	if ( pos >=0 ){
		m_strType =root["type"].asString();
		if ( !strcmp(m_strType.c_str() ,_T("init")))			 //初始化钱包界面
		{
			return ININTAL_TYPE;
		}
		if ( !strcmp(m_strType.c_str() ,_T("revtransaction")))   //服务端收到与本钱包地址相关交易
		{
			return REV_TRANSATION_TYPE;
		}
		if ( !strcmp(m_strType.c_str() ,_T("blockchanged")))     //区块链块发生改变
		{
			return BLOCK_CHANGE_TYPE;
		}
		if ( !strcmp(m_strType.c_str() ,_T("rev_app_transaction"))) //服务端收到与本地钱包相关合约交易
		{
			return APP_TRANSATION_TYPE;
		}
		if(!strcmp(m_strType.c_str(), _T("notify")))             //服务端发送给钱包界面的通知消息
		{
			return SERVER_NOTIYF_TYPE;
		}
		if(!strcmp(m_strType.c_str(), _T("SyncTx")))             //与服务端同步最近100个块的交易
		{
			return SERVER_SYNC_TX;
		}
		if(!strcmp(m_strType.c_str(), _T("releasetx")))          //服务端将交易从区块中释放，通知到界面端消息
		{
			return RELEASE_TX;
		}
		if(!strcmp(m_strType.c_str(), _T("rmtx")))               //服务端将不合法的交易从mempool中清除，通知到界面端消息 
		{
			return REMOVE_TX;
		}
		if(!strcmp(m_strType.c_str(), _T("SyncTxHight")))        //服务端与钱包界面同步高度 
		{
			return SYSTX_HEIGHT;
		}
	}
	return  -1;
}


bool ProcessMsgJson(Json::Value &msgValue, CpolopointclientApp* pApp) 
{
	string objstr = msgValue.toStyledString();
	int type = GetMsgType(objstr.c_str(),msgValue);
	LogPrint("RECIVEUI", "MESG:%s\n",objstr.c_str());
	switch(type)
	{
	case ININTAL_TYPE:
		{
			string msg = msgValue["msg"].asString();
			TRACE("MEST:%s\r\n",msg.c_str());
			if (!strcmp(msg.c_str(),"Verifying blocks..."))
			{
				CPostMsg postmsg(MSG_USER_STARTPROCESS_UI,1);
				pApp->m_msgQueue.push(postmsg);
			}
			else if (!strcmp(msg.c_str(),"Verifying Finished"))
			{
				CPostMsg postmsg(MSG_USER_STARTPROCESS_UI,2);
				pApp->m_msgQueue.push(postmsg);
			}
			else if (!strcmp(msg.c_str(),"Loading addresses..."))
			{
				CPostMsg postmsg(MSG_USER_STARTPROCESS_UI,3);
				pApp->m_msgQueue.push(postmsg);
			}
			else if (!strcmp(msg.c_str(),"initialize end"))
			{
				CPostMsg postmsg(MSG_USER_STARTPROCESS_UI,4);
				pApp->m_msgQueue.push(postmsg);				
			}
//			postmsg.SetData(msg);
//			pApp->m_msgQueue.push(postmsg);
//		TRACE("type: %s   mag: %s\r\n" , postmsg.Getm_strType() ,msg);
			break;
		}
	case SERVER_SYNC_TX:
		{

			Json::Value obj =msgValue["msg"]; 
			//插入到数据库
			string txData ;
			txData =strprintf("%s" , obj.toStyledString().c_str() );
			CPostMsg postmsg(MSG_USER_GET_UPDATABASE,WM_SYNC_TRANSACTION);
			postmsg.SetData(txData);
			pApp->m_msgQueue.push(postmsg);
			
		}
		break;
	case SYSTX_HEIGHT:
		{
			Json::Value obj =msgValue["msg"]; 
			//插入到数据库
			if (!obj.isNull())
			{
				int height = obj["syncheight"].asInt();
				pApp->m_nSyncHight = height;
				pApp->DeleteItemTransaction(height);
			}
		}
		break;
	case REV_TRANSATION_TYPE:
		{

			SYSTEMTIME curTime ;
			memset( &curTime , 0 , sizeof(SYSTEMTIME) ) ;
			GetLocalTime( &curTime ) ;
			static int RecivetxtxTimeLast =0;
			int tempTimemsg= (int)UiFun::SystemTimeToTimet(curTime);
			/// 更新钱包
			CPostMsg postuimsg(MSG_USER_GET_UPDATABASE,WM_UP_ADDRESS);
			if ((tempTimemsg - RecivetxtxTimeLast)>10 || RecivetxtxTimeLast == 0)
			{	
				pApp->m_msgQueue.push(postuimsg);
				RecivetxtxTimeLast = tempTimemsg;
			}

		//	LogPrint("INFO", "REV_TRANSATION %s\n",msgValue.toStyledString().c_str());
			const Json::Value& txArray = msgValue["transation"]; 
			//插入到数据库
			string strHash =txArray["hash"].asString();
			CPostMsg postmsg(MSG_USER_GET_UPDATABASE,WM_REVTRANSACTION);
			postmsg.SetData(strHash);
			pApp->m_msgQueue.push(postmsg);

			}
			break;
		case APP_TRANSATION_TYPE:
		 {
			 Json::Value obj =msgValue["transation"]; 
			 CPostMsg postmsg(MSG_USER_GET_UPDATABASE,WM_APP_TRANSATION);
			 postmsg.SetData(obj.toStyledString().c_str());
			 pApp->m_msgQueue.push(postmsg);
		 }
		  break;
	case RELEASE_TX:
		{
			//插入到数据库
			string strHash =  msgValue["hash"].asString() ;
			CPostMsg postmsg(MSG_USER_GET_UPDATABASE,WM_RELEASETX);
			postmsg.SetData(strHash);
			pApp->m_msgQueue.push(postmsg);
		}
		break;
	case REMOVE_TX:
		{
			//插入到数据库
			string strHash = msgValue["hash"].asString() ;
	
			CPostMsg postmsg(MSG_USER_GET_UPDATABASE,WM_REMOVETX);
			postmsg.SetData(strHash);
			pApp->m_msgQueue.push(postmsg);
		}
		break;
	case BLOCK_CHANGE_TYPE:
		{
			SYSTEMTIME curTime ;
			memset( &curTime , 0 , sizeof(SYSTEMTIME) ) ;
			GetLocalTime( &curTime ) ;
			static int RecivetxMsgTimeLast =0;
			int tempTimemsg=(unsigned int) UiFun::SystemTimeToTimet(curTime);
		
			TRACE("change:%s\r\n","blockchanged");
			uistruct::BLOCKCHANGED_t      m_Blockchanged ;
			m_Blockchanged.type = msgValue["type"].asString();
			m_Blockchanged.tips = msgValue["tips"].asInt();
			m_Blockchanged.high = msgValue["high"].asInt64() ;
			m_Blockchanged.hash = msgValue["hash"].asString();
			m_Blockchanged.connections = msgValue["connections"].asInt();
			m_Blockchanged.fuelrate = msgValue["fuelrate"].asInt();

			string strJson = m_Blockchanged.ToJson();
			CPostMsg postmsg(MSG_USER_UP_PROGRESS,0);
			postmsg.SetData(strJson.c_str());

			pApp->m_msgQueue.push(postmsg);
			
			
		//	if ((tempTimemsg - RecivetxMsgTimeLast)>10 || RecivetxMsgTimeLast == 0)
			if(m_Blockchanged.high == m_Blockchanged.tips)
			{	
				/// 更新tipblock hash
				CPostMsg postblockmsg(MSG_USER_GET_UPDATABASE,WM_UP_BlLOCKTIP);
				string msg = msgValue.toStyledString();
				postblockmsg.SetData(msg);
				pApp->m_msgQueue.push(postblockmsg);  

			
				/// 更新钱包地址
				CPostMsg postuimsg(MSG_USER_GET_UPDATABASE,WM_UP_ADDRESS);
				pApp->m_msgQueue.push(postuimsg);

				RecivetxMsgTimeLast = tempTimemsg;
			}
			break;
		}
	case SERVER_NOTIYF_TYPE:
		{
			string msg = msgValue["msg"].asString();
			TRACE("MEST:%s\r\n",msg.c_str());
			if (!strcmp(msg.c_str(),"server closed"))
			{
				theApp.m_bIsServerClose = false;
			}else if (!strcmp(msg.c_str(),"Lock"))
			{
				CPostMsg postmsg(MSG_USER_UP_PROGRESS,WM_LOCKSTATE);
				postmsg.Setm_strType(msg);
				pApp->m_msgQueue.push(postmsg);
				pApp->m_bIsWalletLocked = TRUE;
				pApp->m_nLockStatus =2;

				//// 更新block
				CPostMsg postblockmsg(MSG_USER_GET_UPDATABASE,WM_UP_UPDATABlLOCK);
				pApp->m_msgQueue.push(postblockmsg);  
			}else if (!strcmp(msg.c_str(),"UnLock"))
			{
				CPostMsg postmsg(MSG_USER_UP_PROGRESS,WM_LOCKSTATE);
				postmsg.Setm_strType(msg);
				pApp->m_msgQueue.push(postmsg);
				pApp->m_bIsWalletLocked = FALSE;
				pApp->m_nLockStatus =1;

				//// 更新block
				CPostMsg postblockmsg(MSG_USER_GET_UPDATABASE,WM_UP_UPDATABlLOCK);
				pApp->m_msgQueue.push(postblockmsg);  
			}else if (msg.find("connections")>=0)
			{
				CPostMsg postmsg(MSG_USER_UP_PROGRESS,WM_CONNECTNET);
				postmsg.Setm_strType(msg);
				pApp->m_msgQueue.push(postmsg);
			}
			//TRACE("type: %s   mag: %s\r\n" , postmsg.Getm_strType() ,msg);
			break;
		}
	default:
		break;
	}
	return true;
}

UINT __stdcall CpolopointclientApp::PraseJsonToMessage(LPVOID pParam)
{
	CpolopointclientApp * pPolopointsApp  = (CpolopointclientApp*)pParam;
	if ( NULL != pPolopointsApp ) {
		while (true)
		{
		    Sleep(100); 
			if (theApp.m_bIsCloseAppFlag && !pPolopointsApp->m_noUiMsgBuffer.HaveNoUiMsg())
			{
				return 1;
			}
			while(pPolopointsApp->m_noUiMsgBuffer.HaveNoUiMsg()) {
				string strMsg;
				pPolopointsApp->m_noUiMsgBuffer.GetNoUiMsg(strMsg);
				Json::Reader reader;  
				Json::Value jsonValue; 
				if (!JsonCheck(strMsg))
				{
					TRACE("JsonCheck noui msg error,msg content:%s\n", strMsg.c_str());
					continue;
				}
				if (strMsg == _T(""))
				{
					continue;
				}
				if (!reader.parse(strMsg, jsonValue)) 
					continue;
				ProcessMsgJson(jsonValue, pPolopointsApp);
			}
		}
	}
	return 1;
}

bool CpolopointclientApp::Update()
{
			CString sPath;
			sPath.Format(_T("%s\\update.exe"),m_strInsPath.c_str());
			sPath.Replace("\\\\","\\");
			CFileFind find;
			if(!find.FindFile(sPath)) return false;
			SHELLEXECUTEINFO ShRun = {0}; 
			ShRun.cbSize = sizeof(SHELLEXECUTEINFO); 
			ShRun.fMask = SEE_MASK_NOCLOSEPROCESS; 
			ShRun.hwnd = NULL; 
			ShRun.lpVerb = NULL; 
			ShRun.lpFile = sPath;
			ShRun.lpParameters = _T("-checkforupdates"); 
			ShRun.nShow = SW_HIDE; 
			ShRun.hInstApp = NULL; 
			ShellExecuteEx(&ShRun); 
			WaitForSingleObject(ShRun.hProcess, 120000); 
			ULONG lResult = 0; 
			if (!GetExitCodeProcess(ShRun.hProcess, &lResult)) return false;
			if (lResult == 0) return false;
			CString sMsg;
			sMsg.Format(_T("检查有%d文件需要更新,现在是否要更新?"),lResult);
			
		/*	if ( IDYES != UiFun::MessageBoxEx(sMsg, UiFun::UI_LoadString("COMM_MODULE" , "COMM_TIP" ,theApp.gsLanguage) , MFB_YESNO|MFB_TIP ) ){
				((ChuobiDlg*)(pApp->m_pMainWnd))->ClosWalletWind();
				return false;
			}*/

			if(m_pMainWnd != NULL)
				SendMessage(((CpolopointclientDlg*)(m_pMainWnd))->m_hWnd,MSG_USER_CLOSEPROCESS,NULL,NULL);
			CloseProcess("polopointsd.exe");
			DWORD exc;
			m_bIsProcessMsgRunSwitch = false;
			m_bIsReceiveMsgRunSwitch = false;
			m_bIsUpdateCheckRunSwitch = false; 
			closesocket(theApp.m_uiConnSocket);
			/// 等待处理消息进程退出
			while( ::GetExitCodeThread( theApp.m_hProcessMsgThread , &exc ) ) {

				if( STILL_ACTIVE == exc ) {
					;
				}else {
					TRACE( "EXC = %d \n" , exc ) ;
					break;
				}
				Sleep(100);
			}
			ShRun.lpParameters = NULL; 
			ShRun.nShow = SW_SHOW;
			ShellExecuteEx(&ShRun);
			CloseProcess("polopointclient.exe");
			return true;
}


UINT __stdcall CpolopointclientApp::CheckUpdate(LPVOID lpParam){

	CpolopointclientApp * pApp  = (CpolopointclientApp*)lpParam; 

	while(TRUE)
	{
		///一分钟
		static int nCount = 0;
		Sleep(1);
		if (theApp.m_bIsCloseAppFlag)
		{
			return 1;
		}

		nCount++;
		if(nCount == 30000 ){
			nCount =0;
			if(false == pApp->Update())
				continue;
	
		}
	}
	return true;
}

UINT __stdcall CpolopointclientApp::ProcessMessage(LPVOID pParam) {
	CpolopointclientApp * pApp  = (CpolopointclientApp*)pParam ;
	
	while ( true)
	{
		CPostMsg Postmsg ;
		/// 消息处理完 才能够退出
		if (!theApp.m_bIsProcessMsgRunSwitch && !pApp->m_msgQueue.pop(Postmsg))
		{
			return 1;
		}

		if(!pApp->m_msgQueue.pop(Postmsg)){
			Sleep(100); 
			continue;
		}
		CpolopointclientDlg *pAppDlg = (CpolopointclientDlg*)(pApp->m_pMainWnd);
		if ((pAppDlg == NULL || !theApp.m_bIsDlgCreateFinished) && Postmsg.GetUItype() != MSG_USER_STARTPROCESS_UI&&Postmsg.GetDatatype() !=WM_SYNC_TRANSACTION){
			pApp->m_msgQueue.push(Postmsg);
			Sleep(100); 
			continue;
		}
		switch (Postmsg.GetUItype() )
		{
		case MSG_USER_STARTPROCESS_UI:
			{
				LogPrint("PROCESSMSG", "MSG_USER_STARTPROCESS_UI 第:%d\n",Postmsg.GetDatatype());
				theApp.DispatchMsg( theApp.GetMtHthrdId() , MSG_USER_STARTPROCESS_UI ,Postmsg.GetDatatype(),0);
				if (Postmsg.GetDatatype() == 4)
				{
					theApp.m_bIsStartMainDlg = true;
				}
				if(theApp.m_bIsSyncTx){
					theApp.m_bIsSyncTx = FALSE;
					theApp.m_bIsCommitDb =theApp.m_SqliteDeal.CommitDbTransaction();
					TRACE("Sync Tx commit transaction\n");
					LogPrint("INFO", "Sync Tx commit transaction\n");
				}
			}
			break;
		case MSG_USER_GET_UPDATABASE:
			{
			
				switch (Postmsg.GetDatatype())
				{
				case WM_UP_ADDRESS:
					{
						LogPrint("PROCESSMSG", "WM_UP_ADDRESS\n");
						//更新钱包地址数据库
						pApp->UpdateAddressData();
					}
					break;
				case WM_SYNC_TRANSACTION:
					{
						
						if(!theApp.m_bIsSyncTx) {
							theApp.m_bIsSyncTx = TRUE;
							theApp.m_SqliteDeal.BeginDBTransaction();
						}
						string txData = Postmsg.GetData();
						if ( "" != txData ) {
							pApp->SyncTransaction(txData) ;
						}
						LogPrint("PROCESSMSG", "WM_SYNC_TRANSACTION 启动同步交易:%s\n",txData.c_str());
					}
					break;
				case WM_REVTRANSACTION:
					{

						//更新历史交易记录数据库
						string pHash = Postmsg.GetData();
						if ( "" != pHash ) {
							string strCondition("");
							strCondition = strprintf(" hash = '%s' ", pHash.c_str());
							int nItem =  pApp->m_SqliteDeal.GetTableCountItem(_T("t_transaction") ,strCondition);
							if (nItem == 0)
							{
								pApp->InsertTransaction(pHash) ;
							}else{
								pApp->UpdateTransaction(pHash);
							}
						}
						LogPrint("PROCESSMSG", "WM_REVTRANSACTION 收取跟钱包有关的交易:%s\n",pHash.c_str());
					}
					break;
				case WM_APP_TRANSATION:
					{
						string txDetail = Postmsg.GetData();
						if ( "" != txDetail ) {
							pApp->UpdateAppRecord(txDetail);
							pApp->InsertAppTransaction(txDetail);
						}
						LogPrint("PROCESSMSG", "WM_APP_TRANSATION 收取跟钱包有关的应用交易:%s\n",txDetail.c_str());
					}
					break;
				case WM_UP_BlLOCKTIP:
					{
						LogPrint("PROCESSMSG", "WM_UP_BlLOCKTIP 更新block最新高度\n");
						//更新最新blocktip数据库
						string data = Postmsg.GetData();
						if (data == "")
						{
							break;
						}
						if ( pApp->m_SqliteDeal.ClearTableData(_T("t_chain_tip") ) ) {
							uistruct::BLOCKCHANGED_t block;
							if (block.JsonToStruct(data))
							{
								string strinsert;
								strinsert = strprintf("'%s','%d','%d','%d','%d'",block.hash, block.high, block.time, block.fuelrate, pApp->m_nLockStatus);
								if (!pApp->m_SqliteDeal.InsertTableItem(_T("t_chain_tip") ,strinsert ))
								{
									TRACE("insert into table t_chain_tip error");
								}
							}
						}
					}
					break;
				case WM_UP_UPDATABlLOCK:
					{
						uistruct::CHAIN_TIP_T chainblock;
						pApp->m_SqliteDeal.GetTipBlockHash(&chainblock);
						string strWhere =strprintf("block_hash='%s'",chainblock.blockhash);
						string updatastr = strprintf("lock=%d",pApp->m_nLockStatus);
						if (!pApp->m_SqliteDeal.UpdateTableItem(_T("t_chain_tip") ,updatastr,strWhere ))
						{
							TRACE("insert into table t_chain_tip error");
						}
					}
					break;
				case WM_UP_ADDRBOOK:
					{
						LogPrint("PROCESSMSG", "WM_UP_ADDRBOOK 更新地址簿数据库\n");
						string josnaddr = Postmsg.GetData();
						uistruct::ADDRBOOK_t addr;
						if (addr.JsonToStruct(josnaddr))
						{
							string strCond;
							strCond = strprintf(" address='%s' ", addr.address.c_str());
							
							int nItem =  pApp->m_SqliteDeal.GetTableCountItem(_T("t_address_book"), strCond);
							if (0 == nItem)
							{
								pApp->InsertAddrBook(addr) ;
							}else{
								pApp->UpdateAddrBook(addr);
							}
						}
					}
					break;
				case WM_UP_DELETERBOOK:
					{
						LogPrint("PROCESSMSG", "WM_UP_DELETERBOOK 删除地址簿数据库\n");
						string josnaddr = Postmsg.GetData();
						uistruct::ADDRBOOK_t addr;
						if (addr.JsonToStruct(josnaddr))
						{
							string strCond;
							strCond = strprintf(" address='%s' ", addr.address.c_str());
							pApp->m_SqliteDeal.DeleteTableItem(_T("t_address_book"),strCond);
						}
					}
					break;
				case WM_RELEASETX:
					{
						LogPrint("PROCESSMSG", "WM_RELEASETX 收到交易重新放到mempool\n");
						//更新历史交易记录数据库
						string pHash = Postmsg.GetData();
						if ( _T("") != pHash ) {
		
							string strCondition("");
							strCondition = strprintf(" hash = '%s' ", pHash.c_str());
							int nItem =  pApp->m_SqliteDeal.GetTableCountItem(_T("t_transaction") ,strCondition);
							if (nItem != 0)
							{
								string updataValue="confirm_height=0,confirmed_time=0,block_hash=''";
								LogPrint("INFO","WM_RELEASETX:%s",pHash.c_str());
								pApp->m_SqliteDeal.UpdateTableItem(_T("t_transaction") , strCondition , updataValue ) ;
								nItem =  pApp->m_SqliteDeal.GetTableCountItem(_T("t_app_transaction") ,strCondition);
								if (nItem != 0)
								{
									pApp->m_SqliteDeal.UpdateTableItem(_T("t_app_transaction") , strCondition , updataValue ) ;
								}
								theApp.m_SqliteDeal.UpdataAllTableData();   /// 更新应用表格
							}
						}
					}
					break;
				case WM_REMOVETX:
					{
						string pHash = Postmsg.GetData();
						if ( _T("") != pHash ) {

							string strCondition(_T(""));
							strCondition = strprintf(" hash = '%s' ", pHash.c_str());
							int nItem =  pApp->m_SqliteDeal.GetTableCountItem(_T("t_transaction") ,strCondition);
							if (nItem != 0)
							{
								LogPrint("INFO","WM_REMOVETX:%s",pHash.c_str());
								pApp->m_SqliteDeal.DeleteTableItem(_T("t_transaction"),strCondition);
								///删除跟app相关的交易
								pApp->m_SqliteDeal.DeleteTableItem(_T("t_app_transaction"),strCondition);
								theApp.m_SqliteDeal.UpdataAllTableData();   /// 更新应用表格
							}
						}
						/// 通知显示交易界面
						CPostMsg Postmsg(MSG_USER_TRANSRECORD_UI,WM_REMOVETX);
						pApp->m_msgQueue.push(Postmsg);
						LogPrint("PROCESSMSG", "WM_REMOVETX 删除交易:%s\n",pHash.c_str());
					}
					break;
				default:
					break;
				}
			}
			break;
		case MSG_USER_UP_PROGRESS:
			{
				switch(Postmsg.GetDatatype())
				{
				case WM_LOCKSTATE:
					{
						LogPrint("PROCESSMSG", "MSG_USER_UP_PROGRESS WM_LOCKSTATE 锁的消息\n");
						pApp->m_msgLockStatusQueue.push(Postmsg);
					}
					break;
				case WM_CONNECTNET:
					{
						LogPrint("PROCESSMSG", "MSG_USER_UP_PROGRESS WM_CONNECTNET 网络连接消息\n");
						pApp->m_msgConnCountQueue.push(Postmsg);
					}
					break;
				default:
					{
						TRACE("change:%s\r\n","MSG_USER_UP_PROGRESS");
						
						pApp->m_msgProcessBarQueue.push(Postmsg);
						LogPrint("PROCESSMSG", "MSG_USER_UP_PROGRESS  更新进度条消息:%s\n",Postmsg.GetData().c_str());
					}
					break;
				}	
			}
			break;
		case MSG_USER_INSERT_DATA:   //// 插入数据
			{
				LogPrint("PROCESSMSG", "MSG_USER_INSERT_DATA 插入数据消息\n");
				uistruct::DATABASEINFO_t   pDatabase; // = (uistruct::DATABASEINFO_t *)Postmsg.GetStrPoint();
				string strTemp = Postmsg.GetData();
				pDatabase.JsonToStruct(strTemp.c_str());
				pApp->m_SqliteDeal.InsertTableItem(pDatabase.strTabName.c_str() ,pDatabase.strSource.c_str()) ;
				if ( !strcmp(pDatabase.strTabName.c_str() , _T("t_p2p_quiz")) ){
					Postmsg.SetType(MSG_USER_INSERT_DATA,WM_P2P_BET_RECORD);
				}else if (!strcmp(pDatabase.strTabName.c_str() , _T("t_dark_record")) )
				{
					Postmsg.SetType(MSG_USER_INSERT_DATA,WM_DARK_RECORD);
				}else if (!strcmp(pDatabase.strTabName.c_str() , _T("t_wallet_address")))
				{
					uistruct::LISTADDR_t addr; 
					addr.JsonToStruct(pDatabase.strcutjson.c_str());
					string Temp = addr.ToJson();
					pApp->SendUIMessage((int)WM_UP_NEWADDRESS,Temp.c_str());
					Postmsg.SetType(MSG_USER_INSERT_DATA,WM_UP_ADDRESS);
				}

			}
			break;
		case MSG_USER_UPDATA_DATA:   /// 更新数据库
			{
					LogPrint("PROCESSMSG", "MSG_USER_UPDATA_DATA 更新数据消息\n");
				uistruct::DATABASEINFO_t pDatabase;// = (uistruct::DATABASEINFO_t *)Postmsg.GetStrPoint();
				string strTemp = Postmsg.GetData();
				pDatabase.JsonToStruct(strTemp);

				if ( !pApp->m_SqliteDeal.UpdateTableItem(pDatabase.strTabName.c_str() , pDatabase.strSource.c_str() , pDatabase.strWhere.c_str() ) ){
					TRACE(_T("t_p2p_quiz数据更新失败!") );
				}

				if ( !strcmp(pDatabase.strTabName.c_str() , _T("t_p2p_quiz")) ){
					Postmsg.SetType(MSG_USER_UPDATA_DATA,WM_P2P_BET_RECORD);
				}else if (!strcmp(pDatabase.strTabName.c_str() , _T("t_dark_record")) )
				{
					Postmsg.SetType(MSG_USER_UPDATA_DATA,WM_DARK_RECORD);
				}

			}
			break;
		default:
			break;
		}
		if (theApp.SendPostThread(Postmsg.GetDatatype()) == 0)
		{
			pApp->DispatchMsg( pApp->GetMtHthrdId() , Postmsg.GetUItype(), Postmsg.GetDatatype() , 0) ;
		}
		
	}
	return 1 ;
}


void CpolopointclientApp::SendUIMessage(int message, string data)
{
	CPostMsg Postmsg(MSG_USER_MAIN_UI, message);
	Postmsg.SetData(data);	
	m_msgUIRecDlgQueue.push(Postmsg);
	m_msgUISendDlgQueue.push(Postmsg);

	CPostMsg msg(MSG_USER_RECIVE_UI, message);
	m_msgQueue.push(msg);

	CPostMsg msg1(MSG_USER_SEND_UI, message);
	m_msgQueue.push(msg1);
}


int CpolopointclientApp::SendPostThread(DWORD msgtype)
{
	CpolopointclientDlg *pDlg = (CpolopointclientDlg*)(((CpolopointclientApp*)this)->m_pMainWnd) ;
	if (pDlg == NULL)
	{
		return 0 ;
	}

	switch (msgtype)
	{
	case WM_REVTRANSACTION:
		{
			GetMainDlgStruct();
			//DispatchMsg( theApp.GetMtHthrdId() , MSG_USER_MAIN_UI , 0,0);

		}
		break;
	case WM_UPWALLET:
		{
			DispatchMsg( theApp.GetMtHthrdId() , MSG_USER_MAIN_UI ,WM_UPWALLET,0);
		}
		break;
	default:
		return 0;
	}
	return 1 ;
}

void CpolopointclientApp::GetMainDlgStruct()
{
	uistruct::MINDLG_T maindlg;
	string strCommand,strShowData;
	strCommand = "0";
	double nmoney =  theApp.m_SqliteDeal.GetTableItemSum(_T("t_wallet_address") ,_T("money"), _T(" 1=1 "));
	if (nmoney < 0)
	{
		maindlg.money = "0.0";
	}else{
		string strmoney;
		strmoney = strprintf("%.3lf",nmoney);
		maindlg.money = strmoney;
	}

	string strCond;
	strCond = " confirm_height = 0 ";

	nmoney =  theApp.m_SqliteDeal.GetTableItemSum(_T("t_transaction") , _T("money") , strCond) ;

	if (nmoney <0)
	{
		maindlg.unconfirmmoney = _T("0.0");
	}else{
		CString strmoney;
		strmoney.Format(_T("%.3lf"),nmoney);
		maindlg.unconfirmmoney = strmoney;
	}

	int nItem =  theApp.m_SqliteDeal.GetTableCountItem(_T("t_transaction"), _T(" 1=1 "));

	strCommand = strprintf("%d",nItem);
	maindlg.itemcount = strCommand;

	uistruct::TRANSRECORDLIST pTransaction;
	strCond = " tx_type='COMMON_TX' order by confirmed_time desc limit 6";
	theApp.m_SqliteDeal.GetTransactionList(strCond, &pTransaction);


	int i = 1;
	if (pTransaction.size() != 0  ) {
		std::vector<uistruct::REVTRANSACTION_t>::const_iterator const_it;
		for (const_it = pTransaction.begin(); const_it != pTransaction.end()&&i<7 ; const_it++ ) {
			uistruct::REVTRANSACTION_t temp = *const_it;
			if(i == 1)
			maindlg.addr1 = temp.ToJson();
			if(i == 2)
				maindlg.addr2 = temp.ToJson();
			if(i == 3)
				maindlg.addr3 = temp.ToJson();
			if(i ==4)
				maindlg.addr4 = temp.ToJson();
			if(i == 5)
				maindlg.addr5 = temp.ToJson();
			if(i == 6)
				maindlg.addr6 = temp.ToJson();

			i++;
		}
	}

	CPostMsg Postmsg(MSG_USER_MAIN_UI,0);
	string msg =maindlg.ToJson();
	Postmsg.SetData(msg.c_str());	
	m_msgUIMainDlgQueue.push(Postmsg);

	CPostMsg msg1(MSG_USER_MAIN_UI,0);
	m_msgQueue.push(msg1);
}

void CpolopointclientApp::InsertAppTransaction(string txdetail)
{
	uistruct::REVTRANSACTION_t transcion;
	if (transcion.JsonToStruct(txdetail))
	{
		/// 插入配置的相关的id
		map<string,string>::const_iterator item = m_mapAppId.find(transcion.desregid);
		if (item == m_mapAppId.end())
		{
			return;
		}
		string strSourceData;
		strSourceData = strprintf("'%s' , '%s' ,'%d' ,'%s' ,'%s' , '%.8f' ,'%d' ,'%s' ,'%s', '%.8f' ,'%s' ,'%d','%d','%s'" , transcion.txhash.c_str() ,\
			transcion.txtype.c_str() ,transcion.ver ,transcion.addr.c_str() ,transcion.regid,transcion.fees,transcion.height,\
			transcion.desaddr.c_str(), transcion.desregid.c_str(),transcion.money,transcion.Contract.c_str(),transcion.confirmedHeight,transcion.confirmedtime,transcion.blockhash.c_str()) ;
		m_SqliteDeal.InsertTableItem(_T("t_app_transaction") ,strSourceData ) ;
	}
}

void  CpolopointclientApp::ParseUIConfigFile(const string& strExeDir){
	string configpath = "";
	configpath += strprintf("%s",strExeDir);
	configpath+= strprintf("\\%s","polopointsclient.conf");

	if (PathFileExistsA(configpath.c_str()))
	{
		CJsonConfigHelp::getInstance()->Init();
		CJsonConfigHelp::getInstance()->ReadJsonConfig(configpath);
		/*
		CScriptCfg scriptCfg;
		CJsonConfigHelp::getInstance()->GetScriptCfgData(scriptCfg);
		m_strFreeze5MonthId = scriptCfg.m_strFreeze5MonthAppId;
		m_strFreeze10MonthId= scriptCfg.m_strFreeze10MonthAppId;
		m_strFreeze20MonthId = scriptCfg.m_strFreeze20MonthAppId;
		*/
		

		//CJsonConfigHelp::getInstance()->GetClosConfig(m_reminder);
		//CJsonConfigHelp::getInstance()->GetPopTipsConfig(m_poptips);
		//CJsonConfigHelp::getInstance()->GetNewestScriptData(m_neststcriptid);
		//CJsonConfigHelp::getInstance()->GetP2pBetStep(m_p2pbetstep);
		//CJsonConfigHelp::getInstance()->GetRedPacketStep(m_redPackestep);
		//CJsonConfigHelp::getInstance()->GetListAppConfig(m_listapp);
		//CJsonConfigHelp::getInstance()->GetHelpUrl(helpurlen,helpurlcn);
		CNetParamCfg netParm;
		CJsonConfigHelp::getInstance()->GetClosConfig(m_reminder);
		CJsonConfigHelp::getInstance()->GetPopTipsConfig(m_poptips);
		CJsonConfigHelp::getInstance()->GetNetParamCfgData(netParm);
		CJsonConfigHelp::getInstance()->GetListAppConfig(m_listapp);
		CJsonConfigHelp::getInstance()->GetInsertAppId(m_mapAppId);
		CJsonConfigHelp::getInstance()->GetAppBalance(m_AppBalanceCfg);

		m_severIp = netParm.server_ip;
		m_uiPort = netParm.server_ui_port;
		m_rpcPort = netParm.rpc_port;
		m_rpcUser = netParm.rpc_user;
		m_rpcPassWord = netParm.rpc_password;
	}
}
