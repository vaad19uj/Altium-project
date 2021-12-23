// TabNFC.cpp : implementation file
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
#include "DirDialog.h"
#include "RFIDread.h"
#include "TabNFC.h"
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	RS232_ONLY		0
#define LOG_AND_DELAY	0
#define USE_PFB		1
#define SHOW_ANTENNA	1

#define NFC_RETRY	8
#define	TARGET_ACK	"D410"
#define	TARGET_NAK	"D411"
#define	INITIATOR_ACK	"[00]"
#define	INITIATOR_NAK	"[FF]"

#define	NFC_TOTBUF_LEN	255
//#define	NFC_TOTBUF_LEN	255
//#define	NFC_TOTBUF_LEN	32
//#define	NFC_TOTBUF_LEN	200
#if USE_PFB
#define NFC_HEADER_LEN	6
#else
#define NFC_HEADER_LEN	4
#endif
#define NFC_EOT		0x10
#if USE_PFB
#define	NFC_BUF_LEN	(NFC_TOTBUF_LEN - NFC_HEADER_LEN)
#else
#define	NFC_BUF_LEN	(NFC_TOTBUF_LEN - NFC_HEADER_LEN - 2)
#endif
#define	NFC_LEN_MASK	0x1FFF
#define	NFC_TEST_MASK	0x2000
#define	NFC_FILE_MASK	0x4000
#define	NFC_MORE_MASK	0x8000
#define	NFC_FILE	"<<FILE>>"

#define MAX_UID		32
#define UID_LEN		68


// NFC Command codes
#define NFC_INTRO	"D4"
#define	CC_ATTR		"00"
#define	CC_WAKEUP	"02"
#define	CC_PAR_SEL	"04"
#define	CC_DATA_EX	"06"
#define	CC_DESELECT	"08"
#define	CC_RELEASE	"0A"

#define	SDD		0
#define	SELECT		1
#define	POLLING		2
#define	ATTR		3
#define	WAKEUP		4
#define	PAR_SEL		5
#define	DATA_EX		6
#define	DESELECT	7
#define	RELEASE		8
#define	PROTOCOL	9
#define	GENERATE_CID	10
#define	SEND_FILE	11
#define	FLAG		12
#define	SET		13


static char *commands[] = {	"SDD",
				"select",
				"polling",
				"attribute",
				"wake_up",
				"param_sel",
				"data_exch",
				"deselect",
				"release",
				"set_protocol",
				"generate_CID",
				"send_file",
				"flag",
				"set",
				NULL};

#define	S_106	0
#define	S_212	1
#define	S_424	2

static char *dataSpeedMode[] = {	"106",	"212", "424",
				NULL};

#define	F_TARGET	0
#define	F_PASIVE	1

static char *flags[] = {	"target",
				"pasive",
				NULL};

#define	D_SPEED		0
#define	D_CID		1
#define	D_DID		2
#define	D_NAD		3
#define	D_SBR		4
#define	D_RBR		5
#define	D_BUFLEN	6
#define	D_GENB		7
#define	D_DATA		8
#define	D_FNAME		9
#define	D_DNAME		10
#define	D_UID		11
#define D_SLOT_NO	12

static char *dataKeys[] = {	"speed",
				"CID",
				"DID",
				"NAD",
				"send_bit_rate",
				"receive_bit_rate",
				"buffer_len",
				"general_byte",
				"data",
				"file_name",
				"dir_name",
				"UID",
				"number_of_slots",
				NULL};

int NFCActive = 0;
static char UIDs[MAX_UID][UID_LEN], UIDsBCC[MAX_UID][UID_LEN];
static int selectedProtocol = -1,
		tNFCbTarget, tNFCbPasive, tNFCbSpeed, pfbx = 0, noUID = 0;
static CString tNFCCID, tNFCDID, tNFCBSend, tNFCBReceive, tNFCBufLen,
		tNFCGenByte, tNFCNAD, tNFCData, tNFCFileName, tNFCDirName,
		tNFCUID, tNFCSlotNo;

/////////////////////////////////////////////////////////////////////////////
// CTabNFC dialog

// make it avalable to external functions:
static CTabNFC *RRdialog;

CTabNFC::CTabNFC(CWnd* pParent /*=NULL*/)
	: CDialog(CTabNFC::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabNFC)
	m_bNFCTarget = FALSE;
	m_bNFCPasive = TRUE;
	m_iNFCSpeedRB = 0;
	m_iNFCCommandRB = 0;
	m_iNFCTimerID = 1;
	m_sNFCCID = _T("0123456789ABCDEF0123");
	m_sNFCCIDNo = _T("");
	m_sNFCDID = _T("01");
	m_sNFCBSend = _T("00");
	m_sNFCBReceive = _T("00");
	m_sNFCBufLen = _T("FF");
	m_sNFCGenByte = _T("0");
	m_sNFCNAD = _T("0");
	m_sNFCData = _T("");
	m_sNFCFileName = _T("");
	m_sNFCDirName = _T("");
	m_sNFCATRetries = _T("");
	m_sNFCATSize = _T("100");
	m_sNFCSlotNo = _T("00");
	m_sNFCUIDNo = _T("");
	m_sNFCUID = _T("");
	//}}AFX_DATA_INIT
	selectedProtocol = -1;
	RRdialog = this;
}


