#pragma once
#include <memory.h>
#include <string>
#include <deque>
#include "MyMutex.h"
using namespace std;

const int nBufferMaxLength = 1024*1024;
class CNoUiMsgBuffer 
{
public:
	CNoUiMsgBuffer();
	~CNoUiMsgBuffer(){};
	void ClearBuffer();

	bool AddBytesToBuffer(char *pCh, int nSize);
	void GetNoUiMsg(string &strMsg);
	bool HaveNoUiMsg(); 
public:
	char  m_Recvbuffer[nBufferMaxLength];
private:
	CMyMutex        cs_NoUiNotifyMsg;  //noui通知消息临界
	std::deque<string> m_dqNoUiMsg;
	int   m_nLength;
};