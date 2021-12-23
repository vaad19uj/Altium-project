#if !defined(AFX_TABTWO_H__7E062B52_3B6E_44C4_B58E_AAD73592C8E3__INCLUDED_)
#define AFX_TABTWO_H__7E062B52_3B6E_44C4_B58E_AAD73592C8E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Tab14443B.h : header file
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

/////////////////////////////////////////////////////////////////////////////
// CTab14443B dialog

class CTab14443B : public CDialog
{
// Construction
public:
	CTab14443B(CWnd* pParent = NULL);   // standard constructor
	void I14BResetTab();

// Dialog Data
	//{{AFX_DATA(CTab14443B)
	enum { IDD = IDD_TAB_14443B };
	int	m_i14BCommandRB;
	CComboBox m_14BPUPI;
	CString m_s14BPUPINo;
	CString	m_s14BPUPI;
	CString	m_s14BAFI;
	CString	m_s14BNumAppl;
	CString	m_s14BBitRate;
	CString	m_s14BMaxFrame;
	CString	m_s14BProtocolType;
	CString	m_s14BFWI;
	CString	m_s14BADC;
	CString	m_s14BFO;
	int		m_i14BFullPower;
	CString	m_s14BTR0;
	CString	m_s14BTR1;
	BOOL	m_b14BSOF;
	BOOL	m_b14BEOF;
	CString	m_s14BCID;
	CString	m_s14BPC2PI;
	CString	m_s14BPI2PC;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTab14443B)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTab14443B)
	afx_msg void On14BCommandRB();
	afx_msg void On14BSend();
	afx_msg void On14BExecute();
	afx_msg void On14BPUPI();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void showTagData(int);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABTWO_H__7E062B52_3B6E_44C4_B58E_AAD73592C8E3__INCLUDED_)
