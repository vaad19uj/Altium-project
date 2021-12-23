#if !defined(AFX_TABTAGIT_H__A8EDB918_FADA_456C_A1F2_E6D48DF49343__INCLUDED_)
#define AFX_TABTAGIT_H__A8EDB918_FADA_456C_A1F2_E6D48DF49343__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabTagit.h : header file
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
// CTabTagit dialog

class CTabTagit : public CDialog
{
// Construction
public:
	CTabTagit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTabTagit)
	enum { IDD = IDD_TAB_TAGIT };
	CComboBox	m_TISID;
	int		m_iTICommandRB;
	CString	m_sTIVersion;
	CString	m_sTISIDNo;
	CString	m_sTISID;
	CString	m_sTINumBl;
	CString	m_sTIManuf;
	CString	m_sTILockStatus;
	BOOL	m_bTIAddressed;
	CString	m_sTIData;
	CString	m_sTIBlSize;
	CString	m_sTIBlockNum;
	BOOL	m_bTIInfo;
	int		m_iTIFullPower;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabTagit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabTagit)
	afx_msg void OnTIFAddressed();
	afx_msg void OnTIExecute();
	afx_msg void OnTICommandRB();
	afx_msg void OnTISID();
	afx_msg void OnTISend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABTAGIT_H__A8EDB918_FADA_456C_A1F2_E6D48DF49343__INCLUDED_)
