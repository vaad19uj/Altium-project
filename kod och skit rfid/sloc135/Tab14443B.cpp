// Tab14443B.cpp : implementation file
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
#include <math.h>
#include "RFIDread.h"
#include "Tab14443B.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	DATA_LEN	256
#define MAX_PUPI	32
#define PUPI_LEN	10

// 14443B Command codes
#define	CC_REQUB	"B004"
#define	CC_WUPB		"B104"
#define	CC_ATTRIB	"18"
#define	CC_HALT		"1850"

#define	REQUB		0
#define	WUPB		1
#define	ATTRIB		2
#define	HALT		3
#define	PROTOCOL	4
#define	SET			5

static char *commands[] = {	"request",
				"wake_up",
				"attrib",
				"halt",
				"set_protocol",
				"set",
				NULL};
#define	D_PUPI		0
#define	D_MAX_FR	1
#define	D_PROTO_TYPE	2
#define	D_TR0		3
#define	D_TR1		4
#define	D_EOF		5
#define	D_SOF		6
#define	D_PC2PI		7
#define	D_PI2PC		8
#define	D_CID		9
#define	D_POWER		10

static char *dataKeys[] = {	"PUPI",
				"TR0",
				"TR1",
				"EOF",
				"SOF",
				"PCD2PICC",
				"PICC2PCD",
				"CID",
				"power",
				NULL};

static char *dataPowerMode[] = {	"full",	"half",
				NULL};

static char PUPIs[MAX_PUPI][PUPI_LEN], AFIs[MAX_PUPI][4], bitRates[MAX_PUPI][4];
static int selectedProtocol = -1, t14BiFullPower,
		numAppls[MAX_PUPI], maxFrames[MAX_PUPI], protocolTypes[MAX_PUPI],
		fwis[MAX_PUPI], adcs[MAX_PUPI], fos[MAX_PUPI];
static char *t14BTR0, *t14BTR1, *t14BPC2PI, *t14BPI2PC;
static int t14BEOF, t14BSOF;
static CString t14BPUPI, t14BmaxFrame, t14BProtocolType, t14BCID;

/////////////////////////////////////////////////////////////////////////////
// CTab14443B dialog


// make it avalable to external functions:
static CTab14443B *RRdialog;

CTab14443B::CTab14443B(CWnd* pParent /*=NULL*/)
	: CDialog(CTab14443B::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTab14443B)
	m_i14BCommandRB = -1;
	m_s14BPUPINo = _T("");
	m_s14BPUPI = _T("");
	m_s14BAFI = _T("");
	m_s14BNumAppl = _T("");
	m_s14BBitRate = _T("");
	m_s14BMaxFrame = _T("8");
	m_s14BProtocolType = _T("1");
	m_s14BFWI = _T("");
	m_s14BADC = _T("");
	m_s14BFO = _T("");
	m_i14BFullPower = 0;
	m_s14BTR0 = _T("0");
	m_s14BTR1 = _T("0");
	m_b14BSOF = FALSE;
	m_b14BEOF = FALSE;
	m_s14BCID = _T("0");
	m_s14BPC2PI = _T("1");
	m_s14BPI2PC = _T("1");
	//}}AFX_DATA_INIT
	selectedProtocol = -1;
	RRdialog = this;
}


