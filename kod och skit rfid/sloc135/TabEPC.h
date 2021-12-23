#if !defined(AFX_TABEPC_H__A8EDB918_FADA_456C_A1F2_E6D48DF49343__INCLUDED_)
#define AFX_TABEPC_H__A8EDB918_FADA_456C_A1F2_E6D48DF49343__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabEPC.h : header file
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
// CTabEPC dialog

class CTabEPC : public CDialog
{
// Construction
public:
	CTabEPC(CWnd* pParent = NULL);   // standard constructor
	void EPCResetTab();

// Dialog Data
	//{{AFX_DATA(CTabEPC)
	enum { IDD = IDD_TAB_EPC };
	CComboBox	m_EPCID;
	int		m_iEPCCommandRB;
	CString	m_sEPCIDNo;
	CString	m_sEPCID;
	CString	m_sEPCSlotNo;
	CString	m_sEPCPass;
	CString	m_sEPCBlockNum;
	CString	m_sEPCData;
	int		m_iEPCFullPower;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabEPC)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabEPC)
	afx_msg void OnEPCExecute();
	afx_msg void OnEPCCommandRB();
	afx_msg void OnEPCSend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABEPC_H__A8EDB918_FADA_456C_A1F2_E6D48DF49343__INCLUDED_)