void CTabNFC::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabNFC)
	DDX_Control(pDX, IDC_NFC_UID2, m_NFCUID);
	DDX_Control(pDX, IDC_NFC_FILE_PROG, m_NFCFileProgress);
	DDX_Control(pDX, IDC_NFC_AT_PROG, m_NFCATProgress);
	DDX_Control(pDX, IDC_NFC_ATEST, m_NFCATest);
	DDX_Check(pDX, IDC_NFC_F_TARGET, m_bNFCTarget);
	DDX_Check(pDX, IDC_NFC_F_PASIVE, m_bNFCPasive);
	DDX_Radio(pDX, IDC_NFC_F_106KBS, m_iNFCSpeedRB);
	DDX_Radio(pDX, IDC_NFC_SDD_RB, m_iNFCCommandRB);
	DDX_Text(pDX, IDC_NFC_CID, m_sNFCCID);
	DDX_Text(pDX, IDC_NFC_DID, m_sNFCDID);
	DDX_Text(pDX, IDC_NFC_BS, m_sNFCBSend);
	DDX_Text(pDX, IDC_NFC_BR, m_sNFCBReceive);
	DDX_Text(pDX, IDC_NFC_BUF_LEN, m_sNFCBufLen);
	DDX_Text(pDX, IDC_NFC_GEN_BYTE, m_sNFCGenByte);
	DDX_Text(pDX, IDC_NFC_NAD, m_sNFCNAD);
	DDX_Text(pDX, IDC_NFC_DATA, m_sNFCData);
	DDX_Text(pDX, IDC_NFC_FILE_NAME, m_sNFCFileName);
	DDX_Text(pDX, IDC_NFC_DIR_NAME, m_sNFCDirName);
	DDX_Text(pDX, IDC_NFC_AT_RETRIES, m_sNFCATRetries);
	DDX_Text(pDX, IDC_NFC_AT_SIZE, m_sNFCATSize);
	DDX_Text(pDX, IDC_NFC_SLOT_NO, m_sNFCSlotNo);
	DDX_Text(pDX, IDC_NFC_UID_NO, m_sNFCUIDNo);
	DDX_CBString(pDX, IDC_NFC_UID2, m_sNFCUID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabNFC, CDialog)
	//{{AFX_MSG_MAP(CTabNFC)
	ON_BN_CLICKED(IDC_NFC_SDD_RB, OnNFCCommandRB)
	ON_BN_CLICKED(IDC_NFC_F_106KBS, OnNFCSpeedRB)
	ON_BN_CLICKED(IDC_NFC_F_PASIVE, OnNFCPassiveCB)
	ON_BN_CLICKED(IDC_NFC_F_TARGET, OnNFCFTarget)
	ON_BN_CLICKED(IDC_NFC_PROTOCOL, OnNFCSend)
	ON_BN_CLICKED(IDC_NFC_EXECUTE, OnNFCExecute)
	ON_BN_CLICKED(IDC_NFC_GENERATE, OnNFCGenerate)
	ON_BN_CLICKED(IDC_NFC_NEW_FNAME, OnNFCNewFname)
	ON_BN_CLICKED(IDC_NFC_NEW_DNAME, OnNFCNewDname)
	ON_BN_CLICKED(IDC_NFC_SEND_FILE, OnNFCSendFile)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_NFC_ATEST, OnNfcAtest)
	ON_BN_CLICKED(IDC_NFC_SELECT_RB, OnNFCCommandRB)
	ON_BN_CLICKED(IDC_NFC_POLLING_RB, OnNFCCommandRB)
	ON_BN_CLICKED(IDC_NFC_ATTR_RB, OnNFCCommandRB)
	ON_BN_CLICKED(IDC_NFC_WAKEUP_RB, OnNFCCommandRB)
	ON_BN_CLICKED(IDC_NFC_PAR_SEL_RB, OnNFCCommandRB)
	ON_BN_CLICKED(IDC_NFC_DATA_EX_RB, OnNFCCommandRB)
	ON_BN_CLICKED(IDC_NFC_DESELECT_RB, OnNFCCommandRB)
	ON_BN_CLICKED(IDC_NFC_RELEASE_RB, OnNFCCommandRB)
	ON_BN_CLICKED(IDC_NFC_F_212KBS, OnNFCSpeedRB)
	ON_BN_CLICKED(IDC_NFC_F_424KBS, OnNFCSpeedRB)
	ON_CBN_SELCHANGE(IDC_NFC_UID2, OnNFCUID)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabNFC message handlers

UINT_PTR timer = 0;



void CTabNFC::NfcSetCommandRB() 
{
	if (m_bNFCPasive) {
		if (m_iNFCSpeedRB == S_106) {
			GetDlgItem(IDC_NFC_SDD_RB)->EnableWindow(TRUE);
			m_iNFCCommandRB = SDD;
			}
		else {
			GetDlgItem(IDC_NFC_POLLING_RB)->EnableWindow(TRUE);
			GetDlgItem(IDC_NFC_SLOT_NO)->EnableWindow(TRUE);
			m_iNFCCommandRB = POLLING;
			}
		}
	else {
		GetDlgItem(IDC_NFC_ATTR_RB)->EnableWindow(TRUE);
		m_iNFCCommandRB = ATTR;
		}
}

void CTabNFC::OnNFCFTarget() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	GetDlgItem(IDC_NFC_SDD_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_SELECT_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_POLLING_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_ATTR_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_WAKEUP_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_PAR_SEL_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_DATA_EX_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_DESELECT_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_RELEASE_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_UID2)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_SLOT_NO)->EnableWindow(FALSE);
	if (m_bNFCTarget) {
		GetDlgItem(IDC_NFC_DATA)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_DIR_NAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_NFC_NEW_DNAME)->EnableWindow(TRUE);

		GetDlgItem(IDC_NFC_SEND)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NFC_FILE_NAME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NFC_NEW_FNAME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NFC_SEND_FILE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NFC_SAVE)->ShowWindow(SW_SHOWNA);
		GetDlgItem(IDC_NFC_DIR_NAME)->ShowWindow(SW_SHOWNA);
		GetDlgItem(IDC_NFC_NEW_DNAME)->ShowWindow(SW_SHOWNA);
		GetDlgItem(IDC_NFC_FILE_PROG)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_NFC_ATEST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NFC_AT_RETRIES)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NFC_AT_SIZE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NFC_AT_T1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NFC_AT_T2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NFC_AT_PROG)->ShowWindow(SW_HIDE);
		m_iNFCCommandRB = -1;

		GetDlgItem(IDC_NFC_F_PASIVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_F_106KBS)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_F_212KBS)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_F_424KBS)->EnableWindow(FALSE);
		}
	else {
		GetDlgItem(IDC_NFC_DATA)->EnableWindow(TRUE);
		GetDlgItem(IDC_NFC_SEND_FILE)->EnableWindow(TRUE);
		GetDlgItem(IDC_NFC_FILE_NAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_NFC_NEW_FNAME)->EnableWindow(TRUE);

		GetDlgItem(IDC_NFC_SEND)->ShowWindow(SW_SHOWNA);
		GetDlgItem(IDC_NFC_FILE_NAME)->ShowWindow(SW_SHOWNA);
		GetDlgItem(IDC_NFC_NEW_FNAME)->ShowWindow(SW_SHOWNA);
		GetDlgItem(IDC_NFC_SEND_FILE)->ShowWindow(SW_SHOWNA);
		GetDlgItem(IDC_NFC_SAVE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NFC_DIR_NAME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NFC_NEW_DNAME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NFC_FILE_PROG)->ShowWindow(SW_HIDE);
		m_iNFCCommandRB = 0;

		GetDlgItem(IDC_NFC_F_PASIVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_NFC_F_106KBS)->EnableWindow(TRUE);
		GetDlgItem(IDC_NFC_F_212KBS)->EnableWindow(TRUE);
		GetDlgItem(IDC_NFC_F_424KBS)->EnableWindow(TRUE);
		NfcSetCommandRB();
		}
	UpdateData(FALSE);	// Set screen values
	// Set 'send file' / 'receive directory'
}

void CTabNFC::OnNFCCommandRB() 
{
}

