// RFIDreadDlg.h : header file
//
/////////////////////////////////////////////////////
// This file is parth of an RFID GUI. It is distributed as is
// and Texas Instruments does not provide any support.
// It is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
/////////////////////////////////////////////////////
// ShreHarsha Rao
// 
/////////////////////////////////////////////////////

#if !defined(AFX_RFIDREADDLG_H__15269A97_2EC1_4F60_B247_3F634C1AAE19__INCLUDED_)
#define AFX_RFIDREADDLG_H__15269A97_2EC1_4F60_B247_3F634C1AAE19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyTabCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CRFIDreadDlg dialog

class CRFIDreadDlg : public CDialog
{
// Construction
public:
	CRFIDreadDlg(CWnd* pParent = NULL);	// standard constructor
	void CRFIDreadDlg::logAddFileOnly();
	void CRFIDreadDlg::logAddFileScreen();
	void CRFIDreadDlg::logAdd(CString, int, int);
	void CRFIDreadDlg::showErr(CString);
	void CRFIDreadDlg::topSetComNum(char *);
	void CRFIDreadDlg::topResetFlagsGUI();
	void CRFIDreadDlg::topUpdateFlagsGUI(char *);
	int CRFIDreadDlg:: GetPortNumXP2000(WORD , WORD , char* );


// Dialog Data
	//{{AFX_DATA(CRFIDreadDlg)
	enum { IDD = IDD_RFID_READER };
	CButton	m_EnableTRF;
	CComboBox	m_ComPort;
	CEdit	m_Logger;
	CEdit m_ComPortMan;
	CMyTabCtrl	m_tabMyTabCtrl;
	CString	m_sLogger;
	CString	m_sIRQTx;
	CString	m_sIRQRx;
	CString	m_sIRQFIFO;
	CString	m_sIRQCRC;
	CString	m_sIRQParity;
	CString	m_sIRQFraming;
	CString	m_sIRQSOF;
	CString	m_sIRQColl;
	CString	m_sFIFOLevH;
	CString	m_sFIFOLevL;
	CString	m_sFIFOFull;
	BOOL	m_bAGCon;
	BOOL	m_bMainChannelAM;
	BOOL	m_bEnableTRF;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRFIDreadDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRFIDreadDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnToto();
	afx_msg void OnExit();
	afx_msg void OnUpdate();
	afx_msg void OnReset();
	afx_msg void OnComPort();
	afx_msg void OnMainChannelAM();
	afx_msg void OnAGCOn();
	afx_msg void OnEnableTRF();
	afx_msg void OnButton1();
	afx_msg void OnClear();
	afx_msg void OnAutoComport();
	afx_msg void OnAutocomport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	int logFileOnly;

	private:
	HANDLE		m_hUSBDevice;
	DWORD		m_dwNumDevices;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RFIDREADDLG_H__15269A97_2EC1_4F60_B247_3F634C1AAE19__INCLUDED_)