void CTab14443B::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTab14443B)
	DDX_Radio(pDX, IDC_14B_REQUB_RB, m_i14BCommandRB);
	DDX_Control(pDX, IDC_14B_PUPI, m_14BPUPI);
	DDX_Text(pDX, IDC_14B_PUPI_NO, m_s14BPUPINo);
	DDX_CBString(pDX, IDC_14B_PUPI, m_s14BPUPI);
	DDX_Text(pDX, IDC_14B_AFI, m_s14BAFI);
	DDX_Text(pDX, IDC_14B_NUM_APPL, m_s14BNumAppl);
	DDX_Text(pDX, IDC_14B_BIT_RATE, m_s14BBitRate);
	DDX_Text(pDX, IDC_14B_MAX_FRAME, m_s14BMaxFrame);
	DDX_Text(pDX, IDC_14B_PROTO_TYPE, m_s14BProtocolType);
	DDX_Text(pDX, IDC_14B_FWI, m_s14BFWI);
	DDX_Text(pDX, IDC_14B_ADC, m_s14BADC);
	DDX_Text(pDX, IDC_14B_FO, m_s14BFO);
	DDX_Radio(pDX, IDC_14B_FULL_POWER, m_i14BFullPower);
	DDX_Text(pDX, IDC_14B_TR0, m_s14BTR0);
	DDX_Text(pDX, IDC_14B_TR1, m_s14BTR1);
	DDX_Check(pDX, IDC_14B_SOF, m_b14BSOF);
	DDX_Check(pDX, IDC_14B_EOF, m_b14BEOF);
	DDX_Text(pDX, IDC_14B_CID, m_s14BCID);
	DDX_Text(pDX, IDC_14B_PC2PI, m_s14BPC2PI);
	DDX_Text(pDX, IDC_14B_PI2PC, m_s14BPI2PC);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTab14443B, CDialog)
	//{{AFX_MSG_MAP(CTab14443B)
	ON_BN_CLICKED(IDC_14B_REQUB_RB, On14BCommandRB)
	ON_BN_CLICKED(IDC_14B_ATTRIB_RB, On14BCommandRB)
	ON_BN_CLICKED(IDC_14B_WUPB_RB, On14BCommandRB)
	ON_BN_CLICKED(IDC_14B_HALT, On14BCommandRB)
	ON_BN_CLICKED(IDC_14B_SEND, On14BSend)
	ON_BN_CLICKED(IDC_14B_EXECUTE, On14BExecute)
	ON_CBN_SELCHANGE(IDC_14B_PUPI, On14BPUPI)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTab14443B message handlers

void CTab14443B::I14BResetTab() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	GetDlgItem(IDC_14B_REQUB_RB)->EnableWindow(TRUE);
	GetDlgItem(IDC_14B_ATTRIB_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_14B_WUPB_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_14B_HALT)->EnableWindow(FALSE);
	m_i14BCommandRB = 0;
	UpdateData(FALSE);	// Set screen values
	On14BCommandRB();
}

void I14BResetTab()
{
        RRdialog->I14BResetTab();
}

void CTab14443B::On14BCommandRB() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	// Disable all command dependant controls
	GetDlgItem(IDC_14B_TR0)->EnableWindow(FALSE);
	GetDlgItem(IDC_14B_TR1)->EnableWindow(FALSE);
	GetDlgItem(IDC_14B_EOF)->EnableWindow(FALSE);
	GetDlgItem(IDC_14B_SOF)->EnableWindow(FALSE);
	GetDlgItem(IDC_14B_PC2PI)->EnableWindow(FALSE);
	GetDlgItem(IDC_14B_PI2PC)->EnableWindow(FALSE);
	GetDlgItem(IDC_14B_CID)->EnableWindow(FALSE);
	GetDlgItem(IDC_14B_PUPI)->EnableWindow(FALSE);
	GetDlgItem(IDC_14B_MAX_FRAME)->EnableWindow(FALSE);
	// Selectively enable controls
	switch (m_i14BCommandRB) {
	  case ATTRIB:
		GetDlgItem(IDC_14B_TR0)->EnableWindow(TRUE);
		GetDlgItem(IDC_14B_TR1)->EnableWindow(TRUE);
		GetDlgItem(IDC_14B_EOF)->EnableWindow(TRUE);
		GetDlgItem(IDC_14B_SOF)->EnableWindow(TRUE);
		GetDlgItem(IDC_14B_PC2PI)->EnableWindow(TRUE);
		GetDlgItem(IDC_14B_PI2PC)->EnableWindow(TRUE);
		GetDlgItem(IDC_14B_CID)->EnableWindow(TRUE);
		GetDlgItem(IDC_14B_PUPI)->EnableWindow(TRUE);
		GetDlgItem(IDC_14B_MAX_FRAME)->EnableWindow(TRUE);
		break;
	  case HALT:
		GetDlgItem(IDC_14B_PUPI)->EnableWindow(TRUE);
		break;
	  }
	UpdateData(FALSE);	// Set screen values
}

static int send() 
{
	char cmd[CMD_LEN];
	strcpy(cmd, "0B");		// configure temporary ASIC
	if (t14BiFullPower)	strcat(cmd, "00");
	else				strcat(cmd, "FF");
	if (portWrite(cmd, 1, 0))
		return 1;

	strcpy(cmd, "100021010C");		// write to register
	portWrite(cmd, 0, 0);
	portRead(cmd, CMD_LEN);
	selectedProtocol = 1;
	newProtocol = 0;
	if (topResetFlags())
		return 1;
	return 0;
}

void CTab14443B::On14BSend() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	t14BiFullPower = m_i14BFullPower;
	if (! send())
		topResetFlagsGUI();
}

