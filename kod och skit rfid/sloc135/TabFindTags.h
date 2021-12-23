#if !defined(AFX_TABFINDTAGS_H__A8EDB918_FADA_456C_A1F2_E6D48DF49343__INCLUDED_)
#define AFX_TABFINDTAGS_H__A8EDB918_FADA_456C_A1F2_E6D48DF49343__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabFindTags.h : header file
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
// CTabFindTags dialog

class CTabFindTags : public CDialog
{
// Construction
public:
	CTabFindTags(CWnd* pParent = NULL);   // standard constructor
	void Hide14443AAndMore();
	void Hide14443B();
	void HideTagit();
	void HideFeliCa();

// Dialog Data
	//{{AFX_DATA(CTabFindTags)
	enum { IDD = IDD_TAB_FINDTAGS };
	CStatic	m_tFIND14443Ax;
	CStatic	m_tFINDTAGITx;
	CStatic	m_tFINDEPCx;
	CStatic	m_tFINDFELICAx;
	CStatic	m_tFIND14443x;
	CStatic	m_tFIND15693x;
	CButton m_FINDRun;
	CString	m_sFIND15693N;
	CString	m_sFIND15693;
	CString	m_sFIND14443AN;
	CString	m_sFIND14443A;
	CString	m_sFIND14443BN;
	CString	m_sFIND14443B;
	CString	m_sFINDTagitN;
	CString	m_sFINDTagit;
	CString	m_sFINDFeliCaN;
	CString	m_sFINDFeliCa;
	CString	m_sFINDEPCN;
	CString	m_sFINDEPC;
	BOOL	m_bFindSel14A;
	BOOL	m_bFindSel15;
	BOOL	m_bFindSelAll;
	BOOL	m_bFindSel14B;
	BOOL	m_bFindSelEPC;
	BOOL	m_bFindSelFelica;
	BOOL	m_bFindSelTagit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabFindTags)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabFindTags)
	afx_msg void OnFINDRun();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnFindSelectAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABFINDTAGS_H__A8EDB918_FADA_456C_A1F2_E6D48DF49343__INCLUDED_)
