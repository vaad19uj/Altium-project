// MyTabCtrl.cpp : implementation file
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
/////////////////////////////////////////////////////

#include "stdafx.h"
#include "RFIDread.h"
#include "MyTabCtrl.h"

#include "Tab15693.h"
#include "Tab14443A.h"
#include "Tab14443B.h"
#include "TabTagit.h"
#include "TabFeliCa.h"
#include "TabEPC.h"
#include "TabFindtags.h"
#include "TabRegisters.h"
#include "TabNFC.h"
#include "TabTest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int newProtocol = 1, searchingTags = 0;
static int tabCurrent = -1, tabReg = -1;

/////////////////////////////////////////////////////////////////////////////
// CMyTabCtrl

CMyTabCtrl::CMyTabCtrl()
{
	int cnt = 0;
	m_tabPages[cnt++] = new CTab15693;
#if SETUP_7960
	m_tabPages[cnt++] = new CTab14443A;
#endif
#if ! SETUP_7961 || SETUP_7960
	m_tabPages[cnt++] = new CTab14443B;
#endif
#if ! SETUP_NFC && ! SETUP_NFC2 && ! SETUP_IDS0
	m_tabPages[cnt++] = new CTabTagit;
#endif
#if SETUP_7960
#if SETUP_FELICA
	m_tabPages[cnt++] = new CTabFeliCa;
#endif
	m_tabPages[cnt++] = new CTabEPC;
#endif
	m_tabPages[cnt++] = new CTabFindTags;
	tabReg = cnt;
	m_tabPages[cnt++] = new CTabRegisters;
#if SETUP_NFC || SETUP_NFC2
	m_tabPages[cnt++] = new CTabNFC;
#endif
#if SHOW_TEST
	m_tabPages[cnt++] = new CTabTest;
#endif
	m_nNumberOfPages = cnt;
}

CMyTabCtrl::~CMyTabCtrl()
{
	for(int nCount=0; nCount < m_nNumberOfPages; nCount++){
		delete m_tabPages[nCount];
	}
}

void CMyTabCtrl::Init()
{
	m_tabCurrent=0;

	int cnt = 0;
	m_tabPages[cnt++]->Create(IDD_TAB_15693, this);
#if SETUP_7960
	m_tabPages[cnt++]->Create(IDD_TAB_14443A, this);
#endif
#if ! SETUP_7961 || SETUP_7960
	m_tabPages[cnt++]->Create(IDD_TAB_14443B, this);
#endif
#if ! SETUP_NFC && ! SETUP_NFC2 && ! SETUP_IDS0
	m_tabPages[cnt++]->Create(IDD_TAB_TAGIT, this);
#endif
#if SETUP_7960
#if SETUP_FELICA
	m_tabPages[cnt++]->Create(IDD_TAB_FELICA, this);
#endif
	m_tabPages[cnt++]->Create(IDD_TAB_EPC, this);
#endif
	m_tabPages[cnt++]->Create(IDD_TAB_FINDTAGS, this);
	m_tabPages[cnt++]->Create(IDD_TAB_REGISTERS, this);
#if SETUP_NFC || SETUP_NFC2
	m_tabPages[cnt++]->Create(IDD_TAB_NFC, this);
#endif
#if SHOW_TEST
	m_tabPages[cnt++]->Create(IDD_TAB_TEST, this);
#endif

	cnt = 0;
	m_tabPages[cnt++]->ShowWindow(SW_SHOW);
#if SETUP_7960
	m_tabPages[cnt++]->ShowWindow(SW_HIDE);
#endif
#if ! SETUP_7961 || SETUP_7960
	m_tabPages[cnt++]->ShowWindow(SW_HIDE);
#endif
#if ! SETUP_NFC && ! SETUP_NFC2 && ! SETUP_IDS0
	m_tabPages[cnt++]->ShowWindow(SW_HIDE);
#endif
#if SETUP_7960
#if SETUP_FELICA
	m_tabPages[cnt++]->ShowWindow(SW_HIDE);
#endif
	m_tabPages[cnt++]->ShowWindow(SW_HIDE);
#endif
	m_tabPages[cnt++]->ShowWindow(SW_HIDE);
	m_tabPages[cnt++]->ShowWindow(SW_HIDE);
#if SETUP_NFC || SETUP_NFC2
	m_tabPages[cnt++]->ShowWindow(SW_HIDE);
#endif
#if SHOW_TEST
	m_tabPages[cnt++]->ShowWindow(SW_HIDE);
#endif

	SetRectangle();
}

