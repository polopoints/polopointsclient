
// polopointclient.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CpolopointclientApp:
// 有关此类的实现，请参阅 polopointclient.cpp
//

#include "uiapi.h"
#include "RoundButton.h"
#include "ShadeButtonST.h"
#include "StaticTrans.h"
#include "ListCtrlCl.h"
#include "DialogBase.h"
#include "HSocket.h"
#include "GIFControl.h"
#include <vector>
#include <map>
#include <list>
#include "config.h"
#include "tinyformat.h"
#include "NoUiMsgBuffer.h"
#include "MyQueue.h"
#include "Log.h"
#include "SqliteDeal.h"
#include "JsonConfigHelp.h"
#include "SplashThread.h"


#define strprintf tfm::format

// ChuobiApp:
// 有关此类的实现，请参阅 huobi.cpp
//
using namespace std;
class CpolopointclientApp : public CWinApp
{
public:
	CpolopointclientApp();

	// 重写
public:
	virtual BOOL InitInstance();

	// 实现

	DECLARE_MESSAGE_MAP()
public:
	string					m_strInsPath			;
	string					m_strModuleFilename	;  
	BOOL                    m_bReIndexServer;
	CString                 m_strAddress;
	void   GetMoFilename( string &path , string &filename ) ;		// 获取文件名和路径
public:
	HANDLE	        GetMtHthrd() const { return hMtThrd; }  
	unsigned int	GetMtHthrdId() const { return nMtThrdID; }  
public:
	CRITICAL_SECTION		cs_MsgDisptch ; 
public:
	HANDLE		 hMtThrd				;	
	HANDLE		 hMtStartEvent			;	
	unsigned int nMtThrdID				;	
	BOOL		 CreateMaintainThrd()	;	
	static UINT __stdcall MtProc(LPVOID pParam) ;
public:
	std::vector<MSG_NOTIFY> sSubscribeList ;
	std::vector< sThrd > v_ProcSubThrd ;
public:
	void RegThrdToMtThrd(  HANDLE hThread , unsigned int hThrdId ) ;	
	void UnRegThrdToMtThrd(HANDLE hThread , unsigned int hThrdId ) ;
	void SubscribeMsg( DWORD nThreadId , HWND hWnd , UINT msg , void * relMsgMem = NULL )	;
	void UnSubscribeMsg( DWORD , HWND , UINT )	;
	void DispatchMsg( unsigned int threadID , UINT msg , WPARAM wParam , LPARAM lParam ) ;
public:
	void StartSeverProcess(const string& strdir);
	void CloseProcess(const string& exename);
	PROCESS_INFORMATION		sever_pi; 
	bool Update();

	bool IsLockWallet();                      //钱包是否加密
	void OnCheckSeverfile();                  //检查服务core程序是否存在
	void CheckPathValid(const string& strDir);//校验工作目录是否有空格、中文等非法目录

	void StartWorkThread();											//启动数据处理线程 
	static UINT __stdcall ReceiveMessageFromServer(LPVOID pParam);  //接收消息从服务端
	static UINT __stdcall PraseJsonToMessage(LPVOID pParam);        //解析Json数据为message
	static UINT __stdcall ProcessMessage(LPVOID pParam);            //处理消息
	static UINT __stdcall CheckUpdate(LPVOID pParam);               //检查程序是否有更新

	void ParseUIConfigFile(const string& strExeDir);             //解析界面配置文件
public:

	//数据库操作
	void UpdateAppRecord(string txdetail);						 //更新APP交易状态
	void PopupContactBalloonTip(uistruct::REVTRANSACTION_t tx);  //冒泡弹出合约交易提示
	void InsertTransaction(string hash);						 //新增新的交易
	void PopupCommBalloonTip(string hash);                       //冒泡弹出普通交易提示
	void InsertAddrBook(uistruct::ADDRBOOK_t addr);              //地址簿新增用户地址
	void UpdateAddrBook(uistruct::ADDRBOOK_t addr);              //地址簿更新用户地址
	void SyncTransaction(string obj);                            //同步交易
	void DeleteItemTransaction(int height);                      //删除交易
	void InsertAppTransaction(string txdetail);                  //新增合约交易
	void UpdateAddressData();                                    //更新地址信息
	void UpdateTransaction(string hash);						 //更新交易信息
	BOOL RunOnlyOneApp();

public:
	UINT         m_uReceiveThreadId;        //接收消息线程ID
	HANDLE       m_hReceiveThread;          //接收消息线程句柄

