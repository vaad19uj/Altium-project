// Tab14443A.cpp : implementation file
//
/////////////////////////////////////////////////////
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
#include "Tab14443A.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	DATA_LEN	256
#define MAX_UID		32
#define UID_LEN		68

// 14443A Command codes

#define	ANTICOLLISION	0
#define	SELECT		1
#define	HLTA		2
#define	DESELECT	3
#define	RATS		4
#define	PPS		5
#define	PROTOCOL	6
#define	SET			7


static char *commands[] = {	"anticollision",
				"select",
				"HLTA",
				"deselect",
				"RATS",
				"PPS",
				"set_protocol",
				"set",
				NULL};

static char *dataPowerMode[] = {	"full",	"half",
				NULL};

#define	F_ADDRESS	0
#define	F_INFO	1

#define	D_UID		0
#define	D_FSDI		1
#define	D_CID		2
#define	D_DR		3
#define	D_DS		4
#define	D_POWER		5

static char *dataKeys[] = {	"UID",
				"FSDI",
				"CID",
				"DR",
				"DS",
				"power",
				NULL};


static char UIDs[MAX_UID][UID_LEN], UIDsBCC[MAX_UID][UID_LEN];
static int selectedProtocol = -1, t14AiFullPower, noUID = 0;
static CString t14AUID, t14AFSDI, t14ACID, t14ADRI, t14ADSI;

/////////////////////////////////////////////////////////////////////////////
// CTab14443A dialog


// make it avalable to external functions:
static CTab14443A *RRdialog;

CTab14443A::CTab14443A(CWnd* pParent /*=NULL*/)
	: CDialog(CTab14443A::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTab14443A)
	m_i14ACommandRB = -1;
	m_s14AVersion = _T("");
	m_s14AUIDNo = _T("");
	m_s14AUID = _T("");
	m_s14AFSDI = _T("1");
	m_s14ACID = _T("0");
	m_s14AFSCI = _T("");
	m_s14ADR = _T("0");
	m_s14ADS = _T("0");
	m_s14AFWI = _T("");
	m_s14ASFGI = _T("");
	m_i14AFullPower = 0;
	m_b14ANADsupp = FALSE;
	m_b14ACIDsupp = FALSE;
	m_s14AHistory = _T("");
	//}}AFX_DATA_INIT
	selectedProtocol = -1;
	RRdialog = this;
}


void CTab14443A::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTab14443A)
	DDX_Control(pDX, IDC_14A_UID, m_14AUID);
	DDX_Radio(pDX, IDC_14A_ANTICOLL_RB, m_i14ACommandRB);
	DDX_Text(pDX, IDC_14A_UID_NO, m_s14AUIDNo);
	DDX_Text(pDX, IDC_14A_FSDI, m_s14AFSDI);
	DDX_Text(pDX, IDC_14A_CID, m_s14ACID);
	DDX_Text(pDX, IDC_14A_FSCI, m_s14AFSCI);
	DDX_Text(pDX, IDC_14A_DR, m_s14ADR);
	DDX_Text(pDX, IDC_14A_DS, m_s14ADS);
	DDX_Text(pDX, IDC_14A_FWI, m_s14AFWI);
	DDX_Text(pDX, IDC_14A_SFGI, m_s14ASFGI);
	DDX_Radio(pDX, IDC_14A_FULL_POWER, m_i14AFullPower);
	DDX_Check(pDX, IDC_14A_NAD_SUPP, m_b14ANADsupp);
	DDX_Check(pDX, IDC_14A_CID_SUPP, m_b14ACIDsupp);
	DDX_Text(pDX, IDC_14A_HISTORY, m_s14AHistory);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTab14443A, CDialog)
	//{{AFX_MSG_MAP(CTab14443A)
	ON_BN_CLICKED(IDC_14A_EXECUTE, On14AExecute)
	ON_BN_CLICKED(IDC_14A_ANTICOLL_RB, On14ACommandRB)
	ON_CBN_SELCHANGE(IDC_14A_UID, On14AUID)
	ON_BN_CLICKED(IDC_14A_SELECT_RB, On14ACommandRB)
	ON_BN_CLICKED(IDC_14A_HLTA_RB, On14ACommandRB)
	ON_BN_CLICKED(IDC_14A_DESELECT_RB, On14ACommandRB)
	ON_BN_CLICKED(IDC_14A_RATS_RB, On14ACommandRB)
	ON_BN_CLICKED(IDC_14A_PPS_RB, On14ACommandRB)
	ON_BN_CLICKED(IDC_14A_PROTOCOL, On14ASend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTab14443A message handlers

void CTab14443A::I14AResetTab() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	GetDlgItem(IDC_14A_ANTICOLL_RB)->EnableWindow(TRUE);
	GetDlgItem(IDC_14A_SELECT_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_14A_HLTA_RB)->EnableWindow(FALSE);

//	GetDlgItem(IDC_14A_DESELECT_RB)->EnableWindow(FALSE);
//	GetDlgItem(IDC_14A_RATS_RB)->EnableWindow(FALSE);
//	GetDlgItem(IDC_14A_PPS_RB)->EnableWindow(FALSE);

	GetDlgItem(IDC_14A_DESELECT_RB)->EnableWindow(TRUE);
	GetDlgItem(IDC_14A_RATS_RB)->EnableWindow(TRUE);
	GetDlgItem(IDC_14A_PPS_RB)->EnableWindow(TRUE);
	



//	GetDlgItem(IDC_14A_UID)->EnableWindow(FALSE);
	m_i14ACommandRB = 0;
	UpdateData(FALSE);	// Set screen values
	On14ACommandRB();
}