static int execute(int cmdno, char *reply)
{
	if ((selectedProtocol == -1 || newProtocol) && ! expert) {
		logAdd("Please, set Protocol", 0, 0);
		return 1;
		}
	// Assemble request
	char cmd[CMD_LEN], hex[4];
	int err = 0, tr0, tr1, val;
	switch (cmdno) {
	  case REQUB:
		strcpy(cmd, CC_REQUB);
		break;
	  case WUPB:
		strcpy(cmd, CC_WUPB);
		break;
	  case ATTRIB:
		strcpy(cmd, CC_ATTRIB);
		strcat(cmd, "1D");
		err += addHex(cmd, CMD_LEN, 8, t14BPUPI, "PUPI");
		// param 1
		err += (sscanf(t14BTR0, "%x", &tr0) != 1); tr0 &= 0x03;
		err += (sscanf(t14BTR1, "%x", &tr1) != 1); tr1 &= 0x03;
		val = tr0 << 6 | tr1 << 4 | t14BEOF << 3 | t14BSOF << 2;
		sprintf(hex, "%2.2x", val);
		strcat(cmd, hex);
		// param 2
		err += (sscanf(t14BPC2PI, "%x", &tr0) != 1); tr0 &= 0x03;
		err += (sscanf(t14BPI2PC, "%x", &tr1) != 1); tr1 &= 0x03;
		val = tr0 << 2 | tr1;
		sprintf(hex, "%1.1x", val);
		strcat(cmd, hex);
		err += addHex(cmd, CMD_LEN, -1, t14BmaxFrame, "Max Frame");
//		maxFr = t14BmaxFrame.GetBuffer(0);
		// param 3
		strcat(cmd, "0");
//		err += addHex(cmd, CMD_LEN, -1, t14BProtocolType, "Protocol Type");
		if (! strncmp(t14BProtocolType, "not ", 4)) strcat(cmd, "0");
		else					strcat(cmd, "1");
		// param 4
		strcat(cmd, "0");
		err += addHex(cmd, CMD_LEN, -1, t14BCID, "CID");
		break;
	  case HALT:
		strcpy(cmd, CC_HALT);
		err += addHex(cmd, CMD_LEN, 8, t14BPUPI, "PUPI");
		break;
	  }
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

static int getTagInfo (char *data, char *pupi, char *afi,
			int *numAppl, char *bitRate,
			int *maxFrame, int *protocolType, int *fwi,
			int *adc, int *fo)
{
	char tmp[10], *p = data, *d;
	int x, i;
	for (d = tmp, i = 0; i < 2; i++)	*d++ = *p++;
	*d = 0;
	if (strcmp(tmp, "50"))
	    return 1;
	for (d = tmp, i = 0; i < 8; i++)	*d++ = *p++;
	*d = 0;
	strcpy(pupi, tmp);
	// Application data
	for (d = tmp, i = 0; i < 2; i++)	*d++ = *p++;
	*d = 0;
	strcpy(afi, tmp);
	p += 4;
	for (d = tmp, i = 0; i < 2; i++)	*d++ = *p++;
	*d = 0;
	sscanf(tmp, "%x", numAppl);
	for (d = tmp, i = 0; i < 2; i++)	*d++ = *p++;
	*d = 0;
	strcpy(bitRate, tmp);
	tmp[0] = *p++; tmp[1] = 0;
/* VEN
char msg[100];
sprintf(msg, "-- tmp=%s", tmp);
logAdd(msg, 0, 0);
 */
	sscanf(tmp, "%x", maxFrame);
/* VEN
sprintf(msg, "-- maxFrame=%d", maxFrame);
logAdd(msg, 0, 0);
 */
	tmp[0] = *p++; tmp[1] = 0;
	sscanf(tmp, "%x", protocolType);
	tmp[0] = *p++; tmp[1] = 0;
	sscanf(tmp, "%x", fwi);
	tmp[0] = *p++; tmp[1] = 0;
	sscanf(tmp, "%x", &x);
	*adc = x >> 2;
	*fo = x & 0x3;
	return 0;
}

static void fwi2val(char *tmp, int fwi)
{
	double n;
	n = pow(2.0, (double) fwi);
	double val;
	val = 256 * 16 / 13.56e6 * n;
	val *= 1000;
	char *units;
	if (val > 1) {
		units = " ms";
		}
	else {
		val *= 1000;
		units = " us";
		}
	sprintf(tmp, "~ %g", val);
	if (tmp[5] == '.')	tmp[5] = 0;
	tmp[6] = 0;
	strcat(tmp, units);
}

void CTab14443B::showTagData(int n) 
{
	char tmp[10];
	m_s14BAFI = AFIs[n];
	sprintf(tmp, "%2.2X", numAppls[n]);
	m_s14BNumAppl = tmp;
	m_s14BBitRate = bitRates[n];
/*
	int k;
	switch (maxFrames[n]) {
	  case 0: k = 16;	break;
	  case 1: k = 24;	break;
	  case 2: k = 32;	break;
	  case 3: k = 40;	break;
	  case 4: k = 48;	break;
	  case 5: k = 64;	break;
	  case 6: k = 96;	break;
	  case 7: k = 128;	break;
	  case 8: k = 256;	break;
	  }
	sprintf(tmp, "%d", k);
 */
	sprintf(tmp, "%d", maxFrames[n]);
	m_s14BMaxFrame = tmp;
	if (protocolTypes[n])
		m_s14BProtocolType = "14443-4 compliant";
	else
		m_s14BProtocolType = "not 14443-4 compliant";
	fwi2val(tmp, fwis[n]);
	m_s14BFWI = tmp;
	if (adcs[n])	m_s14BADC = "7.9.3";
	else		m_s14BADC = "proprietary";
	m_s14BFO = "";
	if (fos[n] & 0x2) {
		m_s14BFO += "NAD";
		}
	if (fos[n] & 0x1) {
		if (fos[n] & 0x2)	m_s14BFO += " & ";
		m_s14BFO += "CID";
		}
}

void CTab14443B::On14BExecute() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	if ((selectedProtocol == -1 || newProtocol) && ! expert) {
		MessageBox("Please, set Protocol");
		return;
		}
	if (m_i14BCommandRB == -1) {
		MessageBox("Pleasse, select Command");
		return;
		}
	// Save data
	t14BPUPI = m_s14BPUPI.GetBuffer(0);
	t14BmaxFrame = m_s14BMaxFrame;
	t14BProtocolType = m_s14BProtocolType;
	t14BTR0 = m_s14BTR0.GetBuffer(0);
	t14BTR1 = m_s14BTR1.GetBuffer(0);
	t14BEOF = m_b14BEOF;
	t14BSOF = m_b14BSOF;
	t14BPC2PI = m_s14BPC2PI.GetBuffer(0);
	t14BPI2PC = m_s14BPI2PC.GetBuffer(0);
	t14BCID = m_s14BCID;

	char reply[CMD_LEN];
	if (execute(m_i14BCommandRB, reply))
		return;
	char data[64], pupi[PUPI_LEN], afi[4], bitRate[4], *p;
	int numAppl, maxFrame, protocolType, fwi, adc, fo;
	if (! m_i14BCommandRB) {	// Request
		strcpy(PUPIs[0],"");
		m_s14BAFI = " ";
		m_s14BNumAppl = " ";
		m_s14BBitRate = " ";
		m_s14BMaxFrame = " ";
		m_s14BProtocolType = " ";
		m_s14BFWI = " ";
		m_s14BADC = " ";
		m_s14BFO = "";

		m_14BPUPI.ResetContent();
		int index, cnt = 0, i;

		for (p = reply, i = 0; i < 4*MAX_PUPI; i++) {

			if (! (p = strchr(p, '[')))
				break;
			p++;
			if (*p && (*p == ']' || *p == 'z' || *p == 'Z'))
				continue;
			if (getXWord(&p, data))	continue;
			if (strlen(data) != 24) {
				logAdd("Short reply", 0, 0);
				continue;
				}
			getTagInfo (data, pupi, afi, &numAppl, bitRate,
				&maxFrame, &protocolType, &fwi, &adc, &fo);
/* VEN
char msg[100];
sprintf(msg, "maxFrame=%d", maxFrame);
logAdd(msg, 0, 0);
 */
			index = m_14BPUPI.AddString(pupi);
			m_14BPUPI.SetItemData(index, cnt);
			strcpy(PUPIs[cnt], pupi);
			strcpy(AFIs[cnt], afi);
			strcpy(bitRates[cnt], bitRate);
			numAppls[cnt] = numAppl;
			maxFrames[cnt] = maxFrame;
			protocolTypes[cnt] = protocolType;
			fwis[cnt] = fwi;
			adcs[cnt] = adc;
			fos[cnt] = fo;
			cnt++;
			showTagData(0);
			if (cnt >= MAX_PUPI)
				break;
			}
		m_s14BPUPI = PUPIs[0];
		m_14BPUPI.SetCurSel(0);
		char num[8];
		sprintf(num, "%d", cnt);
		m_s14BPUPINo = num;
		
		GetDlgItem(IDC_14B_PUPI)->EnableWindow(TRUE);
		UpdateData(FALSE);	// Set screen values
		GetDlgItem(IDC_14B_ATTRIB_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_14B_WUPB_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_14B_HALT)->EnableWindow(TRUE);
 		}
}

