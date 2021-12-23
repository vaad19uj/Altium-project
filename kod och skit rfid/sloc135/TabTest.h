#if !defined(AFX_TABTHREE_H__798A9124_C906_446C_822D_322B5AB6C4C4__INCLUDED_)
#define AFX_TABTHREE_H__798A9124_C906_446C_822D_322B5AB6C4C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabTest.h : header file
//
/////////////////////////////////////////////////////
// This file is part of an RFID GUI. It is distributed as is
// and Texas Instruments does not provide any support.
// It is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
/////////////////////////////////////////////////////
// ShreHarsha Rao
/////////////////////////////////////////////////////
extern int portDummyWrite (char *, int, int);

/////////////////////////////////////////////////////////////////////////////
// CTabTest dialog

class CTabTest : public CDialog
{
// Construction
public:
	CTabTest(CWnd* pParent = NULL);   // standard constructor
	void TResetTab();

// Dialog Data
	//{{AFX_DATA(CTabTest)
	enum { IDD = IDD_TAB_TEST };
	CString	m_sTCurrentTP;
	CString	m_sTString;
	CString	m_sTErrors;
	BOOL	m_bTExpert;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabTest)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabTest)
	afx_msg void OnDela();
	afx_msg void OnTNewTp();
	afx_msg void OnTRunTp();
	afx_msg void OnTSend();
	afx_msg void OnTExpert();
	afx_msg void OnTDigitest();
	afx_msg void OnTSendRaw();
	afx_msg void OnFirmver();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABTHREE_H__798A9124_C906_446C_822D_322B5AB6C4C4__INCLUDED_)