void I14AResetTab() 
{
	RRdialog->I14AResetTab();
}

void CTab14443A::On14ACommandRB() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	// Disable all command dependant controls
	GetDlgItem(IDC_14A_UID)->EnableWindow(FALSE);
	GetDlgItem(IDC_14A_FSDI)->EnableWindow(FALSE);
	GetDlgItem(IDC_14A_CID)->EnableWindow(FALSE);
	GetDlgItem(IDC_14A_DR)->EnableWindow(FALSE);
	GetDlgItem(IDC_14A_DS)->EnableWindow(FALSE);
	// Selectively enable controls
	switch (m_i14ACommandRB) {
	  case SELECT:
		GetDlgItem(IDC_14A_FSDI)->EnableWindow(FALSE);
		GetDlgItem(IDC_14A_CID)->EnableWindow(FALSE);
		GetDlgItem(IDC_14A_UID)->EnableWindow(TRUE);
		break;
	  case RATS:
		GetDlgItem(IDC_14A_FSDI)->EnableWindow(TRUE);
		GetDlgItem(IDC_14A_CID)->EnableWindow(TRUE);
		break;
	  case PPS:
		GetDlgItem(IDC_14A_DR)->EnableWindow(TRUE);
		GetDlgItem(IDC_14A_DS)->EnableWindow(TRUE);
		break;
	  }
}

static int send() 
{
	char cmd[CMD_LEN], hex[4];
#if ! SETUP_7961 || SETUP_7960
	strcpy(cmd, "0A");		// configure temporary ASIC
	if (t14AiFullPower)	strcat(cmd, "00");
	else				strcat(cmd, "FF");
	if (portWrite(cmd, 1, 0))
		return 1;
#endif

	int reg01 = 0;	// Kje ga dobim?
	reg01 |= 0x09;
	sprintf(hex, "%2.2X", reg01);
	strcpy(cmd, "10");		// write to register
#if SETUP_7961
	if (t14AiFullPower)	strcat(cmd, "0031");
	else				strcat(cmd, "0021");
#endif
	strcat(cmd, "01");		// register #
	strcat(cmd, hex);
	portWrite(cmd, 0, 0);
	portRead(cmd, CMD_LEN);
	selectedProtocol = 1;
	newProtocol = 0;
	if (topResetFlags())
		return 1;
	return 0;
}

void CTab14443A::On14ASend() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	t14AiFullPower = m_i14AFullPower;
	if (! send())
		topResetFlagsGUI();
}