	UINT         m_uPraseJsonToMsgThreadId; //解析Json为消息线程ID
	HANDLE       m_hPraseJsonToMsgThread;   //解析Json为消息线程句柄

	UINT         m_uProcessMsgThreadId;    //处理消息线程ID
	HANDLE       m_hProcessMsgThread;      //处理消息线程句柄

	UINT         m_uCheckUpDateThreadId;   //检查程序更新线程ID
	HANDLE       m_hCheckUpdateThread;     //检查程序更新线程句柄

	BOOL         m_bIsCloseAppFlag;        //程序关闭退出标志
	BOOL         m_bIsServerClose;         //服务器core程序关闭标志
	BOOL         m_bIsWalletLocked;        //钱包加密标志
	BOOL         m_bIsUpdateCheckRunSwitch;//更新检查线程开关标志  
	BOOL		 m_bIsProcessMsgRunSwitch; //处理消息线程开关标志  
	BOOL         m_bIsReceiveMsgRunSwitch; //接收服务端数据线程开关标志
	BOOL         m_bIsHaveLocked;          //钱包是否锁定状态
	BOOL         m_bIsPassLock;

	BOOL	     m_bIsDlgCreateFinished;   //主对话框框的所有dlg是否创建完
	BOOL         m_bIsStartMainDlg;        //是否启动主对话框
	BOOL         m_bIsSyncTx;              //是否同步交易
	BOOL		 m_bIsCommitDb;            //是否提交SQL执行到DB标志
	BOOL	     m_bIsSyncBlock;           //从网络同步block标志

	string		  m_strProgressGifFile;	   //获取网络节点GIF动画文件
	string		  m_strProgressOutGifFile; //退出程序GIF动画文件

	int          m_nNetWorkType;           //网络状态：0-是main网络(正式网络) 1-regtest网络(局域网络) 2-testnet(测试网络) 
	int          m_nLockStatus;            //锁定状态
	CNoUiMsgBuffer m_noUiMsgBuffer;        //接收服务端消息

	CHSocket*    m_pSocket ;			   //Socketrk操作
	string		 m_rpcPort;                //RPC端口
	string		 m_strServerCfgFileName;   //服务器配置文件名
	string		 m_sendPreHeadstr;         //发送RPC命令报文头
	string		 m_sendEndHeadstr;         //发送RPC命令报文尾
	string       m_uiPort;                 //接受服务器推送消息端口
	string       m_severIp;                //访问远程服务器IP
	string       m_rpcUser;                //RPC访问用户名（远程访问时需要）
	string       m_rpcPassWord;			   //RPC访问密码(远程访问时需要)

	SOCKET       m_uiConnSocket;           //UI与服务器端通讯socket
	int			 m_nSyncHight;             //服务端同步给UI端的区块链高度  
	int          m_nBlockTipHight;         //钱包UI端保存的区块tip高度

	//string       m_strFreeze5MonthId;      //冻结5个月应用ID
	//string       m_strFreeze10MonthId;     //冻结10个月应用ID
	//string       m_strFreeze20MonthId;     //冻结20个月应用ID

	//string      m_betScritptid;
	//string      m_redPacketScriptid;
	//string      m_ipoScritptid;

	int blocktipheight;
	bool m_poptips;
	int m_reminder;

	CSplashThread* pSplashThread;

	map<string,string>  m_mapAppId;        //应用ID的map
	//CP2PBetCfg		m_P2PBetCfg;
	CAppBalance     m_AppBalanceCfg;
	map<string,CONFIG_APP_DATA> m_listapp;
public:
	void  SendUIMessage(int message,string jsonaddr);
	void  GetMainDlgStruct();
	int   SendPostThread(DWORD msgtype);    //发送消息到界面
public:
	CMyQueue m_msgQueue;                    //缓存给UI界面处理的消息队列
	CMyQueue m_msgLockStatusQueue;          //钱包锁状态消息队列
	CMyQueue m_msgConnCountQueue;           //网络连接数消息队列
	CMyQueue m_msgProcessBarQueue;          //更新进度条Bar队列
	CMyQueue m_msgUIRecDlgQueue;            //接收界面消息队列
	CMyQueue m_msgUISendDlgQueue;           //发送界面消息队列
	CMyQueue m_msgUITxDetailDlgQueue;       //交易详情对话框消息队列
	CMyQueue m_msgUIMainDlgQueue;           //首页对话框消息队列 


	CSqliteDeal   m_SqliteDeal;             //Sqlite3数据库操作
public:


};


extern CpolopointclientApp theApp;