void CTabNFC::OnNFCPassiveCB() 
{
	UpdateData(TRUE);	// Set screen values
	GetDlgItem(IDC_NFC_SDD_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_SELECT_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_POLLING_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_ATTR_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_WAKEUP_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_PAR_SEL_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_DATA_EX_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_DESELECT_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_RELEASE_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_UID2)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_SLOT_NO)->EnableWindow(FALSE);
	NfcSetCommandRB();
	UpdateData(FALSE);	// Set screen values
}

void CTabNFC::OnNFCSpeedRB() 
{
	if (! m_bNFCPasive)	return;
	UpdateData(TRUE);	// Set screen values
	GetDlgItem(IDC_NFC_SDD_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_SELECT_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_POLLING_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_ATTR_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_WAKEUP_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_PAR_SEL_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_DATA_EX_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_DESELECT_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_RELEASE_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_UID2)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_SLOT_NO)->EnableWindow(FALSE);
	NfcSetCommandRB();
	UpdateData(FALSE);	// Set screen values
}

void CTabNFC::NfcResetTab() 
{
	GetDlgItem(IDC_NFC_DATA)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_SEND_FILE)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_NEW_FNAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_NFC_FILE_PROG)->ShowWindow(SW_HIDE);


	GetDlgItem(IDC_NFC_ATEST)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_NFC_AT_RETRIES)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_NFC_AT_SIZE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_NFC_AT_T1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_NFC_AT_T2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_NFC_AT_PROG)->ShowWindow(SW_HIDE);
}

void NFCActivateRF() 
{
	RRdialog->NfcResetTab();
	char cmd[64];
	portWrite("100001", 0, 0);
	portRead(cmd, 60);
}

void NFCDeactivateRF() 
{
	char cmd[64];
	portWrite("00F", 9, 0);
	portRead(cmd, 60);
	portWrite("100021", 0, 0);
	portRead(cmd, 60);
}

void NFCHideTagit() 
{
	findtagsHideTagit();
}

void NFCHideRAM() 
{
	registersNFCSetup();
}

static int send() 
{
	static int targ = 0;
	char cmd[CMD_LEN];
	NFCActive = 1;
//	if (! tNFCbTarget && targ) {
		portWrite("00F", 9, 0);
		portRead(cmd, CMD_LEN);
//		}
	strcpy(cmd, "10");		// MSP writing to register
	strcat(cmd, "0021");	// write 21 to register 00
	strcat(cmd, "01");		// register #
#if 0
	if (tNFCbTarget)	strcat(cmd, "2A");
	else				strcat(cmd, "3A");
#endif
	if (tNFCbTarget)	strcat(cmd, "2");
	else				strcat(cmd, "3");
	if (tNFCbPasive) {
		if (! tNFCbSpeed)		strcat(cmd, "1");
		else if (tNFCbSpeed == 1)	strcat(cmd, "2");
		else if (tNFCbSpeed == 2)	strcat(cmd, "3");
		}
	else {
		if (! tNFCbSpeed)		strcat(cmd, "9");
		else if (tNFCbSpeed == 1)	strcat(cmd, "A");
		else if (tNFCbSpeed == 2)	strcat(cmd, "B");
		}
	if (portWrite(cmd, 0, 0))
		return 1;
#if RS232_ONLY
	selectedProtocol = 1;
	newProtocol = 0;
	return 0;
#endif
	portRead(cmd, CMD_LEN);

	if (tNFCbTarget) {
		strcpy(cmd, "D4");
		strcat(cmd, "0D");		// lenght = 13
		strcat(cmd, "D40C");
		strcat(cmd, tNFCCID);
		portWrite(cmd, 0, 0);
		portRead(cmd, CMD_LEN);
//		targ = 1;
		}
	else {
		strcpy(cmd, "D4");
		strcat(cmd, "03");		// lenght = 3
		strcat(cmd, "D4");
		if (tNFCbPasive)	strcat(cmd, "F0");
		else			strcat(cmd, "F1");
		portSetReadMode();
		portSetLogging();
		portWrite(cmd, 0, 0);
		portRead(cmd, CMD_LEN);
//		targ = 0;
		}
	selectedProtocol = 1;
	newProtocol = 0;
	return 0;
}

void CTabNFC::OnNFCSend() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	tNFCbTarget = m_bNFCTarget;
	tNFCbPasive = m_bNFCPasive;
	tNFCbSpeed  = m_iNFCSpeedRB;
	tNFCCID = m_sNFCCID;
	if (send())
		return;
	if (tNFCbTarget)
		timer = SetTimer(m_iNFCTimerID, 20, NULL);
	else {
		if (! timer)	return;
		KillTimer(m_iNFCTimerID);
		timer = 0;
		}
	m_iNFCCommandRB = 0;
}

void data2hex(char *data, int *len)
{
//	return;
	char hex[4], *p;
	unsigned char u;
	int i = *len;
	p = data;
	p += *len - 1;
	while (i) {
		i--;
if (i == 155)
hex[0] = 0;
		u = (unsigned char) *p;
		sprintf(hex, "%2.2X", (int) u);
		data[2*i] = hex[0];
		data[2*i+1] = hex[1];
		p--;
		}
	*len *= 2;
	data[*len] = 0;
}

void hex2data(char *data, int *len)
{
//	return;
	char hex[4], c, *p;
	hex[2] = 0;
	p = data;
	int i = 0, byte;
	while (i < *len) {
		hex[0] = data[i];
		hex[1] = data[i+1];
		sscanf(hex, "%x", &byte);
		i += 2;
		c = (char) byte;
		*p++ = c;
		}
	*len /= 2;
	data[*len] = 0;
}

int nfcGetAcknowledge(char *answer)
{
	char reply[12], *p;
	int ok = 0, ll, n;
	ll = strlen(INITIATOR_ACK);
	answer[0] = 0;
	while ((n = portRead(reply, ll))) {
		if ((p = strchr(reply, '[')))
			strcpy(answer, p);
		else if (answer[0])
			strcat(answer, reply);
		if (strchr(answer, ']')) {
			if (strstr(answer, INITIATOR_ACK))
				ok = 1;
			break;
			}
		}
	return ok;
}

