// TabFindTags.cpp : implementation file
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


#include "stdafx.h"
#include "RFIDread.h"
#include "TabFindTags.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	MAX_TAG		32

/////////////////////////////////////////////////////////////////////////////
// CTabFindTags dialog

// make it avalable to external functions:
static CTabFindTags *findDialog;

CTabFindTags::CTabFindTags(CWnd* pParent /*=NULL*/)
	: CDialog(CTabFindTags::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabFindTags)
	m_sFIND15693N = _T("");
	m_sFIND15693 = _T("");
	m_sFIND14443AN = _T("");
	m_sFIND14443A = _T("");
	m_sFIND14443BN = _T("");
	m_sFIND14443B = _T("");
	m_sFINDTagitN = _T("");
	m_sFINDTagit = _T("");
	m_sFINDFeliCaN = _T("");
	m_sFINDFeliCa = _T("");
	m_sFINDEPCN = _T("");
	m_sFINDEPC = _T("");
	m_bFindSel15 = TRUE;
	m_bFindSel14A = TRUE;
	m_bFindSel14B = TRUE;
	m_bFindSelEPC = TRUE;
	m_bFindSelFelica = TRUE;
	m_bFindSelTagit = TRUE;
	m_bFindSelAll = TRUE;
	//}}AFX_DATA_INIT
	findDialog = this;
}


void CTabFindTags::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabFindTags)
	DDX_Control(pDX, IDC_FIND_14443Ax, m_tFIND14443Ax);
	DDX_Control(pDX, IDC_FIND_TAGITx, m_tFINDTAGITx);
	DDX_Control(pDX, IDC_FIND_EPCx, m_tFINDEPCx);
	DDX_Control(pDX, IDC_FIND_FELICAx, m_tFINDFELICAx);
	DDX_Control(pDX, IDC_FIND_14443x, m_tFIND14443x);
	DDX_Control(pDX, IDC_FIND_15693x, m_tFIND15693x);
	DDX_Control(pDX, IDC_FIND_RUN, m_FINDRun);
	DDX_Text(pDX, IDC_FIND_15693N, m_sFIND15693N);
	DDX_Text(pDX, IDC_FIND_15693, m_sFIND15693);
	DDX_Text(pDX, IDC_FIND_14443AN, m_sFIND14443AN);
	DDX_Text(pDX, IDC_FIND_14443A, m_sFIND14443A);
	DDX_Text(pDX, IDC_FIND_14443BN, m_sFIND14443BN);
	DDX_Text(pDX, IDC_FIND_14443B, m_sFIND14443B);
	DDX_Text(pDX, IDC_FIND_TAGITN, m_sFINDTagitN);
	DDX_Text(pDX, IDC_FIND_TAGIT, m_sFINDTagit);
	DDX_Text(pDX, IDC_FIND_FLCN, m_sFINDFeliCaN);
	DDX_Text(pDX, IDC_FIND_FLC, m_sFINDFeliCa);
	DDX_Text(pDX, IDC_FIND_EPCN, m_sFINDEPCN);
	DDX_Text(pDX, IDC_FIND_EPC, m_sFINDEPC);
	DDX_Check(pDX, IDC_FIND_SEL_14A, m_bFindSel14A);
	DDX_Check(pDX, IDC_FIND_SEL_15, m_bFindSel15);
	DDX_Check(pDX, IDC_FIND_SEL_ALL, m_bFindSelAll);
	DDX_Check(pDX, IDC_FIND_SEL_14B, m_bFindSel14B);
	DDX_Check(pDX, IDC_FIND_SEL_EPC, m_bFindSelEPC);
	DDX_Check(pDX, IDC_FIND_SEL_FELICA, m_bFindSelFelica);
	DDX_Check(pDX, IDC_FIND_SEL_TAGIT, m_bFindSelTagit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabFindTags, CDialog)
	//{{AFX_MSG_MAP(CTabFindTags)
	ON_BN_CLICKED(IDC_FIND_RUN, OnFINDRun)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_FIND_SEL_ALL, OnFindSelectAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabFindTags message handlers
static int runStop = 0;

void CTabFindTags::HideFeliCa()
{
        GetDlgItem(IDC_FIND_FLCN)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_FLC)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_FLC1)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_FLC2)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_FLC3)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_SEL_FELICA)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_FELICAx)->ShowWindow(SW_HIDE);
}

