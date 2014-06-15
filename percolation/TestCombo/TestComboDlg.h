// TestComboDlg.h : header file
//
#include "combination.h"
#include<utility>
#include<map>


#if !defined(AFX_TESTCOMBODLG_H__DE323766_C018_11D7_ACBC_F9366451BC33__INCLUDED_)
#define AFX_TESTCOMBODLG_H__DE323766_C018_11D7_ACBC_F9366451BC33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTestComboDlg dialog

class CTestComboDlg : public CDialog
{
// Construction
public:
	CTestComboDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTestComboDlg)
	enum { IDD = IDD_TESTCOMBO_DIALOG };
	CProgressCtrl	c_progressbar;
	int		m_n_seq;
	int		m_r_seq;
	CString	m_complete;
	int		m_radio;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestComboDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	std::map<char,int> mc;
private:
	__int64 fact(int num);
	__int64 NumOfCombo();
	void InitialiseMap();
	bool CheckMap();
	void char_combination(char n[],int n_column,
       char r[], int r_size, int r_column, int loop);
public:
  void temp(char *begin,char* end);

	// Generated message map functions
	//{{AFX_MSG(CTestComboDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTCOMBODLG_H__DE323766_C018_11D7_ACBC_F9366451BC33__INCLUDED_)
