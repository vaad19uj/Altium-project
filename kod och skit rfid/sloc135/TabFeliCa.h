#if !defined(AFX_TABFELICA_H__A8EDB918_FADA_456C_A1F2_E6D48DF49343__INCLUDED_)
#define AFX_TABFELICA_H__A8EDB918_FADA_456C_A1F2_E6D48DF49343__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabFeliCa.h : header file
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
// CTabFeliCa dialog

class CTabFeliCa : public CDialog
{
// Construction
public:
	CTabFeliCa(CWnd* pParent = NULL);   // standard constructor
	void FLCResetTab();

// Dialog Data
	//{{AFX_DATA(CTabFeliCa)
	enum { IDD = IDD_TAB_FELICA };
	CComboBox	m_FLCMID;
	int		m_iFLCCommandRB;
	CString	m_sFLCMIDNo;
	CString	m_sFLCMID;
	CString	m_sFLCManufParam;
	CString	m_sFLCReqData;
	CString	m_sFLCSlotNo;
	CString	m_sFLCSpeed;
	int		m_iFLCFullPower;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabFeliCa)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabFeliCa)
	afx_msg void OnFLCExecute();
	afx_msg void OnFLCCommandRB();
	afx_msg void OnFLCMID();
	afx_msg void OnFLCSend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABFELICA_H__A8EDB918_FADA_456C_A1F2_E6D48DF49343__INCLUDED_)