void CTabFindTags::Hide14443AAndMore()
{
        GetDlgItem(IDC_FIND_14443AN)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_14443A)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_14443A1)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_14443A2)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_14443A3)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_SEL_14A)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_14443Ax)->ShowWindow(SW_HIDE);

        GetDlgItem(IDC_FIND_FLCN)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_FLC)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_FLC1)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_FLC2)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_FLC3)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_SEL_FELICA)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_FELICAx)->ShowWindow(SW_HIDE);

        GetDlgItem(IDC_FIND_EPCN)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_EPC)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_EPC1)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_EPC2)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_EPC3)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_SEL_EPC)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_EPCx)->ShowWindow(SW_HIDE);
}

void CTabFindTags::Hide14443B()
{
        GetDlgItem(IDC_FIND_14443BN)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_14443B)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_14443B1)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_14443B2)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_14443B3)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_SEL_14B)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_14443x)->ShowWindow(SW_HIDE);
}

void CTabFindTags::HideTagit()
{
        GetDlgItem(IDC_FIND_TAGITN)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_TAGIT)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_TAGIT1)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_TAGIT2)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_TAGIT3)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_SEL_TAGIT)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_FIND_TAGITx)->ShowWindow(SW_HIDE);
}
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

void CTabFindTags::OnTimer(UINT nIDEvent) 
{
	char num[4];
	int cnt;
	TagID ids[MAX_TAG], *t;

	if (nIDEvent != 1)	return;

	static int blink = 0;
	char *blink2;

	if (blink == 0)	blink2 = "--";
	else if (blink == 1)	blink2 = "\\\\";
	else if (blink == 2)	blink2 = "||";
	else if (blink == 3)	blink2 = "//";

	//if (blink == 0)	blink2 = "11";
	//else if (blink == 1)	blink2 = "22";
	//else if (blink == 2)	blink2 = "33";
	//else if (blink == 3)	blink2 = "44";

	blink++;
	if (blink == 4)	blink = 0;

	UpdateData(TRUE);	// Get curret values from the screen
	if (m_bFindSel15) {
		/* 15693 */
		m_tFIND15693x.SetWindowText(blink2);
		UpdateData(FALSE);	// Set screen values

		/* 15693, high data rate, 1 out of 4, inventory */
		find15693Tags(ids, MAX_TAG);
		t = ids;
		cnt = 0;
		if (*(t->id)) {
			m_sFIND15693 = t->id;
			cnt++;
			for (t++; *(t->id); t++) {
				m_sFIND15693 += "\r\n";
				m_sFIND15693 += t->id;
				cnt++;
				}
			}
		else
			m_sFIND15693 = "";
		sprintf(num, "%d", cnt);
		m_sFIND15693N = num;
		UpdateData(FALSE);	// Set screen values
		}

#if SETUP_7960
	if (m_bFindSel14A) {
		/* 14443A */
		m_tFIND14443Ax.SetWindowText(blink2);
		UpdateData(FALSE);	// Set screen values

		/* 14443A */
		find14443ATags(ids, MAX_TAG);
		t = ids;
		cnt = 0;
		if (*(t->id)) {
			m_sFIND14443A = t->id;
			cnt++;
			for (t++; *(t->id); t++) {
				m_sFIND14443A += "\r\n";
				m_sFIND14443A += t->id;
				cnt++;
				}
			}
		else
			m_sFIND14443A = "";
		sprintf(num, "%d", cnt);
		m_sFIND14443AN = num;
		UpdateData(FALSE);	// Set screen values

		//Need to add some delay here to prevent "Failed to open Comport error"
		unsigned long delaycnt;
		for(delaycnt=0;delaycnt<1000000;delaycnt++);



		}
#endif

#if ! SETUP_7961 || SETUP_7960
	if (m_bFindSel14B) {
		/* 14443B */
		m_tFIND14443x.SetWindowText(blink2);
		UpdateData(FALSE);	// Set screen values

		find14443BTags(ids, MAX_TAG);
		t = ids;
		cnt = 0;
		if (*(t->id)) {
			m_sFIND14443B = t->id;
			cnt++;
			for (t++; *(t->id); t++) {
				m_sFIND14443B += "\r\n";
				m_sFIND14443B += t->id;
				cnt++;
				}
			}
		else
			m_sFIND14443B = "";
		sprintf(num, "%d", cnt);
		m_sFIND14443BN = num;
		UpdateData(FALSE);	// Set screen values
		}
#endif

#if ! SETUP_NFC && ! SETUP_NFC
	if (m_bFindSelTagit) {
		m_tFINDTAGITx.SetWindowText(blink2);
		UpdateData(FALSE);	// Set screen values

		/* Tagit */
		findTagitTags(ids, MAX_TAG);
		t = ids;
		cnt = 0;
		if (*(t->id)) {
			m_sFINDTagit = t->id;
			cnt++;
			for (t++; *(t->id); t++) {
				m_sFINDTagit += "\r\n";
				m_sFINDTagit += t->id;
				cnt++;
				}
			}
		else
			m_sFINDTagit = "";
		sprintf(num, "%d", cnt);
		m_sFINDTagitN = num;
		UpdateData(FALSE);	// Set screen values
		}
#endif

#if SETUP_7960
#if SETUP_FELICA
	if (m_bFindSelFelica) {
		m_tFINDFELICAx.SetWindowText(blink2);
		UpdateData(FALSE);	// Set screen values

		/* FeliCa */
		findFeliCaTags(ids, MAX_TAG);
		t = ids;
		cnt = 0;
		if (*(t->id)) {
			m_sFINDFeliCa = t->id;
			cnt++;
			for (t++; *(t->id); t++) {
				m_sFINDFeliCa += "\r\n";
				m_sFINDFeliCa += t->id;
				cnt++;
				}
			}
		else
			m_sFINDFeliCa = "";
		sprintf(num, "%d", cnt);
		m_sFINDFeliCaN = num;
		UpdateData(FALSE);	// Set screen values
		}
#endif

	if (m_bFindSelEPC) {
		/* EPC */
		m_tFINDEPCx.SetWindowText(blink2);
		UpdateData(FALSE);	// Set screen values

		findEPCTags(ids, MAX_TAG);
		t = ids;
		cnt = 0;
		if (*(t->id)) {
			m_sFINDEPC = t->id;
			cnt++;
			for (t++; *(t->id); t++) {
				m_sFINDEPC += "\r\n";
				m_sFINDEPC += t->id;
				cnt++;
				}
			}
		else
			m_sFINDEPC = "";
		sprintf(num, "%d", cnt);
		m_sFINDEPCN = num;
		UpdateData(FALSE);	// Set screen values
		}
#endif

	return;
}