int nfcGetAcknowledgeF(char *answer)
{
	char reply[12];
	int ok = 0, ll, n;
	ll = 0;
	answer[0] = 0;
	while ((n = portRead(reply, 1))) {
		if (reply[0] == '[')
			answer[ll++] = reply[0];
		else if (answer[0])
			answer[ll++] = reply[0];
		if (reply[0] == ']') {
			answer[ll] = 0;
			if (strstr(answer, INITIATOR_ACK))
				ok = 1;
			break;
			}
		}
	return ok;
}
static int execute(int cmdno, char *reply)
{
	if ((selectedProtocol == -1 || newProtocol) && ! expert) {
		logAdd("Please, set Protocol", 0, 0);
		return 1;
		}
	char cmd[CMD_LEN], hex[8], *h;
	int err = 0;
	if (cmdno == SDD || cmdno == SELECT) {
		// see 14A
		if (cmdno == SDD)	strcpy(cmd, "A001");
		else {
			char *p;
			int i;
			strcpy(cmd, "A2");
			p = tNFCUID.GetBuffer(0);
			for (i = 0; i < noUID; i++)
				if (! strcmp(UIDs[i], p))	break;
			if (i >= noUID)	err++;
			else			strcat(cmd, UIDsBCC[i]);
			}
		}
	if (cmdno == POLLING) {
		// see Felica
		strcpy(cmd, "44");
		err += addHex(cmd, CMD_LEN, 2, tNFCSlotNo, "Number of Slots");
		}
	if (cmdno == SDD || cmdno == SELECT || cmdno == POLLING) {
		if (err) {
			hopa("Send aborted");
			return 1;
			}
		if (portWrite(cmd, 0, 0))
			return 1;

		// Read the reply
		int nn;
		nn = portRead(reply, CMD_LEN);
		return 0;
		}
	switch (cmdno) {
	  case ATTR:		h = CC_ATTR;	break;
	  case WAKEUP:		h = CC_WAKEUP;	break;
	  case PAR_SEL:		h = CC_PAR_SEL;	break;
	  case DATA_EX:		h = CC_DATA_EX;	break;
	  case DESELECT:	h = CC_DESELECT;	break;
	  case RELEASE:		h = CC_RELEASE;	break;
	  }
	strcpy(cmd, NFC_INTRO);
	strcat(cmd, "00");
	strcat(cmd, NFC_INTRO);
	strcat(cmd, h);
	int len, dlen = 0;
	if (cmdno == ATTR || cmdno == WAKEUP)
		err += addHex(cmd, CMD_LEN, 20, tNFCCID, "CID");
	if (cmdno != DATA_EX)
		err += addHex(cmd, CMD_LEN, 2, tNFCDID, "DID");
	if (cmdno == ATTR) {
		if (! strcmp(tNFCBSend.GetBuffer(0), "normal"))
			strcat(cmd, "00");
		else
			strcat(cmd, "08");
		if (! strcmp(tNFCBReceive.GetBuffer(0), "normal"))
			strcat(cmd, "00");
		else
			strcat(cmd, "08");
		strcat(cmd, "04");	// TO : WT=4 - 302us
		}
	if (cmdno == DATA_EX) {
		dlen = strlen(tNFCData);
		if (! dlen) {
			hopa("Short NFC data");
			return 1;
			}
#if RS232_ONLY
		strcat(cmd, "[");
#endif
		int pfb;
		pfb = pfbx;
		pfbx++;
		pfbx &= 0x03;
		sprintf(hex, "%2.2X", pfb);
		strcat(cmd, hex);
//		strcat(cmd, tNFCDID);
//		strcat(cmd, tNFCNAD);
		sprintf(hex, "%4.4X", dlen * 2);
		strcat(cmd, hex);
		char line[512];
		strcpy(line, tNFCData.GetBuffer(0));
		data2hex(line, &dlen);
		strcat(cmd, line);
#if RS232_ONLY
		strcat(cmd, "]");
#endif
		}
	len = strlen(cmd) / 2 - 1;
	sprintf(hex, "%2.2X", len);
	cmd[2] = hex[0];
	cmd[3] = hex[1];
	if (err) {
		hopa("Send aborted");
		return 1;
		}
#if RS232_ONLY
	if (cmdno != DATA_EX) {
		strcpy(reply, INITIATOR_ACK);
		return 0;
		}
#endif
//	int retry = NFC_RETRY;
	int retry = 1;	// No retry
	int ok = 0;
	while (retry && ! ok) {
		if (portWrite(cmd, 0, 0))
			return 1;
		// Read the reply
		ok = nfcGetAcknowledge(reply);
		if (ok)
			break;
		retry--;
		}
	return 0;
}

void CTabNFC::OnNFCExecute() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	if ((selectedProtocol == -1 || newProtocol) && ! expert) {
		MessageBox("Please, set Protocol");
		return;
		}
	if (m_iNFCCommandRB == -1) {
		MessageBox("Pleasse, select Command");
		return;
		}
	// Save flags
	tNFCbTarget = m_bNFCTarget;
	tNFCbPasive = m_bNFCPasive;
	tNFCbSpeed = m_iNFCSpeedRB;
	// Save data
	tNFCCID = m_sNFCCID;
	tNFCDID = m_sNFCDID;
	tNFCNAD = m_sNFCNAD;
	tNFCBSend = m_sNFCBSend;
	tNFCBReceive = m_sNFCBReceive;
	tNFCBufLen = m_sNFCBufLen;
	tNFCGenByte = m_sNFCGenByte;
	tNFCData = m_sNFCData;
	tNFCUID = m_sNFCUID;
	tNFCSlotNo = m_sNFCSlotNo;

	char reply[CMD_LEN];
#if 0
	char datao[256], data[256], num[8];
	int cnt = 1;
	datao[0] = 0;
	if (m_iNFCCommandRB) {
		cnt = 5;
		strcpy(datao, tNFCData.GetBuffer(0));
		}
	while (cnt > 0) {
		cnt--;
		strcpy(data, datao);
		sprintf(num, "--%3.3d", cnt);
		strcat(data, num);
		tNFCData = data;
		if (execute(m_iNFCCommandRB, reply))
			return;
		}
#else
	if (execute(m_iNFCCommandRB, reply))
		return;
#endif
goto naprej;

	// check error code
	char *emsg, *p;
	int err;
	if ((p = strstr(reply, "[x"))) {
		p += 2;
		sscanf(p, "%x", &err);
		switch (err) {
		  case 0x01:
			emsg = "Command not suported";
			break;
		  case 0x10:
			emsg = "Specified block not available";
			break;
		  case 0x12:
			emsg = "Specified block already user-locked";
			break;
		  case 0x16:
			emsg = "Specified block was not successfuly programmed";
			break;
		  case 0x18:
			emsg = "Specified block was not successfuly locked";
			break;
		  case 0x1F:
			emsg = "Command not allowed";
			break;
		  default:
			emsg = "Unknown error";
		  }
		p = strchr(p, ']');
		if (err) {
			CString msg;
			msg.Format("*** %s ***", emsg);
			logAdd(msg, 0, 0);
			return;
			}
		}
	else {
		if (! (p = strchr(reply, '['))) {
			return;
			}
		}
//	if (strncmp(p, INITIATOR_ACK, 2))
//		return;

