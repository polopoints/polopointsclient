#pragma once


// CSet 对话框

class CSet : public CDialogBar
{
	DECLARE_DYNAMIC(CSet)

public:
	CSet();   // 标准构造函数
	virtual ~CSet();

// 对话框数据
	enum { IDD = IDD_DIALOG_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	HBITMAP		     m_pBmp; 
public:
	CShadeButtonST         m_rBtnRpc;
	CShadeButtonST         m_rBtnQbjm;
	CShadeButtonST         m_rBtnXgmm;
	CShadeButtonST         m_rBtnQbsd;
	CShadeButtonST         m_rBtnQbbf;
	CShadeButtonST         m_rBtnDcsy;
	CShadeButtonST         m_rBtnDrsy;
	CShadeButtonST         m_rBtnSzyyid;
	CShadeButtonST         m_rBtnFfmrsz;
	CShadeButtonST         m_rBtnGbts;
public:
	void SetBkBmpNid( UINT nBitmapIn ) ;
	bool GetFileName(CString &fileName,CString strexe );
	void AddImportWalletAndBookAddr(CString fileName);
	void WriteExportWalletAndBookAddr(CString fileName);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL Create(CWnd* pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID);
	afx_msg void OnBnClickedButtonPrc();
	afx_msg void OnBnClickedButtonQbjm();
	afx_msg void OnBnClickedButtonXgmm();
	afx_msg void OnBnClickedButtonQbsd();
	afx_msg void OnBnClickedButtonQbbf();
	afx_msg void OnBnClickedButtonDcsy();
	afx_msg void OnBnClickedButtonDrsy();
	afx_msg void OnBnClickedButtonYyid();
	afx_msg void OnBnClickedButtonHfmrsz();
	afx_msg void OnBnClickedButtonGbts();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
