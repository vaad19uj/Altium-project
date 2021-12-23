// TabRegisters.cpp : implementation file
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
#include "TabRegisters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
static char SIDs[MAX_SID][SID_LEN];
unsigned char manufs[MAX_SID], blSizes[MAX_SID], numBls[MAX_SID];
unsigned short versions[MAX_SID];
static int selectedProtocol = -1,
		tTIbAddressed, tTIbInfo;
static CString tTISID, tTIBlockNum, tTIBlSize, tTIData;
 */

/////////////////////////////////////////////////////////////////////////////
// CTabRegisters dialog

// make it avalable to external functions:
static CTabRegisters *regDialog;


CTabRegisters::CTabRegisters(CWnd* pParent /*=NULL*/)
	: CDialog(CTabRegisters::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabRegisters)
	m_sRGCircStatus = _T("");
	m_sRGISOCtrl = _T("");
	m_sRGISOMode1 = _T("");
	m_sRGISOMode2 = _T("");
	m_sRGTXTimerH = _T("");
	m_sRGTXTimerL = _T("");
	m_sRGTXPulseLen = _T("");
	m_sRGRXNRWait = _T("");
	m_sRGRXWait = _T("");
	m_sRGModulator = _T("");
	m_sRGRXSpecial = _T("");
	m_sRGRegulator = _T("");
	m_sRGIRQ = _T("");
	m_sRGOscInt = _T("");
	m_sRGCollision = _T("");
	m_sRGRSSI = _T("");
	m_sRGram10 = _T("");
	m_sRGram11 = _T("");
	m_sRGram12 = _T("");
	m_sRGram13 = _T("");
	m_sRGram14 = _T("");
	m_sRGram15 = _T("");
	m_sRGram16 = _T("");
	m_sRGram17 = _T("");
	m_sRGram18 = _T("");
	m_sRGram19 = _T("");
	m_sRGTest1 = _T("");
	m_sRGTest2 = _T("");
	m_sRGFIFO = _T("");
	m_sRGTXLen1 = _T("");
	m_sRGTXLen2 = _T("");
	m_sRGnfc18E = _T("");
	m_sRGnfc19E = _T("");
	//}}AFX_DATA_INIT
	regDialog = this;
}


