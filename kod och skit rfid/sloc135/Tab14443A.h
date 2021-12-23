#if !defined(AFX_TAB14443A_H__A8EDB918_FADA_456C_A1F2_E6D48DF49343__INCLUDED_)
#define AFX_TAB14443A_H__A8EDB918_FADA_456C_A1F2_E6D48DF49343__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Tab14443A.h : header file
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
// CTab14443A dialog

class CTab14443A : public CDialog
{
// Construction
public:
	CTab14443A(CWnd* pParent = NULL);   // standard constructor
	void I14AResetTab();

// Dialog Data
	//{{AFX_DATA(CTab14443A)
	enum { IDD = IDD_TAB_14443A };
	CComboBox	m_14AUID;
	int		m_i14ACommandRB;
	CString	m_s14AVersion;
	CString	m_s14AUIDNo;
	CString	m_s14AUID;
	CString	m_s14AFSDI;
	CString	m_s14ACID;
	CString	m_s14AFSCI;
	CString	m_s14ADR;
	CString	m_s14ADS;
	CString	m_s14AFWI;
	CString	m_s14ASFGI;
	int		m_i14AFullPower;
	BOOL	m_b14ANADsupp;
	BOOL	m_b14ACIDsupp;
	CString	m_s14AHistory;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTab14443A)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTab14443A)
	afx_msg void On14AExecute();
	afx_msg void On14ACommandRB();
	afx_msg void On14AUID();
	afx_msg void On14ASend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TAB14443A_H__A8EDB918_FADA_456C_A1F2_E6D48DF49343__INCLUDED_)