naprej:
	p = reply;
	if (m_iNFCCommandRB == SDD || m_iNFCCommandRB == POLLING) {
		char data[260], uid[40], uid2[40], param[24], req[8];
		m_NFCUID.ResetContent();
		int index, cnt = 0, len, i;
		p = reply;
		for (i = 0; i < 4*MAX_UID; i++) {
			if (! (p = strchr(p, '[')))
				break;
			p++;
			if (*p && (*p == ']' || *p == 'z' || *p == 'Z'))
				continue;
			if (m_iNFCCommandRB == SDD) {
				if (getXWord(&p, uid))	continue;
				len = strlen(uid);
				if (! (len == 10 || len == 18 || len == 26)) {
					hopa("Invalid UID length");
					continue;
					}
				I14AuidRemoveBCC(uid2, uid);
				}
			else {
				if (getXWord(&p, data))	continue;
				if (FlcGetTagInfo(data, uid, param, req))
								continue;
				}
			index = m_NFCUID.AddString(uid2);
			m_NFCUID.SetItemData(index, cnt);
			strcpy(UIDs[cnt], uid2);
			strcpy(UIDsBCC[cnt], uid);
			cnt++;
			if (cnt >= MAX_UID)
				break;
			}
		m_NFCUID.SetCurSel(0);
		m_sNFCUID = UIDs[0];
		char num[8];
		sprintf(num, "%d", cnt);
		m_sNFCUIDNo = num;
		noUID = cnt;
		UpdateData(FALSE);	// Set screen values
 		}
	char data[33];
	if (m_iNFCCommandRB == SELECT) {
		if (getXWord(&p, data))
			return;
		}

	// Show data
	// State table described in ETIS TS 102 190 v1.1.1 (2003-03) p.41
	if (m_iNFCCommandRB == SDD) {	// SDD
		GetDlgItem(IDC_NFC_SELECT_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_NFC_UID2)->EnableWindow(TRUE);
		m_iNFCCommandRB = SELECT;
 		}
	else if (m_iNFCCommandRB == SELECT) {	// Select
		GetDlgItem(IDC_NFC_SDD_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_SELECT_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_ATTR_RB)->EnableWindow(TRUE);
		m_iNFCCommandRB = ATTR;
 		}
	else if (m_iNFCCommandRB == POLLING) {	// Polling
		GetDlgItem(IDC_NFC_ATTR_RB)->EnableWindow(TRUE);
		m_iNFCCommandRB = ATTR;
 		}
	else if (m_iNFCCommandRB == ATTR) {	// Attribute
		GetDlgItem(IDC_NFC_POLLING_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_WAKEUP_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_PAR_SEL_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_NFC_DATA_EX_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_NFC_DESELECT_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_RELEASE_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_UID2)->EnableWindow(FALSE);

		GetDlgItem(IDC_NFC_DATA)->EnableWindow(TRUE);
		GetDlgItem(IDC_NFC_SEND_FILE)->EnableWindow(TRUE);
		GetDlgItem(IDC_NFC_NEW_FNAME)->EnableWindow(TRUE);

		GetDlgItem(IDC_NFC_ATEST)->ShowWindow(SW_SHOWNA);
		GetDlgItem(IDC_NFC_AT_RETRIES)->ShowWindow(SW_SHOWNA);
		GetDlgItem(IDC_NFC_AT_SIZE)->ShowWindow(SW_SHOWNA);
		GetDlgItem(IDC_NFC_AT_T1)->ShowWindow(SW_SHOWNA);
		GetDlgItem(IDC_NFC_AT_T2)->ShowWindow(SW_SHOWNA);
		GetDlgItem(IDC_NFC_AT_PROG)->ShowWindow(SW_SHOWNA);
		m_NFCATProgress.SetPos(0);
 		}
	else if (m_iNFCCommandRB == PAR_SEL) {	// Parameter select
		GetDlgItem(IDC_NFC_WAKEUP_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_PAR_SEL_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_DATA_EX_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_NFC_DESELECT_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_RELEASE_RB)->EnableWindow(FALSE);
		m_iNFCCommandRB = DATA_EX;
 		}
	else if (m_iNFCCommandRB == DATA_EX) {	// Data exchange
		GetDlgItem(IDC_NFC_WAKEUP_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_PAR_SEL_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_DATA_EX_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_NFC_DESELECT_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_NFC_RELEASE_RB)->EnableWindow(TRUE);
 		}
	else if (m_iNFCCommandRB == DESELECT) {	// Deselect
		GetDlgItem(IDC_NFC_WAKEUP_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_NFC_PAR_SEL_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_DATA_EX_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_DESELECT_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_RELEASE_RB)->EnableWindow(FALSE);
		m_iNFCCommandRB = WAKEUP;
 		}
	else if (m_iNFCCommandRB == WAKEUP) {	// Wake up
		GetDlgItem(IDC_NFC_WAKEUP_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_PAR_SEL_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_NFC_DATA_EX_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_NFC_DESELECT_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_RELEASE_RB)->EnableWindow(FALSE);
		m_iNFCCommandRB = DATA_EX;
 		}
	else if (m_iNFCCommandRB == RELEASE) {	// Release
		GetDlgItem(IDC_NFC_WAKEUP_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_PAR_SEL_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_DATA_EX_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_DESELECT_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_NFC_RELEASE_RB)->EnableWindow(FALSE);
		m_iNFCCommandRB = ATTR;
 		}
	UpdateData(FALSE);	// Set screen values
}

static void generateCID()
{
	time_t seconds;
	time(&seconds);
	srand((unsigned int) seconds);
	char all[24], part[10], c;
	int r;
	static int iter = 0;
	r = rand() & 0x0fff;
	sprintf(part, "%3.3X", r);
	strcpy(all, part);
	r = rand() & 0x0fff;
	sprintf(part, "%3.3X", r);
	strcat(all, part);
	r = rand() & 0x0fff;
	sprintf(part, "%3.3X", r);
	strcat(all, part);
	r = rand() & 0x0fff;
	sprintf(part, "%3.3X", r);
	strcat(all, part);
	r = rand() & 0x0fff;
	sprintf(part, "%3.3X", r);
	strcat(all, part);
	r = rand() & 0x0fff;
	sprintf(part, "%3.3X", r);
	strcat(all, part);
	r = rand() & 0x00ff;
	sprintf(part, "%2.2X", r);
	strcat(all, part);
	iter++;
	if (iter == 2) {
		c = all[0];
		all[0] = all[16];
		all[16] = c;
		c = all[1];
		all[0] = all[11];
		all[11] = c;
		}
	if (iter == 3) {
		c = all[0];
		all[0] = all[15];
		all[16] = c;
		c = all[1];
		all[0] = all[12];
		all[11] = c;
		iter = 0;
		}
	tNFCCID = all;
}

void CTabNFC::OnNFCGenerate() 
{
	generateCID();
	m_sNFCCID = tNFCCID;
	UpdateData(FALSE);	// Set screen values
}