void CTabRegisters::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabRegisters)
	DDX_Text(pDX, IDC_RG_CIRC_STATUS, m_sRGCircStatus);
	DDV_MaxChars(pDX, m_sRGCircStatus, 2);
	DDX_Text(pDX, IDC_RG_ISO_CTRL, m_sRGISOCtrl);
	DDV_MaxChars(pDX, m_sRGISOCtrl, 2);
	DDX_Text(pDX, IDC_RG_ISO_MODE1, m_sRGISOMode1);
	DDV_MaxChars(pDX, m_sRGISOMode1, 2);
	DDX_Text(pDX, IDC_RG_ISO_MODE2, m_sRGISOMode2);
	DDV_MaxChars(pDX, m_sRGISOMode2, 2);
	DDX_Text(pDX, IDC_RG_TX_TIMER_H, m_sRGTXTimerH);
	DDV_MaxChars(pDX, m_sRGTXTimerH, 2);
	DDX_Text(pDX, IDC_RG_TX_TIMER_L, m_sRGTXTimerL);
	DDV_MaxChars(pDX, m_sRGTXTimerL, 2);
	DDX_Text(pDX, IDC_RG_TX_PULSE_LEN, m_sRGTXPulseLen);
	DDV_MaxChars(pDX, m_sRGTXPulseLen, 2);
	DDX_Text(pDX, IDC_RG_RX_NR_WAIT, m_sRGRXNRWait);
	DDV_MaxChars(pDX, m_sRGRXNRWait, 2);
	DDX_Text(pDX, IDC_RG_RX_WAIT, m_sRGRXWait);
	DDV_MaxChars(pDX, m_sRGRXWait, 2);
	DDX_Text(pDX, IDC_RG_MODULATOR, m_sRGModulator);
	DDV_MaxChars(pDX, m_sRGModulator, 2);
	DDX_Text(pDX, IDC_RG_RX_SPECIAL, m_sRGRXSpecial);
	DDV_MaxChars(pDX, m_sRGRXSpecial, 2);
	DDX_Text(pDX, IDC_RG_REGULATOR, m_sRGRegulator);
	DDV_MaxChars(pDX, m_sRGRegulator, 2);
	DDX_Text(pDX, IDC_RG_IRQ, m_sRGIRQ);
	DDV_MaxChars(pDX, m_sRGIRQ, 2);
	DDX_Text(pDX, IDC_RG_OSC_INT, m_sRGOscInt);
	DDV_MaxChars(pDX, m_sRGOscInt, 2);
	DDX_Text(pDX, IDC_RG_COLLISION, m_sRGCollision);
	DDV_MaxChars(pDX, m_sRGCollision, 2);
	DDX_Text(pDX, IDC_RG_RSSI, m_sRGRSSI);
	DDV_MaxChars(pDX, m_sRGRSSI, 5);
	DDX_Text(pDX, IDC_RG_RAM10, m_sRGram10);
	DDV_MaxChars(pDX, m_sRGram10, 2);
	DDX_Text(pDX, IDC_RG_RAM11, m_sRGram11);
	DDV_MaxChars(pDX, m_sRGram11, 2);
	DDX_Text(pDX, IDC_RG_RAM12, m_sRGram12);
	DDV_MaxChars(pDX, m_sRGram12, 2);
	DDX_Text(pDX, IDC_RG_RAM13, m_sRGram13);
	DDV_MaxChars(pDX, m_sRGram13, 2);
	DDX_Text(pDX, IDC_RG_RAM14, m_sRGram14);
	DDV_MaxChars(pDX, m_sRGram14, 2);
	DDX_Text(pDX, IDC_RG_RAM15, m_sRGram15);
	DDV_MaxChars(pDX, m_sRGram15, 2);
	DDX_Text(pDX, IDC_RG_RAM16, m_sRGram16);
	DDV_MaxChars(pDX, m_sRGram16, 2);
	DDX_Text(pDX, IDC_RG_RAM17, m_sRGram17);
	DDV_MaxChars(pDX, m_sRGram17, 2);
	DDX_Text(pDX, IDC_RG_RAM18, m_sRGram18);
	DDV_MaxChars(pDX, m_sRGram18, 2);
	DDX_Text(pDX, IDC_RG_RAM19, m_sRGram19);
	DDV_MaxChars(pDX, m_sRGram19, 2);
	DDX_Text(pDX, IDC_RG_TEST1, m_sRGTest1);
	DDV_MaxChars(pDX, m_sRGTest1, 2);
	DDX_Text(pDX, IDC_RG_TEST2, m_sRGTest2);
	DDV_MaxChars(pDX, m_sRGTest2, 2);
	DDX_Text(pDX, IDC_RG_FIFO, m_sRGFIFO);
	DDV_MaxChars(pDX, m_sRGFIFO, 2);
	DDX_Text(pDX, IDC_RG_TX_LEN1, m_sRGTXLen1);
	DDV_MaxChars(pDX, m_sRGTXLen1, 2);
	DDX_Text(pDX, IDC_RG_TX_LEN2, m_sRGTXLen2);
	DDV_MaxChars(pDX, m_sRGTXLen2, 2);
	DDX_Text(pDX, IDC_RG_NFC18E, m_sRGnfc18E);
	DDX_Text(pDX, IDC_RG_NFC19E, m_sRGnfc19E);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabRegisters, CDialog)
	//{{AFX_MSG_MAP(CTabRegisters)
	ON_BN_CLICKED(IDC_RG_READ, OnRGRead)
	ON_BN_CLICKED(IDC_RG_WRITE, OnRGWrite)
	ON_BN_CLICKED(IDC_RG_DEFAULTS, OnRGDefaults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabRegisters message handlers

void CTabRegisters::NFCSetup()
{
#if SETUP_NFC || SETUP_NFC2
        GetDlgItem(IDC_RG_RAM10)->EnableWindow(FALSE);
        GetDlgItem(IDC_RG_RAM11)->EnableWindow(FALSE);
        GetDlgItem(IDC_RG_RAM12)->EnableWindow(FALSE);
        GetDlgItem(IDC_RG_RAM13)->EnableWindow(FALSE);
        GetDlgItem(IDC_RG_RAM14)->EnableWindow(FALSE);
        GetDlgItem(IDC_RG_RAM15)->EnableWindow(FALSE);
        GetDlgItem(IDC_RG_RAM16)->EnableWindow(FALSE);
        GetDlgItem(IDC_RG_RAM17)->EnableWindow(FALSE);
        GetDlgItem(IDC_RG_RAM18)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_RG_RAM19)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_RG_RAM18T)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_RG_RAM19T)->ShowWindow(SW_HIDE);

        GetDlgItem(IDC_RG_NFCBOX)->ShowWindow(SW_SHOWNA);
        GetDlgItem(IDC_RG_NFC18)->ShowWindow(SW_SHOWNA);
        GetDlgItem(IDC_RG_NFC18E)->ShowWindow(SW_SHOWNA);
        GetDlgItem(IDC_RG_NFC19)->ShowWindow(SW_SHOWNA);
        GetDlgItem(IDC_RG_NFC19E)->ShowWindow(SW_SHOWNA);
