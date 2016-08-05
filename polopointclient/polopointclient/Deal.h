#pragma once


// CDeal 对话框
#include "CJFlatComboBox.h"

class CDeal : public CDialogBar
{
	DECLARE_DYNAMIC(CDeal)

public:
	CDeal();   // 标准构造函数
	virtual ~CDeal();

// 对话框数据
	enum { IDD = IDD_DIALOG_DEAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	HBITMAP		     m_pBmp; 
public:
	CListCtrlCl        m_listCtrl;
	CShadeButtonST     m_rBtnTxdetail;
	CShadeButtonST     m_rBtnExportTx;
	CShadeButtonST     m_rBtnSearch;
	CCJFlatComboBox		   m_condition;
	CCJFlatComboBox		   m_time;
	CCJFlatComboBox		   m_pageitem;
	CEdit		       m_edit;
	COLORREF          m_colorEditText;
	bool              m_seteditcolor;
	uistruct::TRANSRECORDLIST m_pListInfo ;
public:
	CStaticTrans           m_sCountpage  ;
	CShadeButtonST         m_rBtnUp;
	CShadeButtonST         m_rBtnNext;
	int                    m_pagesize;
	int                    m_offset;
	int                    m_nConut   ;     //翻页计数
public:
	void		     SetBkBmpNid( UINT nBitmapIn ) ;
	void  OninitializeList();
	void  ShowComboxCotent();
	string GetConditonStr(int &operate);
	string GetConditonTxType(int &operate);
	string GetConditonTime();
	string Getaddr();
	void  ShowPageCotent(int pageIndex);
	void OnShowListCtrl(uistruct::TRANSRECORDLIST pListInfo);
	void ShowAddrConditon();
	void ShowPageDataInfo(int nConut);
	void GetExportCol(int index,map<int,string> &item,uistruct::REVTRANSACTION_t const_it);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL Create(CWnd* pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID);
	afx_msg void OnCbnSelchangeComboTime();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnCbnSelchangeComboPage();
	afx_msg void OnBnClickedUpPage();
	afx_msg void OnBnClickedNextPage();
	afx_msg void OnBnClickedExportExel();
	afx_msg void OnBnClickedButtonTxdetail();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnShowListCtrl(  WPARAM wParam, LPARAM lParam ) ;
	afx_msg void OnBnClickedButtonSearch();
};
