#if !defined(AFX_TABONE_H__4F1DD92C_C67D_48AE_A73F_02D7EDA0580E__INCLUDED_)
#define AFX_TABONE_H__4F1DD92C_C67D_48AE_A73F_02D7EDA0580E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Tab15693.h : header file
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

class CTab15693 : public CDialog
{
// Construction
public:
	CTab15693(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTab15693)
	enum { IDD = IDD_TAB_15693 };
	CListCtrl	m_15RSSI;
	CButton	m_15Address;
	CButton	m_15Select;
	CComboBox	m_15Protocol;
	CComboBox	m_15UID;
	int		m_i15CommandRB;
	CString	m_s15NumBl;
	CString	m_s15AFI;
	CString	m_s15Data;
	CString	m_s15DSFID;
	BOOL	m_b15DataRate;
	CString	m_s15Protocol;
	CString	m_s15FirstBN;
	BOOL	m_b15Select;
	CString	m_s15UID;
	BOOL	m_b15Option;
	BOOL	m_b15Address;
	BOOL	m_b15SubCarrier;
	CString	m_15UIDNo;
	CString	m_s15TagBS;
	CString	m_s15TagNB;
	int		m_i15FullPower;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTab15693)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTab15693)
	afx_msg BOOL OnInitDialog();
	afx_msg void On15Send();
	afx_msg void On15CommandRB();
	afx_msg void On15ReadSB();
	afx_msg void On15Execute();
	afx_msg void On15FSelect();
	afx_msg void On15UID();
	afx_msg void OnTicc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABONE_H__4F1DD92C_C67D_48AE_A73F_02D7EDA0580E__INCLUDED_)