void CTabFindTags::OnFINDRun() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	if (runStop) {
		logAddFileScreen();
		searchingTags = 0;
		m_FINDRun.SetWindowText("Run");
		// Hide all action characters
		GetDlgItem(IDC_FIND_15693x)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FIND_14443Ax)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FIND_14443x)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FIND_TAGITx)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FIND_FELICAx)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FIND_EPCx)->ShowWindow(SW_HIDE);
		UpdateData(FALSE);	// Set curret values

		runStop = 0;
		KillTimer(1);
		}
	else {
		logAddFileOnly();
		searchingTags = 1;
		m_FINDRun.SetWindowText("Stop");
		// Show all action characters
		GetDlgItem(IDC_FIND_15693x)->ShowWindow(SW_SHOWNA);
#if SETUP_7960
		GetDlgItem(IDC_FIND_14443Ax)->ShowWindow(SW_SHOWNA);
#endif
#if ! SETUP_7961 || SETUP_7960
		GetDlgItem(IDC_FIND_14443x)->ShowWindow(SW_SHOWNA);
#endif
#if ! SETUP_NFC && ! SETUP_NFC
		GetDlgItem(IDC_FIND_TAGITx)->ShowWindow(SW_SHOWNA);
#endif
#if SETUP_7960
#if SETUP_FELICA
		GetDlgItem(IDC_FIND_FELICAx)->ShowWindow(SW_SHOWNA);
#endif
		GetDlgItem(IDC_FIND_EPCx)->ShowWindow(SW_SHOWNA);
#endif

		UpdateData(FALSE);	// Set curret values
		runStop = 1;
		// create a timer with id=1 and delay of 100 milliseconds
	
		//SetTimer(1, 100, NULL);
		SetTimer(1, 5, NULL);
		}
}

void findtagsHideFeliCa()
{
	findDialog->HideFeliCa();
}

void findtagsHideTagit()
{
	findDialog->HideTagit();
}

void findtagsHide14443B()
{
	findDialog->Hide14443B();
}

void findtagsHide14443AAndMore()
{
	findDialog->Hide14443AAndMore();
}

void CTabFindTags::OnFindSelectAll() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	BOOL val;
	if (m_bFindSelAll)	val = TRUE;
	else			val = FALSE;
	m_bFindSel15 = val;
	m_bFindSel14A = val;
	m_bFindSel14B = val;
	m_bFindSelTagit = val;
	m_bFindSelFelica = val;
	m_bFindSelEPC = val;
	UpdateData(FALSE);	// Set curret values
}