int parse14443B (int doit, char *line, char *path, int lineno, char *reply)
{
	int err = 0, cmd, cmdx, val = 0;
	char word[128], *p = line;
	static char tt14BTR0[4], tt14BTR1[4], tt14BPC2PI[4], tt14BPI2PC[4];
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
	  case REQUB: case WUPB: case HALT:
		if (doit)
			execute(cmd, reply);
		break;
	  case PROTOCOL:
		if (doit)
			send();
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
		if (cmd == D_EOF || cmd == D_EOF) {
			val == (! strcmp(data, "0")) ? 1 : 0;
			}
		if (cmd == D_POWER) {
			cmdx = lookup(word, dataPowerMode);
			if (cmdx == -1) {
				msg.Format("Invalid power option '%s' in line %d", word, lineno);
				hopa(msg);
				return 1;
				}
			}
		char *d = data.GetBuffer(0);
		if (cmd == D_TR0 || cmd == D_TR1 || cmd == D_PC2PI || cmd == D_PI2PC) {
			char *d1 = d;
			d1++;
			*d1 = 0;
			}
		if (doit) {
			switch (cmd) {
			  case D_PUPI:
				t14BPUPI = data;
				break;
			  case D_MAX_FR:
				t14BmaxFrame = data;
				break;
			  case D_PROTO_TYPE:
				t14BProtocolType = data;
				break;
			  case D_TR0:
				strcpy(tt14BTR0, d);
				t14BTR0 = tt14BTR0;
				break;
			  case D_TR1:
				strcpy(tt14BTR1, d);
				t14BTR1 = tt14BTR1;
				break;
			  case D_EOF:
				t14BEOF = val;
				break;
			  case D_SOF:
				t14BSOF = val;
				break;
			  case D_PC2PI:
				strcpy(tt14BPC2PI, d);
				t14BPC2PI = tt14BPC2PI;
				break;
			  case D_PI2PC:
				strcpy(tt14BPC2PI, d);
				t14BPC2PI = tt14BPC2PI;
				break;
			  case D_CID:
				t14BPUPI = data;
				break;
			  case D_POWER:
				t14BiFullPower = cmdx;
				break;
			  }
			}
		break;
	  }
	return 0;
}

