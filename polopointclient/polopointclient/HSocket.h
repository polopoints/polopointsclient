#pragma once
#include "config.h"
class CHSocket
{
public:
	CHSocket(void);
	CHSocket(CString strIpAddr , WORD nPort );
	~CHSocket(void);
private:
	CString  m_strIpAddr ;
	WORD     m_nPort ;
	SOCKET   m_Socket ;
	int      m_nMaxbufferLen;
	char     m_cRecvbuffer[10*1024];
	BOOL     m_bOnoff;
public:
	BOOL    OnConnnect();   //Á¬½Ó
	BOOL    OnSend(const char* sCmdBuff , int nLen);
	BOOL    OnRecv(BLOCKDATA pBlockDataProc , WPARAM wParam);
	void    OnClose();
	SOCKET  GetSocket();
public:
	HANDLE       m_hblockThread         ;
	UINT         m_ublockThreadId       ;
	void         StartThread()          ;  
	static UINT __stdcall blockProc(LPVOID pParam);
public:
	BLOCKDATA    m_pBlockDataProc;
	WPARAM       m_pBlockDataParam;
};

