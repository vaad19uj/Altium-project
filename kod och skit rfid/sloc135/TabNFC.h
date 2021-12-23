#if !defined(AFX_TABNFC_H__A8EDB918_FADA_456C_A1F2_E6D48DF49343__INCLUDED_)
#define AFX_TABNFC_H__A8EDB918_FADA_456C_A1F2_E6D48DF49343__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabNFC.h : header file
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
// CTabNFC dialog

class CTabNFC : public CDialog
{
// Construction
public:
	CTabNFC(CWnd* pParent = NULL);   // standard constructor
	void NfcResetTab();
	int NfcSendFile(char *);

// Dialog Data
	//{{AFX_DATA(CTabNFC)
	enum { IDD = IDD_TAB_NFC };
	CComboBox	m_NFCUID;
	CProgressCtrl	m_NFCFileProgress;
	CProgressCtrl	m_NFCATProgress;
	CButton	m_NFCATest;
	BOOL	m_bNFCTarget;
	BOOL	m_bNFCPasive;
	int		m_iNFCSpeedRB;
	int		m_iNFCCommandRB;
	int		m_iNFCTimerID;
	CString	m_sNFCCID;
	CString	m_sNFCCIDNo;
	CString	m_sNFCDID;
	CString	m_sNFCBSend;
	CString	m_sNFCBReceive;
	CString	m_sNFCBufLen;
	CString	m_sNFCGenByte;
	CString	m_sNFCNAD;
	CString	m_sNFCData;
	CString	m_sNFCFileName;
	CString	m_sNFCDirName;
	CString	m_sNFCATRetries;
	CString	m_sNFCATSize;
	CString	m_sNFCSlotNo;
	CString	m_sNFCUIDNo;
	CString	m_sNFCUID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabNFC)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabNFC)
	afx_msg void OnNFCCommandRB();
	afx_msg void OnNFCSpeedRB();
	afx_msg void OnNFCPassiveCB();
	afx_msg void OnNFCFTarget();
	afx_msg void OnNFCSend();
	afx_msg void OnNFCExecute();
	afx_msg void OnNFCGenerate();
	afx_msg void OnNFCNewFname();
	afx_msg void OnNFCNewDname();
	afx_msg void OnNFCSendFile();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnNfcAtest();
	afx_msg void OnNFCUID();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void NfcSetCommandRB();
	void NfcTestAntenna(int, int *);
	void NfcMakeAtest();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABNFC_H__A8EDB918_FADA_456C_A1F2_E6D48DF49343__INCLUDED_)