void CMyTabCtrl::SetRectangle()
{
	CRect tabRect, itemRect;
	int nX, nY, nXc, nYc;

	GetClientRect(&tabRect);
	GetItemRect(0, &itemRect);

	nX=itemRect.left;
	nY=itemRect.bottom+1;
	nXc=tabRect.right-itemRect.left-1;
	nYc=tabRect.bottom-nY-1;

	m_tabPages[0]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	for(int nCount=1; nCount < m_nNumberOfPages; nCount++){
		m_tabPages[nCount]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_HIDEWINDOW);
	}
}

BEGIN_MESSAGE_MAP(CMyTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CMyTabCtrl)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyTabCtrl message handlers

void CMyTabCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTabCtrl::OnLButtonDown(nFlags, point);

	if(m_tabCurrent != GetCurFocus()){
		m_tabPages[m_tabCurrent]->ShowWindow(SW_HIDE);
		m_tabCurrent=GetCurFocus();
		m_tabPages[m_tabCurrent]->ShowWindow(SW_SHOW);
		m_tabPages[m_tabCurrent]->SetFocus();
		newProtocol = 1;
#if SETUP_7960
		if (m_tabCurrent == 1)
			I14AResetTab();
#endif
#if SETUP_IDS0
		if (m_tabCurrent == 1) {
			NFCHideTagit();
			findtagsHide14443B();
			findtagsHide14443AAndMore();
			}
#endif
		if (m_tabCurrent == 2) {
#if SETUP_7961 && ! SETUP_7960
			// hide 14443B area in findTags
			findtagsHide14443B();
			findtagsHide14443AAndMore();
#endif
#if ! SETUP_7961 || SETUP_7960
			I14BResetTab();
#endif
			}
#if SETUP_7960
		if (m_tabCurrent == 4)
#if SETUP_FELICA
			FLCResetTab();
		if (m_tabCurrent == 5)
#endif
			EPCResetTab();
#endif
#if SETUP_NFC || SETUP_NFC2
		extern NFCActive;
		if (NFCActive && (m_tabCurrent == 0 || m_tabCurrent == 1 ||
							m_tabCurrent == 2)) {
			NFCDeactivateRF();
			NFCActive = 0;
			}
		// hide Tagit
		if (m_tabCurrent == 2) {
			NFCHideTagit();
			findtagsHide14443AAndMore();
			}
		// hide RAM
		if (m_tabCurrent == 3)
			NFCHideRAM();
		// acivate RF field
		if (m_tabCurrent == 4)
			NFCActivateRF();
#endif
#if SETUP_7961 && ! SETUP_7960
		if (m_tabCurrent == 4)
			TResetTab();
#endif
#if SETUP_7960
#if SETUP_FELICA
		if (m_tabCurrent == 8)
#else
		if (m_tabCurrent == 5)
			 findtagsHideFeliCa();
		if (m_tabCurrent == 7)
#endif
			TResetTab();
#else
		if (m_tabCurrent == 5)
			TResetTab();
#endif
		UpdateData(FALSE);	// Set screen values
		if (m_tabCurrent == tabReg)
			registersRead();
		tabCurrent = m_tabCurrent;
	}
}

void tabRegistersRead()
{
	if (tabCurrent == tabReg)
		registersRead();
}