#else
        GetDlgItem(IDC_RG_NFCBOX)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_RG_NFC18)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_RG_NFC18E)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_RG_NFC19)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_RG_NFC19E)->ShowWindow(SW_HIDE);
#endif
}

static int readRegs(char regs[32][4], int *num) 
{
	*num = 0;
	char reply[CMD_LEN], *cmd = "131F00";
	int nn, x;
#if SETUP_NFC || SETUP_NFC2
	cmd = "131000";
#endif
	if (portWrite(cmd, 0, 0))
		return 1;
	nn = portRead(reply, CMD_LEN);
	if (nn <= 0)	return 1;

	char word[4], *p;
	int cnt = 0;
	for (p = reply; *p; ) {
		if (! (p = strchr(p, '[')))
			break;
		p++;
		if (*p && (*p == ']' || *p == 'z' || *p == 'Z'))
			continue;
		if (getXWord(&p, word))	continue;
		sscanf(word, "%x", &x);
		sprintf(regs[cnt], "%2.2X", x);
		cnt++;
#if SETUP_NFC || SETUP_NFC2
		if (cnt > 16) {
#else
		if (cnt > 31) {
#endif
			logAdd("Too many words", 0, 0);
			return 1;
			}
		}
#if SETUP_NFC || SETUP_NFC2
	if (cnt != 16) {
		logAdd("expecting 16 words", 0, 0);
#else
	if (cnt != 31) {
		logAdd("expecting 31 words", 0, 0);
#endif
		return 1;
		}

#if SETUP_NFC || SETUP_NFC2
	// Fill "00" into unused registers
	for (cnt = 16; cnt < 24; cnt++);
		strcpy(regs[cnt], "00");
	// Read the remaining 7 registers
	cmd = "130718";
	if (portWrite(cmd, 0, 0))
		return 1;
	nn = portRead(reply, CMD_LEN);
	if (nn <= 0)	return 1;

	for (p = reply; *p; ) {
		if (! (p = strchr(p, '[')))
			break;
		p++;
		if (*p && (*p == ']' || *p == 'z' || *p == 'Z'))
			continue;
		if (getXWord(&p, word))	continue;
		sscanf(word, "%x", &x);
		sprintf(regs[cnt], "%2.2X", x);
		cnt++;
		if (cnt > 31) {
			logAdd("Too many words", 0, 0);
			return 1;
			}
		}
	if (cnt != 31) {
		logAdd("expecting 16+7 words", 0, 0);
		return 1;
		}

#endif

	*num = cnt;
	return 0;
}

char * hex2rssi (char *str)
{
	static char rssi[8];
	int stable, aux, active, val;
	sscanf(str, "%X", &val);
	stable = (val & 0x40) >> 6;
	aux = (val & 0x38) >> 3 ;
	active = val & 0x07;
	sprintf(rssi, "%d.%d.%d", stable, aux, active);
	return rssi;
}

static char * rssi2hex (char *str)
{
	static char hex[4];
	int stable, aux, active, val;
	if (sscanf(str, "%d.%d.%d", &stable, &aux, &active) == 3) {
		val = stable << 6 | aux << 3 | active;
		sprintf(hex, "%2.2X", val);
		}
	else
		strcpy(hex, "00");
	return hex;
}

void CTabRegisters::OnRGRead() 
{
	char regs[32][4];
	int cnt, i;
	if (readRegs(regs, &cnt))
	    return;
	for (i = 0; i < cnt; i++) {
		switch (i) {
		  case  0: m_sRGCircStatus = regs[i];	break;
		  case  1: m_sRGISOCtrl = regs[i];	break;
		  case  2: m_sRGISOMode1 = regs[i];	break;
		  case  3: m_sRGISOMode2 = regs[i];	break;
		  case  4: m_sRGTXTimerH = regs[i];	break;
		  case  5: m_sRGTXTimerL = regs[i];	break;
		  case  6: m_sRGTXPulseLen = regs[i];	break;
		  case  7: m_sRGRXNRWait = regs[i];	break;
		  case  8: m_sRGRXWait = regs[i];	break;
		  case  9: m_sRGModulator = regs[i];	break;
		  case 10: m_sRGRXSpecial = regs[i];	break;
		  case 11: m_sRGRegulator = regs[i];	break;
		  case 12: m_sRGIRQ = regs[i];	break;
		  case 13: m_sRGOscInt = regs[i];	break;
		  case 14: m_sRGCollision = regs[i];	break;
		  case 15: m_sRGRSSI = hex2rssi(regs[i]);	break;
#if ! SETUP_NFC && ! SETUP_NFC2
		  case 16: m_sRGram10 = regs[i];	break;
		  case 17: m_sRGram11 = regs[i];	break;
		  case 18: m_sRGram12 = regs[i];	break;
		  case 19: m_sRGram13 = regs[i];	break;
		  case 20: m_sRGram14 = regs[i];	break;
		  case 21: m_sRGram15 = regs[i];	break;
		  case 22: m_sRGram16 = regs[i];	break;
		  case 23: m_sRGram17 = regs[i];	break;
#endif
		  case 24: m_sRGram18 = regs[i];
#if SETUP_NFC || SETUP_NFC2
			   m_sRGnfc18E = regs[i];
#endif
				break;
		  case 25: m_sRGram19 = regs[i];
#if SETUP_NFC || SETUP_NFC2
			   m_sRGnfc19E = regs[i];
#endif
				break;
		  case 26: m_sRGTest1 = regs[i];	break;
		  case 27: m_sRGTest2 = regs[i];	break;
		  case 28: m_sRGFIFO = regs[i];	break;
		  case 29: m_sRGTXLen1 = regs[i];	break;
		  case 30: m_sRGTXLen2 = regs[i];	break;
		  }
		}
logAdd("top update", 0, 0);
	topUpdateFlagsGUI(regs[0]);
	UpdateData(FALSE);	// Set screen values
}

void registersRead() 
{
	regDialog->registersRead();
}

void CTabRegisters::registersRead() 
{
	regDialog->OnRGRead();
}

void CTabRegisters::OnRGWrite() 
{
	char regs[32][4];
	int cnt, i;
	if (readRegs(regs, &cnt))
	    return;
	UpdateData(TRUE);	// Get curret values from the screen
	char screen[32][4];
	for (i = 0; i < cnt; i++) {
		switch (i) {
		  case  0: strcpy(screen[i], m_sRGCircStatus.GetBuffer(0)); break;
		  case  1: strcpy(screen[i], m_sRGISOCtrl.GetBuffer(0)); break;
		  case  2: strcpy(screen[i], m_sRGISOMode1.GetBuffer(0)); break;
		  case  3: strcpy(screen[i], m_sRGISOMode2.GetBuffer(0)); break;
		  case  4: strcpy(screen[i], m_sRGTXTimerH.GetBuffer(0)); break;
		  case  5: strcpy(screen[i], m_sRGTXTimerL.GetBuffer(0)); break;
		  case  6: strcpy(screen[i], m_sRGTXPulseLen.GetBuffer(0)); break;
		  case  7: strcpy(screen[i], m_sRGRXNRWait.GetBuffer(0)); break;
		  case  8: strcpy(screen[i], m_sRGRXWait.GetBuffer(0)); break;
		  case  9: strcpy(screen[i], m_sRGModulator.GetBuffer(0)); break;
		  case 10: strcpy(screen[i], m_sRGRXSpecial.GetBuffer(0)); break;
		  case 11: strcpy(screen[i], m_sRGRegulator.GetBuffer(0)); break;
		  case 12: strcpy(screen[i], m_sRGIRQ.GetBuffer(0)); break;
		  case 13: strcpy(screen[i], m_sRGOscInt.GetBuffer(0)); break;
		  case 14: strcpy(screen[i], m_sRGCollision.GetBuffer(0)); break;
		  case 15: strcpy(screen[i], rssi2hex(m_sRGRSSI.GetBuffer(0))); break;
#if ! SETUP_NFC && ! SETUP_NFC2
		  case 16: strcpy(screen[i], m_sRGram10.GetBuffer(0)); break;
		  case 17: strcpy(screen[i], m_sRGram11.GetBuffer(0)); break;
		  case 18: strcpy(screen[i], m_sRGram12.GetBuffer(0)); break;
		  case 19: strcpy(screen[i], m_sRGram13.GetBuffer(0)); break;
		  case 20: strcpy(screen[i], m_sRGram14.GetBuffer(0)); break;
		  case 21: strcpy(screen[i], m_sRGram15.GetBuffer(0)); break;
		  case 22: strcpy(screen[i], m_sRGram16.GetBuffer(0)); break;
		  case 23: strcpy(screen[i], m_sRGram17.GetBuffer(0)); break;
#endif
		  case 24: strcpy(screen[i], m_sRGram18.GetBuffer(0)); break;
		  case 25: strcpy(screen[i], m_sRGram19.GetBuffer(0)); break;
		  case 26: strcpy(screen[i], m_sRGTest1.GetBuffer(0)); break;
		  case 27: strcpy(screen[i], m_sRGTest2.GetBuffer(0)); break;
		  case 28: strcpy(screen[i], m_sRGFIFO.GetBuffer(0)); break;
		  case 29: strcpy(screen[i], m_sRGTXLen1.GetBuffer(0)); break;
		  case 30: strcpy(screen[i], m_sRGTXLen2.GetBuffer(0)); break;
		  }
		}
	char cmd[64], all[64], one[8], reply[CMD_LEN];
	all[0] = 0;
	for (i = 0; i < cnt; i++) {
		if (i == 12 || i == 13 || i ==14 || i == 15 || i == 28)
			continue;
		if (strcmp(regs[i], screen[i])) {
			sprintf(one, "%2.2X%s", i, screen[i]);
			strcat(all, one);
			}
		}
	if (all[0]) {
		sprintf(cmd, "10%s", all);
		if (portWrite(cmd, 0, 0))
			return;
		portRead(reply, CMD_LEN);
		}
	else
		logAdd("No register changed; skipping write", 0, 0);

}

void CTabRegisters::OnRGDefaults() 
{
	m_sRGCircStatus = "01";
	m_sRGISOCtrl = "02";
	m_sRGISOMode1 = "00";
	m_sRGISOMode2 = "00";
	m_sRGTXTimerH = "C2";
	m_sRGTXTimerL = "00";
	m_sRGTXPulseLen = "00";
	m_sRGRXNRWait = "0E";
	m_sRGRXWait = "1F";
	m_sRGModulator = "11";
	m_sRGRXSpecial = "40";
	m_sRGRegulator = "87";
	m_sRGIRQ = "00";
	m_sRGOscInt = "3E";
	m_sRGCollision = "00";
	m_sRGRSSI = "0.0.0";
	m_sRGram10 = "00";
	m_sRGram11 = "00";
	m_sRGram12 = "00";
	m_sRGram13 = "00";
	m_sRGram14 = "00";
	m_sRGram15 = "00";
	m_sRGram16 = "00";
	m_sRGram17 = "00";
	m_sRGram18 = "00";
	m_sRGram19 = "00";
	m_sRGTest1 = "00";
	m_sRGTest2 = "00";
	m_sRGFIFO = "00";
	m_sRGTXLen1 = "00";
	m_sRGTXLen2 = "00";
	UpdateData(FALSE);	// Set screen values
	OnRGWrite();
}

void registersNFCSetup()
{
	regDialog->NFCSetup();
}