void CTabNFC::OnNFCNewFname() 
{
	char *types = "All Files (*.*)|*.*||";
	CFileDialog m_ldFile(TRUE);

	// Show the File open dialog and capture the result
	if (m_ldFile.DoModal() != IDOK)
		return;
	// Get the selected filename
	CString cs = m_ldFile.GetPathName();
	m_sNFCFileName = cs;
	tNFCFileName = m_sNFCFileName;
	UpdateData(FALSE);	// Set screen values
}

void CTabNFC::OnNFCNewDname() 
{
	CDirDialog dlg("");

	// Show the File open dialog and capture the result
	dlg.DoModal();
	// Get the selected filename
	CString cs = dlg.GetPath();
	m_sNFCDirName = cs;
	tNFCDirName = m_sNFCDirName;
	UpdateData(FALSE);	// Set screen values
}

int CTabNFC::NfcSendFile(char *status) 
{
	char *path;
	path = tNFCFileName.GetBuffer(0);
	int len = strlen(path);
	ifstream in;
	in.open(path, ios_base::binary);
	if (in.fail())
		return 9;
	char cmd[CMD_LEN], line[2*NFC_TOTBUF_LEN+16], hex[8], *p;
	p = strrchr(path, '\\');
	if (! p)
		p = path;
	else
		p++;
	strcpy(line, NFC_FILE);
	strcat(line, p);
	struct _stat sbuf;
	int size = 0, val;
	if (! _stat(path, &sbuf))
		size = sbuf.st_size;
	sprintf(cmd, "%d", size);
	strcat(line, ";");
	strcat(line, cmd);
	len = strlen(line);
	data2hex(line, &len);
	len |= NFC_MORE_MASK | NFC_FILE_MASK;
	int pfb, tot = 0, tot0 = 0, retry, first = 1, ok, more, failed = 0;
	int ret = 0, xTot = 0, xBad = 0;
	time_t start, stop;
	time(&start);
	char reply[12];
	logAdd(path, 0, 0);
	// show progress slider
	if (size) {
		GetDlgItem(IDC_NFC_FILE_PROG)->ShowWindow(SW_SHOWNA);
		m_NFCFileProgress.SetPos(0);
		UpdateData(FALSE);	// Set screen values
		}
#if ! LOG_AND_DELAY
	portNoLogging();
	portSetXFastMode();
#endif
	while (1) {
		strcpy(cmd, NFC_INTRO);
		strcat(cmd, "00");
		strcat(cmd, NFC_INTRO);
		strcat(cmd, CC_DATA_EX);
		pfb = pfbx;
		pfbx++;
		pfbx &= 0x03;
		if (! in.eof())
			pfb |= NFC_EOT;
		sprintf(hex, "%2.2x", pfb);
		strcat(cmd, hex);
//		strcat(cmd, tNFCDID);
//		strcat(cmd, tNFCNAD);
#if ! LOG_AND_DELAY
		if (! first)
			portSetXFastMode();
#endif
		first = 0;
#if RS232_ONLY
		strcat(cmd, "[");
#endif
		sprintf(hex, "%4.4X", len);
		strcat(cmd, hex);
		strcat(cmd, line);
		len = strlen(cmd) / 2 - 1;
		sprintf(hex, "%2.2X", len);
		cmd[2] = hex[0];
		cmd[3] = hex[1];
#if RS232_ONLY
		strcat(cmd, "]");
#endif
//logAdd("s", 0, 0);
		retry = NFC_RETRY;
		ok = 0;
		while (retry && ! ok) {
			xTot++;
			if (portWrite(cmd, 0, 0)) {
				ret = 1;
				failed = 1;
				goto fail;
				}
			// Read the reply
			ok = nfcGetAcknowledgeF(reply);
			if (ok)
				break;
			retry--;
			xBad++;
			}
//logAdd("a", 0, 0);
		if (! retry) {
			failed = 1;
			break;
			}
		if (in.eof())	break;
		in.read(line, NFC_BUF_LEN);
		len = in.gcount();
		if (! len && in.fail()) {
			in.clear();
			logAdd("=======================", 0, 0);
			break;
			}
		more = 0;
		if (len == NFC_BUF_LEN)
			more = 1;
		tot0 += len;
		val = (tot0 + 1) * 100 / size;
		if (size && ! (val % 10)) {
			m_NFCFileProgress.SetPos(val);
			UpdateData(FALSE);	// Set screen values
			}
		data2hex(line, &len);
		line[len] = 0;
		if (more)
			len |= NFC_MORE_MASK;
		len |= NFC_FILE_MASK;
		}
fail:
	time(&stop);
	in.close();
	in.clear();
	stop -= start;
	CString msg;
	double rate;
	if (stop == 0)	rate = 0;
	else			rate = (double) tot / stop;
	if (failed) {
		msg.Format("FAILED; %d bytes transfered in %d seconds (%g).",
											tot, stop, rate);
		strcpy(status, "[FAILED]");
		}
	else {
		msg.Format("DONE; %d bytes transfered in %d seconds (%g).",
											tot, stop, rate);
		strcpy(status, "[DONE]");
		}
	logAdd(msg, 0, 0);
	if (xBad)
		msg.Format("    %d (%d) retries.", xBad, xTot);
	else
		msg.Format("    No retries.");
	logAdd(msg, 0, 0);
	// hide progress slider
	GetDlgItem(IDC_NFC_FILE_PROG)->ShowWindow(SW_HIDE);
	portSetReadMode();
	portSetLogging();
	return ret;
}

void CTabNFC::OnNFCSendFile() 
{
	if (! strlen(m_sNFCFileName.GetBuffer(0))) {
		hopa("Select a file");
		return;
		}
	tNFCFileName = m_sNFCFileName.GetBuffer(0);
	char status[16];
	int err = NfcSendFile(status);
	if (err == 9) {
		MessageBox("Can not open " + tNFCFileName);
		return;
		}
}