static int execute(int cmdno, char *reply)
{
	if ((selectedProtocol == -1 || newProtocol) && ! expert) {
		logAdd("Please, set Protocol", 0, 0);
		return 1;
		}
	char cmd[CMD_LEN], hex[8], *p;
	int i;
	if (! cmdno)	strcpy(cmd, "A001");
	else if (cmdno == 1)	strcpy(cmd, "A2");
	else		strcpy(cmd, "18");
	int err = 0, s, r, divisor;
	switch (cmdno) {
	  case ANTICOLLISION:
		break;
	  case SELECT:
		p = t14AUID.GetBuffer(0);
		for (i = 0; i < noUID; i++)
			if (! strcmp(UIDs[i], p))	break;
		if (i >= noUID)	err++;
		else			strcat(cmd, UIDsBCC[i]);
		break;
	  case HLTA:
		strcat(cmd, "5000");
		break;
	  case DESELECT:
		strcat(cmd, "C0");
		break;
	  case RATS:
		strcat(cmd, "E0");
		err += addHex(cmd, CMD_LEN, -1, t14AFSDI, "FSDI");
		err += addHex(cmd, CMD_LEN, -1, t14ACID, "FCID");
		break;
	  case PPS:
		strcat(cmd, "D");
		err += addHex(cmd, CMD_LEN, -1, t14ACID, "FCID");
		strcat(cmd, "11");
		strcat(cmd, "0");
		sscanf(t14ADRI.GetBuffer(0), "%x", &r); r &= 0x03;
		sscanf(t14ADSI.GetBuffer(0), "%x", &s); s &= 0x03;
		divisor = s << 2 | r;
		sprintf(hex, "%1.1X", divisor);
		strcat(cmd, hex);
		break;
	  default:
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

void I14AuidRemoveBCC(char *uid2, char *uid)
{
	char *p, *d;
	int i, l = strlen(uid);
	p = uid;
	d = uid2;
	for (i = 0; i < 6; i++)	*d++ = *p++;
	if (l < 11) {
		*d++ = *p++;
		*d++ = *p++;
		*d = 0;
		return;
		}
	p += 2;
	for (i = 0; i < 6; i++)	*d++ = *p++;
	if (l < 19) {
		*d++ = *p++;
		*d++ = *p++;
		*d = 0;
		return;
		}
	p += 2;
	for (i = 0; i < 8; i++)	*d++ = *p++;
	*d = 0;
}

void CTab14443A::On14AExecute() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	if ((selectedProtocol == -1 || newProtocol) && ! expert) {
		MessageBox("Please, set Protocol");
		return;
		}
	if (m_i14ACommandRB == -1) {
		MessageBox("Pleasse, select Command");
		return;
		}
	// Save data
	t14AUID = m_s14AUID;
	t14AFSDI = m_s14AFSDI;
	t14ACID = m_s14ACID;
	t14ADRI = m_s14ADR;
	t14ADSI = m_s14ADS;

	char reply[CMD_LEN];
	if (execute(m_i14ACommandRB, reply))
		return;

	// check error code
	char *p;
	if ((p = strstr(reply, "[x"))) {
		return;
		}
	else
		p = reply;

	char data[DATA_LEN+4], uid[34], uid2[34], tmp[4];
	int nibble;
	static int retry = 0;
	// Show data
	if (m_i14ACommandRB == ANTICOLLISION) {	// Anticollision
		m_14AUID.ResetContent();
		int index, cnt = 0, len, i;
		p = reply;
		for (i = 0; i < 4*MAX_UID; i++) {
			if (! (p = strchr(p, '[')))
				break;
			p++;
			if (*p && (*p == ']' || *p == 'z' || *p == 'Z'))
				continue;
			if (getXWord(&p, uid))	continue;
			len = strlen(uid);
			if (! (len == 10 || len == 18 || len == 26)) {
				hopa("Invalid UID length");
				continue;
				}
			I14AuidRemoveBCC(uid2, uid);
			index = m_14AUID.AddString(uid2);
			m_14AUID.SetItemData(index, cnt);
			strcpy(UIDs[cnt], uid2);
			strcpy(UIDsBCC[cnt], uid);
			cnt++;
			if (cnt >= MAX_UID)
				break;
			}
		m_14AUID.SetCurSel(0);
		m_s14AUID = UIDs[0];
		char num[8];
		sprintf(num, "%d", cnt);
		m_s14AUIDNo = num;
		noUID = cnt;
		UpdateData(FALSE);	// Set screen values
 		}
/*
	if (m_i14ACommandRB == RATS && (p = strchr(p, '['))) {
		p++;
		if (getXWord(&p, data) && ! retry) {
			retry = 1;
			On14AExecute() ;
			retry = 0;
			return;
			}
 		}
 */
	if (m_i14ACommandRB == RATS && (p = strchr(p, '['))) {
		// ISO/IEC FDIS 14443-4:2000(E); chapter 5.2.1; pp. 7
		p++;
		if (getXWord(&p, data))	return;
		if (strlen(data) < 4) {
			hopa("Short RATS reply");
			return;
			}
		p = data;
		p += 2;	// (TL) skip length
		int tc, tb, ta;
		nibble = (*p >= 'A')	? (10 + (*p - 'A'))
					: (*p - '0');
		tc = nibble & 0x04;
		tb = nibble & 0x02;
		ta = nibble & 0x01;
		p++;
		tmp[0] = *p++;
		tmp[1] = 0;
		m_s14AFSCI = tmp;
		if (ta) {
			nibble = (*p >= 'A')	? (10 + (*p - 'A'))
						: (*p - '0');
			nibble &= 0x07;
/*
			if (nibble > 3)	m_s14ADS = "8";
			else if (nibble > 1)	m_s14ADS = "4";
			else if (nibble > 0)	m_s14ADS = "2";
			else			m_s14ADS = "0";
 */
			sprintf(tmp, "%x", nibble);
			m_s14ADS = tmp;
			p++;
			nibble = (*p >= 'A')	? (10 + (*p - 'A'))
						: (*p - '0');
			nibble &= 0x07;
/*
			if (nibble > 3)	m_s14ADR = "8";
			else if (nibble > 1)	m_s14ADR = "4";
			else if (nibble > 0)	m_s14ADR = "2";
			else			m_s14ADR = "0";
 */
			sprintf(tmp, "%x", nibble);
			m_s14ADR = tmp;
			p++;
			}
		if (tb) {
			tmp[0] = *p++;
			m_s14AFWI = tmp;
			tmp[0] = *p++;
			m_s14ASFGI = tmp;
			}
		m_b14ACIDsupp = 0;
		m_b14ANADsupp = 0;
		if (tc) {
			p++;
			m_s14AFWI = tmp;
			nibble = (*p >= 'A')	? (10 + (*p - 'A'))
						: (*p - '0');
			if (nibble & 0x02)	m_b14ACIDsupp = 1;
			if (nibble & 0x01)	m_b14ANADsupp = 1;
			p++;
			}
		UpdateData(FALSE);	// Set screen values
		m_s14AHistory = p;
 		}
	if (m_i14ACommandRB == PPS && (p = strchr(p, '['))) {
		UpdateData(FALSE);	// Set screen values
 		}

	// Show data
	// State table described in ISO?IEC FDIS 14443-3:2000(E) p.10
	if (! m_i14ACommandRB && noUID) {	// Anticollision
		GetDlgItem(IDC_14A_ANTICOLL_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_14A_SELECT_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_14A_HLTA_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_14A_DESELECT_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_14A_RATS_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_14A_PPS_RB)->EnableWindow(FALSE);
		//m_i14ACommandRB = 1;
		}
	else if (m_i14ACommandRB == SELECT && strstr(reply, "[20]")) {
		GetDlgItem(IDC_14A_ANTICOLL_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_14A_SELECT_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_14A_HLTA_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_14A_DESELECT_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_14A_RATS_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_14A_PPS_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_14A_FSDI)->EnableWindow(TRUE);
		GetDlgItem(IDC_14A_CID)->EnableWindow(TRUE);
		m_i14ACommandRB = 4;
		}
	else if (m_i14ACommandRB == HLTA || m_i14ACommandRB == DESELECT) {
		GetDlgItem(IDC_14A_ANTICOLL_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_14A_SELECT_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_14A_HLTA_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_14A_DESELECT_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_14A_RATS_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_14A_PPS_RB)->EnableWindow(FALSE);
		m_i14ACommandRB = 0;
		}
	else if (m_i14ACommandRB == RATS) {
		GetDlgItem(IDC_14A_ANTICOLL_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_14A_SELECT_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_14A_HLTA_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_14A_DESELECT_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_14A_RATS_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_14A_PPS_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_14A_FSDI)->EnableWindow(FALSE);
		GetDlgItem(IDC_14A_CID)->EnableWindow(FALSE);
		GetDlgItem(IDC_14A_DR)->EnableWindow(TRUE);
		GetDlgItem(IDC_14A_DS)->EnableWindow(TRUE);
		m_i14ACommandRB = 5;
		}
	else if (m_i14ACommandRB == PPS) {
		GetDlgItem(IDC_14A_ANTICOLL_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_14A_SELECT_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_14A_HLTA_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_14A_DESELECT_RB)->EnableWindow(TRUE);
		GetDlgItem(IDC_14A_RATS_RB)->EnableWindow(FALSE);
		GetDlgItem(IDC_14A_PPS_RB)->EnableWindow(FALSE);
		m_i14ACommandRB = 0;
		}
	UpdateData(FALSE);	// Set screen values
}

int parse14443A (int doit, char *line, char *path, int lineno, char * reply)
{
	int err = 0, cmd, cmdx;
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
	  case ANTICOLLISION: case SELECT: case HLTA: case DESELECT:
	  case RATS: case PPS:
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
		if (cmd == D_POWER){
			cmdx = lookup(word, dataPowerMode);
			if (cmdx == -1) {
				msg.Format("Invalid power option '%s' in line %d", word, lineno);
				hopa(msg);
				return 1;
				}
			}
		if (doit) {
			switch (cmd) {
			  case D_UID:
				t14AUID = data;
				break;
			  case D_FSDI:
				t14AFSDI = data;
				break;
			  case D_CID:
				t14ACID = data;
				break;
			  case D_DR:
				t14ADRI = data;
				break;
			  case D_DS:
				t14ADSI = data;
				break;
			  case D_POWER:
				t14AiFullPower = cmdx;
				break;
			  }
			}
		break;
	  }
	return 0;
}

