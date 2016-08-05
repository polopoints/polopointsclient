#include "StdAfx.h"
#include "HSocket.h"


CHSocket::CHSocket(void)
{
	m_Socket = INVALID_SOCKET ;
	m_nMaxbufferLen = 10*1024-1 ;
	memset(m_cRecvbuffer , 0 , 10*1024) ;
	m_bOnoff = FALSE ;
}
CHSocket::CHSocket(CString strIpAddr , WORD nPort)
{
	m_Socket = INVALID_SOCKET ;
	m_nMaxbufferLen = 10*1024-1 ;
	memset(m_cRecvbuffer , 0 , 10*1024) ;
	m_bOnoff = FALSE ;
	m_strIpAddr = strIpAddr ;
	m_nPort = nPort ;
}

CHSocket::~CHSocket(void)
{
	OnClose();
}
BOOL CHSocket::OnConnnect()
{   
	BOOL bRes = FALSE ;
	struct sockaddr_in servAddr;
	SOCKET hSocket =  INVALID_SOCKET;
	memset(&servAddr, 0, sizeof(struct sockaddr_in));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(m_nPort);
	servAddr.sin_addr.s_addr = inet_addr(m_strIpAddr);  

	if (servAddr.sin_addr.s_addr != INADDR_NONE ){
		if ((hSocket = socket(AF_INET, SOCK_STREAM, 0)) != INVALID_SOCKET){
			if (connect(hSocket, (struct sockaddr *)&servAddr, sizeof(struct sockaddr_in)) == 0){
				m_Socket = hSocket;
				bRes = TRUE ;
			}else{
				int error = WSAGetLastError();
				hSocket = INVALID_SOCKET;
				closesocket(hSocket);
			}
		}
	}
	return bRes ;
}
BOOL CHSocket::OnSend(const char* sCmdBuff , int nLen)
{
	if ( INVALID_SOCKET == m_Socket ||  NULL == sCmdBuff || 0 == nLen ) return FALSE ;

	if (-1 == send(m_Socket, sCmdBuff, nLen, 0))
		return FALSE;

	return TRUE;
}
BOOL CHSocket::OnRecv(BLOCKDATA pBlockDataProc , WPARAM wParam)
{
	BOOL bRes = FALSE ;
	if ( pBlockDataProc != NULL ){
		m_pBlockDataProc = pBlockDataProc ;
		m_pBlockDataParam = wParam ;
		m_hblockThread = (HANDLE)_beginthreadex(NULL,	0,	blockProc, this, 0, &m_ublockThreadId);	
		bRes = TRUE ;
	}
	return bRes ;
}
UINT __stdcall CHSocket::blockProc(LPVOID pParam)
{
	CHSocket * pHSocket = (CHSocket*)pParam ;
	ASSERT(pHSocket) ;
	while (true)
	{
		if ( pHSocket->m_bOnoff ) {  
			break; ;
		}
		Sleep(100);
		if ( pHSocket->m_Socket == INVALID_SOCKET ) {
			if ( pHSocket->OnConnnect()){
				TRACE("连接到服务器失败 , IP:%s   Port:%d" , pHSocket->m_strIpAddr , pHSocket->m_nPort );
			}
		}else{
			int nRecLen = recv( pHSocket->m_Socket , pHSocket->m_cRecvbuffer , pHSocket->m_nMaxbufferLen , 0);
			if ( nRecLen > 0 ) {
				pHSocket->m_pBlockDataProc(pHSocket->m_cRecvbuffer ,nRecLen ,pHSocket->m_pBlockDataParam  );
			}else if(nRecLen == 0) {
				if (INVALID_SOCKET != pHSocket->m_Socket ){
					closesocket(pHSocket->m_Socket);
					pHSocket->m_Socket = INVALID_SOCKET ;
				}
			}else if(nRecLen < 0) {
				if (INVALID_SOCKET != pHSocket->m_Socket ){
					closesocket(pHSocket->m_Socket);
					pHSocket->m_Socket = INVALID_SOCKET ;
				}
			}
		}
	}
	if ( pHSocket->m_Socket !=INVALID_SOCKET ) {
		closesocket(pHSocket->m_Socket);
		pHSocket->m_Socket = INVALID_SOCKET ;
	}
	return 1 ;
}
void CHSocket::OnClose()
{
	m_bOnoff = TRUE ;
	::WaitForSingleObject(m_hblockThread, INFINITE);  
}