void CTabNFC::OnTimer(UINT nIDEvent) 
{
	char intro[NFC_HEADER_LEN+4], intro2[NFC_HEADER_LEN+4];

	if (nIDEvent == 3) {
		NfcMakeAtest();
		return;
		}
	if (nIDEvent != m_iNFCTimerID)	return;

	portSetScanMode();
	int nn = portScan(intro, NFC_HEADER_LEN);
	if (nn == -1) {
		KillTimer(m_iNFCTimerID);
		return;
		}
	if (! nn)	return;
	char *p;
	if (! (p = strchr(intro, '[')))
		return;
	p += 1;
	int xlen;
	xlen = p - &intro[0];
	strcpy(intro2, p);
	strcpy(intro, intro2);
#if ! LOG_AND_DELAY
	portSetFastMode();
#endif
	nn = portRead(intro2, xlen);
	if (nn == -1) {
		KillTimer(m_iNFCTimerID);
		return;
		}
	strcat(intro, intro2);
	// Suspend timer
#if ! LOG_AND_DELAY
	portSetXFastMode();
#endif
	char data[2*NFC_TOTBUF_LEN+64], pfbbuf[4];
	int iintro, len, more, toFile = 0, isFile, isTest, noMore, pfb, pfb0, n;
	int retry = NFC_RETRY, tot = 0, size = 0, val;
	CString fname = m_sNFCDirName;
	if (fname.GetLength())
		fname += "\\";
	ofstream out;
	pfbbuf[2] = 0;
	p = intro;
//char msg[256];

//	logAdd("000", 0, 0);
int iter = 0;
	while (1) {
iter++;
#if USE_PFB
		pfbbuf[0] = *p++;
		pfbbuf[1] = *p++;
		sscanf(pfbbuf, "%X", &pfb);
#endif
		sscanf(p, "%X", &iintro);
		len = iintro & NFC_LEN_MASK;
		isTest = iintro & NFC_TEST_MASK;
		isFile = iintro & NFC_FILE_MASK;
		more = iintro & NFC_MORE_MASK;
		len++;	// Get closing brace ]
//if (iter > 2) portWrite("0", 9);
//sprintf(msg, "..001 len=%d, more = %d", len, more);
//logAdd(msg, 0, 0);
		retry--;
		if ((! (n = portRead(data, len)) ||
			 (n != len && n != (len+4)))
#if USE_PFB
			&& pfb != pfb0
#endif
						) {
#if RS232_ONLY
			portWrite(INITIATOR_NAK, 9, 0);
#else
			portWrite(TARGET_NAK, 0, 0);
//logAdd(TARGET_NAK, 0, 0);
#endif
			if (retry)
				goto skip2;
			else
				break;
			}
#if USE_PFB
		if (pfb == pfb0)
			goto skip;
#endif
//if (iter > 1) portWrite("0", 9);
if (iter > 1)
iter++;
//data[len] = 0;
//sprintf(msg, "..002 data=%s", data);
//logAdd(msg, 0, 0);
		hex2data(data, &len);
//data[len] = 0;
//sprintf(msg, "..002+ data=%s", data);
//logAdd(msg, 0, 0);
		if (! strncmp(data, NFC_FILE, strlen(NFC_FILE))) {
			data[len] = 0;
			logAdd(data, 0, 0);
			p = data;
			p += strlen(NFC_FILE);
			char *sep;
			if ((sep = strchr(p, ';'))) {
				*sep = 0;
				sep++;
				sscanf(sep, "%d", &size);
				}
			fname += p;
			out.open(fname, ios_base::binary);
			toFile = 1;
#if ! LOG_AND_DELAY
			portNoLogging();
#endif
			// show progress slider
			if (size) {
				GetDlgItem(IDC_NFC_FILE_PROG)->ShowWindow(SW_SHOWNA);
				m_NFCFileProgress.SetPos(0);
				UpdateData(FALSE);	// Set screen values
				}
			}
		else if (toFile) {
//			out << data;
			out.write(data, len);
			tot += len;
			val = (tot + 1) * 100 / size;
			if (size && ! (val % 10)) {
				m_NFCFileProgress.SetPos(val);
				UpdateData(FALSE);	// Set screen values
				}
			}
		else if (isFile) {
			portWrite(TARGET_NAK, 0, 0);
			break;
			}
		else if (isTest) {
#if ! LOG_AND_DELAY
			portNoLogging();
#endif
			}
		else {
			m_sNFCData = data;
			UpdateData(FALSE);	// Set screen values
			}
#if USE_PFB
		pfb0 = pfb;
	  skip:
#endif
//logAdd("..003", 0, 0);
#if RS232_ONLY
		portWrite(INITIATOR_ACK, 9, 0);
#else
		portWrite(TARGET_ACK, 0, 0);
#endif
		if (! more) {
//			logAdd("=======================", 0, 0);
			break;
			}
//if (iter > 1) portWrite("0", 9);
	skip2:
		len = NFC_HEADER_LEN;
//logAdd("..004", 0, 0);
		retry = NFC_RETRY;
		noMore = 0;
		while (1) {
			retry--;
			if (! (n = portRead(intro, len))) {
#if RS232_ONLY
				portWrite(INITIATOR_NAK, 9, 0);
#else
//				portWrite(TARGET_NAK, 0, 0);
#endif
				noMore = 1;
				break;
				}
//logAdd("..005..", 0, 0);
			if ((p = strchr(intro, '[')))
				break;
			if (! retry)
				goto fail;
			}
//logAdd("..005", 0, 0);
		if (noMore)
			break;
//logAdd("..006", 0, 0);
//if (iter > 1) portWrite("0", 9);
		p += 1;
		xlen = p - &intro[0];
//		xlen = NFC_HEADER_LEN - xlen;
		strcpy(intro2, p);
		strcpy(intro, intro2);
//logAdd("..007", 0, 0);
		nn = portRead(intro2, xlen);
//logAdd("..008", 0, 0);
		if (nn == -1)
			break;
		strcat(intro, intro2);
		p = intro;
//if (iter > 1) portWrite("0", 9);
		retry = NFC_RETRY;
		}
//logAdd("..999", 0, 0);
fail:
	if (toFile) {
		out.close();
		logAdd("DONE", 0, 0);
		// hide progress slider
		GetDlgItem(IDC_NFC_FILE_PROG)->ShowWindow(SW_HIDE);
		}
	portSetLogging();
	
	CDialog::OnTimer(nIDEvent);
	portSetReadMode();
}

