#pragma once


// CRece 对话框

class CRece : public CDialogBar
{
	DECLARE_DYNAMIC(CRece)

public:
	CRece();   // 标准构造函数
	virtual ~CRece();

// 对话框数据
	enum { IDD = IDD_DIALOG_RECE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	HBITMAP		     m_pBmp; 
public:
	CListCtrlCl        m_listCtrl;
	CShadeButtonST     m_rBtnAcitve;
	CShadeButtonST     m_rBtnNewaddr;
	CShadeButtonST     m_rBtnCopyaddr;
	CShadeButtonST     m_rBtnExcel;
	map<string,uistruct::LISTADDR_t> m_MapAddrInfo;
public:
	void		     SetBkBmpNid( UINT nBitmapIn ) ;
	void ShowListInfo();
	void    ModifyListCtrlItem();
	void    InsertListCtrlItem();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL Create(CWnd* pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonJhdz();
	afx_msg void OnBnClickedButtonNewaddress();
	afx_msg void OnBnClickedCopyaddress();
	afx_msg LRESULT OnShowListCtrl(  WPARAM wParam, LPARAM lParam ) ;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedExportExel2();
};
