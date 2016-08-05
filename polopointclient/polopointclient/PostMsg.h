#pragma once
#include <string>
using namespace std;
class CPostMsg
{
	int m_nUIType;
    int m_nDataType;
	string m_strType;
	string m_strData;
public:

	CPostMsg(int uitype,int datatype);
	CPostMsg(void);
	~CPostMsg(void);
	void Setm_strType(string const &strTpye);
	void SetType(int uitype,int datatype);
	int GetUItype() const;
	int GetDatatype() const;
	void SetData(string const & data){
		m_strData = data;
	}
	string GetData(){
		return m_strData;
	}
	string GetStrType() const;
public:
	 bool operator==(CPostMsg const&meg)const;
};