int parseNFC (int doit, char *line, char *path, int lineno, char * reply)
{
	int err = 0, cmd, cmdx, flag;
	char word[128], *p = line;
	CString data, msg;
	if (getWord(&p, word, "command", &err, path, lineno, &msg)) {
		hopa(msg);
		return 1;
		}
	cmd = lookup(word, commands);
	switch (cmd) {
	  case -1:
		msg.Format("Invalid keyword '%s' in line %d", word, lineno);
		hopa(msg);
		return 1;
	  case SDD: case SELECT: case POLLING:
	  case ATTR: case WAKEUP: case PAR_SEL: case DATA_EX:
	  case DESELECT: case RELEASE:
		if (doit)
			execute(cmd, reply);
		break;
	  case PROTOCOL:
		if (doit)
			send();
		break;
	  case GENERATE_CID:
		if (doit)
			generateCID();
		break;
	  case SEND_FILE:
		if (doit)
			RRdialog->NfcSendFile(reply);
		break;
	  case FLAG:
		if (getWord(&p, word, "flag", &err, path, lineno, &msg)) {
			hopa(msg);
			return 1;
			}
		cmd = lookup(word, flags);
		if (cmd == -1) {
			msg.Format("Invalid flag '%s' in line %d", word, lineno);
			hopa(msg);
			return 1;
			}
		if (getWord(&p, word, "flag value", &err, path, lineno, &msg)) {
			hopa(msg);
			return 1;
			}
		if (! strcmp(word, "0"))	flag = 0;
		else if (! strcmp(word, "1"))	flag = 1;
		else {
			msg.Format("Invalid flag value '%s' in line %d", word, lineno);
			hopa(msg);
			return 1;
			}
		if (doit) {
			switch (cmd) {
			  case F_TARGET:
				tNFCbTarget = flag;
				break;
			  case F_PASIVE:
				tNFCbPasive = flag;
				break;
			  }
			}
		break;
	  case SET:
		if (getWord(&p, word, "set", &err, path, lineno, &msg)) {
			hopa(msg);
			return 1;
			}
		cmd = lookup(word, dataKeys);
		if (cmd == -1) {
			msg.Format("Invalid data '%s' in line %d", word, lineno);
			hopa(msg);
			return 1;
			}
		if (getWord(&p, word, "data value", &err, path, lineno, &msg)) {
			hopa(msg);
			return 1;
			}
		data = word;
		if (cmd == D_SPEED){
			cmdx = lookup(word, dataSpeedMode);
			if (cmdx == -1) {
				msg.Format("Invalid speed option '%s' in line %d", word, lineno);
				hopa(msg);
				return 1;
				}
			}
		if (doit) {
			switch (cmd) {
			  case D_SPEED:
				tNFCbSpeed = cmdx;
				break;
			  case D_CID:
				tNFCCID = data;
				break;
			  case D_DID:
				tNFCDID = data;
				break;
			  case D_NAD:
				tNFCNAD = data;
				break;
			  case D_SBR:
				tNFCBSend = data;
				break;
			  case D_RBR:
				tNFCBReceive = data;
				break;
			  case D_BUFLEN:
				tNFCBufLen = data;
				break;
			  case D_GENB:
				tNFCGenByte = data;
				break;
			  case D_DATA:
				tNFCData = data;
				break;
			  case D_FNAME:
				tNFCFileName = data;
				break;
			  case D_DNAME:
				tNFCDirName = data;
				break;
			  case D_UID:
				tNFCUID = data;
				break;
			  case D_SLOT_NO:
				tNFCSlotNo = data;
				break;
			  }
			}
		break;
	  }
	return 0;
}

void CTabNFC::NfcTestAntenna(int no, int *bad)
{
	unsigned char c = '0';
	char cmd[CMD_LEN], line[2*NFC_TOTBUF_LEN+16], hex[8], reply[12];
	int tot, val, len, xBad = 0, pfb, retry, first = 1, ok, failed = 0, i;
#if ! LOG_AND_DELAY
	portNoLogging();
	portSetXFastMode();
#endif
	tot = no;

	while (no) {
		no--;
		val = ((tot - no) + 1) * 100 / tot;
		if (! (val % 10)) {
			m_NFCATProgress.SetPos(val);
			UpdateData(FALSE);	// Set screen values
			}
		for (i = 0; i < NFC_BUF_LEN; i++)
			line[i] = c++;
		len = NFC_BUF_LEN;
		data2hex(line, &len);
		line[len] = 0;
		if (no)
			len |= NFC_MORE_MASK;
		len |= NFC_TEST_MASK;
		strcpy(cmd, NFC_INTRO);
		strcat(cmd, "00");
		strcat(cmd, NFC_INTRO);
		strcat(cmd, CC_DATA_EX);
		pfb = pfbx;
		pfbx++;
		pfbx &= 0x03;
		if (no)
			pfb |= NFC_EOT;
		sprintf(hex, "%2.2x", pfb);
		strcat(cmd, hex);
//		strcat(cmd, tNFCDID);
//		strcat(cmd, tNFCNAD);
#if ! LOG_AND_DELAY
		if (! first)
			portSetXFastMode();
#endif
		first = 0;
#if RS232_ONLY
		strcat(cmd, "[");
#endif
		sprintf(hex, "%4.4X", len);
		strcat(cmd, hex);
		strcat(cmd, line);
		len = strlen(cmd) / 2 - 1;
		sprintf(hex, "%2.2X", len);
		cmd[2] = hex[0];
		cmd[3] = hex[1];
#if RS232_ONLY
		strcat(cmd, "]");
#endif
		retry = NFC_RETRY;
		ok = 0;
		while (retry && ! ok) {
			if (portWrite(cmd, 0, 0)) {
				failed = 1;
				goto fail;
				}
			// Read the reply
			ok = nfcGetAcknowledgeF(reply);
			if (ok)
				break;
			retry--;
logAdd("retry", 1, 0);
			xBad++;
			}
		if (! retry) {
			failed = 1;
			break;
			}
		}
fail:
	CString msg;
	if (failed)	*bad = -1;
	else		*bad = xBad;
	portSetReadMode();
	portSetLogging();
}

void CTabNFC::NfcMakeAtest() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	char *p;
	int no, bad, i;
	p = m_sNFCATSize.GetBuffer(0);
	if (strlen(p))
		i = sscanf(p, "%d", &no);
	if (! strlen(p) || ! i || no <= 10) {
		hopa("Empty / invalid / short sample size");
		return;
		}
	CString msg;
	char ret[8];
	NfcTestAntenna(no, &bad);
	if (bad < 0) {
		m_sNFCATRetries = "FAIL";
		logAdd("test FAILED", 0, 0);
		UpdateData(FALSE);	// Set screen values
		return;
		}
	else {
		sprintf(ret, "%d", bad);
		m_sNFCATRetries = ret;
		UpdateData(FALSE);	// Set screen values
		return;
		}
}

void CTabNFC::OnNfcAtest() 
{
	static int runStop = 0;
	if (runStop) {
		m_NFCATest.SetWindowText("Test antenna coupling");
		UpdateData(FALSE);	// Set screen values
		runStop = 0;
		KillTimer(3);
		}
	else {
		m_NFCATest.SetWindowText("Stop");
		UpdateData(FALSE);	// Set screen values
		runStop = 1;
		// create a timer with id=1 and delay of 100 milliseconds
		SetTimer(3, 100, NULL);
		}
}

void CTabNFC::OnNFCUID() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	int index = m_NFCUID.GetCurSel(), i;
	char sel[UID_LEN];
	m_NFCUID.GetLBText(index, sel);
	CString cstr;
	for (i = 0; i < MAX_UID; i++) {
		if (! strcmp(sel, UIDs[i]))
			break;
		}
	if (i < MAX_UID) {
		m_sNFCUID = UIDs[i];
		}
	UpdateData(FALSE);	// Set screen values
}