int find14443ATags (TagID *ids, int num)
{
	if (MAX_ID_LEN < UID_LEN)
		return 1;
	if (send())
		return 1;
	char reply[CMD_LEN];
#if NO_COM
	strcpy(reply, "_14443A_");
#endif
	if (execute(ANTICOLLISION, reply))
		return 1;
	char uid[34], uid2[34], *p;
	int cnt = 0, len, i;
	TagID *id = ids;
	p = reply;
	for (i = 0; i < 4*MAX_UID; i++) {
		if (! (p = strchr(p, '[')))
			break;
		p++;
		if (*p && (*p == ']' || *p == 'z' || *p == 'Z'))
			continue;
		if (getXWord(&p, uid))	continue;
		len = strlen(uid);
		if (! (len == 10 || len == 18 || len == 26))
			continue;
		I14AuidRemoveBCC(uid2, uid);
		strcpy(id->id, uid);
		id++;
		cnt++;
		if (cnt >= num)	break;
		}
	id->id[0] = 0;
	return 0;
}

void CTab14443A::On14AUID() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	int index = m_14AUID.GetCurSel(), i;
	char sel[UID_LEN];
	m_14AUID.GetLBText(index, sel);
	CString cstr;
	for (i = 0; i < MAX_UID; i++) {
		if (! strcmp(sel, UIDs[i]))
			break;
		}
	if (i < MAX_UID) {
		m_s14AUID = UIDs[i];
		}
	UpdateData(FALSE);	// Set screen values
}
