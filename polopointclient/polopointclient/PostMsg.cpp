#include "StdAfx.h"
#include "PostMsg.h"


CPostMsg::CPostMsg(int uitype,int datatype)
{
	 m_nUIType = uitype;
	 m_nDataType= datatype;
	 m_strType = _T("");
	 m_strData = _T("");
}
CPostMsg::CPostMsg(void){
	m_nUIType = 0;
	m_nDataType= 0;
	m_strType = _T("");
	m_strData = _T("");
}

CPostMsg::~CPostMsg(void)
{
	int b =5;
	int c = 6;
}
void CPostMsg::Setm_strType(string const &strTpye){
	m_strType = strTpye;
}
bool CPostMsg::operator==(CPostMsg const&meg)const{
	if (meg.m_nUIType != m_nUIType && meg.m_nDataType !=m_nDataType&&meg.m_strType != m_strType)
	{
		return false;
	}
	if( meg.m_strData != m_strData)
	{
	 return false;
	}
	
	return true;
}
int CPostMsg::GetUItype() const{
	return m_nUIType;
}
int CPostMsg::GetDatatype() const{
	return m_nDataType;
}
string CPostMsg::GetStrType() const{
	return m_strType;
}
void CPostMsg::SetType(int uitype,int datatype){
	m_nUIType = uitype;
	m_nDataType= datatype;
}