int find14443BTags (TagID *ids, int num)
{
	if (MAX_ID_LEN < PUPI_LEN)
		return 1;
	if (send())
		return 1;
	char reply[CMD_LEN];
#if NO_COM
	strcpy(reply, "_14443B_");
#endif
	if (execute(REQUB, reply))
		return 1;
	int cnt = 0, i;
	TagID *id = ids;
	char data[64], pupi[PUPI_LEN], afi[4], bitRate[4], *p;
	int numAppl, maxFrame, protocolType, fwi, adc, fo;
	for (p = reply, i = 0; i < 4*MAX_PUPI; i++) {
		if (! (p = strchr(p, '[')))
			break;
		p++;
		if (*p && (*p == ']' || *p == 'z' || *p == 'Z'))
			continue;
		if (getXWord(&p, data))	continue;
		if (strlen(data) != 24) {
			logAdd("Short reply", 0, 0);
			continue;
			}
		getTagInfo(data, pupi, afi, &numAppl, bitRate,
				&maxFrame, &protocolType, &fwi, &adc, &fo);
		strcpy(id->id, pupi);
		id++;
		cnt++;
		if (cnt >= num)
			break;
		}
	id->id[0] = 0;
	return 0;
}

void CTab14443B::On14BPUPI() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	int index = m_14BPUPI.GetCurSel(), i;
	char sel[PUPI_LEN];
	m_14BPUPI.GetLBText(index, sel);
	CString cstr;
	for (i = 0; i < MAX_PUPI; i++) {
		if (! strcmp(sel, PUPIs[i]))
			break;
		}
	if (i < MAX_PUPI) {
		m_s14BPUPI = PUPIs[i];
		}
	showTagData(i);
	UpdateData(FALSE);	// Set screen values
}
