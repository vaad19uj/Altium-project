#if !defined(AFX_TABREGISTERS_H__A8EDB918_FADA_456C_A1F2_E6D48DF49343__INCLUDED_)
#define AFX_TABREGISTERS_H__A8EDB918_FADA_456C_A1F2_E6D48DF49343__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabRegisters.h : header file
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
// CTabRegisters dialog

class CTabRegisters : public CDialog
{
// Construction
public:
	CTabRegisters(CWnd* pParent = NULL);   // standard constructor
	void NFCSetup();
	void registersRead();

// Dialog Data
	//{{AFX_DATA(CTabRegisters)
	enum { IDD = IDD_TAB_REGISTERS };
	CString	m_sRGCircStatus;
	CString	m_sRGISOCtrl;
	CString	m_sRGISOMode1;
	CString	m_sRGISOMode2;
	CString	m_sRGTXTimerH;
	CString	m_sRGTXTimerL;
	CString	m_sRGTXPulseLen;
	CString	m_sRGRXNRWait;
	CString	m_sRGRXWait;
	CString	m_sRGModulator;
	CString	m_sRGRXSpecial;
	CString	m_sRGRegulator;
	CString	m_sRGIRQ;
	CString	m_sRGOscInt;
	CString	m_sRGCollision;
	CString	m_sRGRSSI;
	CString	m_sRGram10;
	CString	m_sRGram11;
	CString	m_sRGram12;
	CString	m_sRGram13;
	CString	m_sRGram14;
	CString	m_sRGram15;
	CString	m_sRGram16;
	CString	m_sRGram17;
	CString	m_sRGram18;
	CString	m_sRGram19;
	CString	m_sRGTest1;
	CString	m_sRGTest2;
	CString	m_sRGFIFO;
	CString	m_sRGTXLen1;
	CString	m_sRGTXLen2;
	CString	m_sRGnfc18E;
	CString	m_sRGnfc19E;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabRegisters)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabRegisters)
	afx_msg void OnRGRead();
	afx_msg void OnRGWrite();
	afx_msg void OnRGDefaults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABREGISTERS_H__A8EDB918_FADA_456C_A1F2_E6D48DF49343__INCLUDED_)
