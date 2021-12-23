// TabEPC.cpp : implementation file
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
#include "TabEPC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	DATA_LEN	256
#define MAX_ID		32
#define ID_LEN		68

// EPC Command codes
#define	CC_BEGIN	0x00
#define	CC_RESET	0x12
#define	CC_WRITE	0x01
#define	CC_LOCK		0x20
#define	CC_LOCK_ALL	0x21
#define	CC_DESTROY	0x02

#define	BEGIN		0
#define	RESET		1
#define	WRITE		2
#define	LOCK		3
#define	LOCK_ALL	4
#define	DESTROY		5
#define	PROTOCOL	6
#define	SET		7


static char *commands[] = {	"begin_round",
				"complete_reset",
				"block_write",
				"block_lock",
				"lock_whole_memory",
				"destroy",
				"set_protocol",
				"set",
				NULL};

static char *slotNoOptions[] = {	"0",	"4",
				NULL};
static char *dataPowerMode[] = {	"full",	"half",
				NULL};

#define	D_ID		0
#define	D_SLOT_NO	1
#define	D_PASS		2
#define	D_BLOCK_NUM	3
#define	D_DATA		4
#define	D_POWER		5

static char *dataKeys[] = {	"EPC",
				"number_of_slots",
				"password",
				"block_number",
				"data",
				"power",
				NULL};


static char IDs[MAX_ID][ID_LEN];
static int selectedProtocol = -1,
		tEPCiFullPower;
static CString tEPCID, tEPCSlotNo, tEPCPass, tEPCBlockNum, tEPCData;

/////////////////////////////////////////////////////////////////////////////
// CTabEPC dialog


// make it avalable to external functions:
static CTabEPC *RRdialog;

CTabEPC::CTabEPC(CWnd* pParent /*=NULL*/)
	: CDialog(CTabEPC::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabEPC)
	m_iEPCCommandRB = -1;
	m_sEPCIDNo = _T("");
	m_sEPCID = _T("");
	m_sEPCSlotNo = _T("4");
	m_sEPCPass = _T("");
	m_sEPCBlockNum = _T("");
	m_sEPCData = _T("");
	m_iEPCFullPower = 0;
	//}}AFX_DATA_INIT
	selectedProtocol = -1;
	RRdialog = this;
}

void CTabEPC::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabEPC)
	DDX_Control(pDX, IDC_EPC_ID, m_EPCID);
	DDX_Radio(pDX, IDC_EPC_BEGIN_RB, m_iEPCCommandRB);
	DDX_Text(pDX, IDC_EPC_ID_NO, m_sEPCIDNo);
	DDX_CBString(pDX, IDC_EPC_ID, m_sEPCID);
	DDX_Text(pDX, IDC_EPC_SLOT_NO, m_sEPCSlotNo);
	DDX_Text(pDX, IDC_EPC_PASS, m_sEPCPass);
	DDX_Text(pDX, IDC_EPC_BLOCK_NUM, m_sEPCBlockNum);
	DDX_Text(pDX, IDC_EPC_DATA, m_sEPCData);
	DDX_Radio(pDX, IDC_EPC_FULL_POWER, m_iEPCFullPower);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabEPC, CDialog)
	//{{AFX_MSG_MAP(CTabEPC)
	ON_BN_CLICKED(IDC_EPC_BEGIN_RB, OnEPCCommandRB)
	ON_BN_CLICKED(IDC_EPC_RESET_RB, OnEPCCommandRB)
	ON_BN_CLICKED(IDC_EPC_WRITE_RB, OnEPCCommandRB)
	ON_BN_CLICKED(IDC_EPC_LOCK_RB, OnEPCCommandRB)
	ON_BN_CLICKED(IDC_EPC_LOCK_ALL_RB, OnEPCCommandRB)
	ON_BN_CLICKED(IDC_EPC_DESTROY_RB, OnEPCCommandRB)
	ON_BN_CLICKED(IDC_EPC_PROTOCOL, OnEPCSend)
	ON_BN_CLICKED(IDC_EPC_EXECUTE, OnEPCExecute)
//	ON_CBN_SELCHANGE(IDC_EPC_ID, OnEPCID)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabEPC message handlers

void CTabEPC::EPCResetTab() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	GetDlgItem(IDC_EPC_BEGIN_RB)->EnableWindow(TRUE);
	GetDlgItem(IDC_EPC_RESET_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_EPC_WRITE_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_EPC_LOCK_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_EPC_LOCK_ALL_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_EPC_DESTROY_RB)->EnableWindow(FALSE);
	m_iEPCCommandRB = 0;
	UpdateData(FALSE);	// Set screen values
	OnEPCCommandRB();
}

void EPCResetTab() 
{
	RRdialog->EPCResetTab();
}

void CTabEPC::OnEPCCommandRB() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	// Disable all command dependant controls
	GetDlgItem(IDC_EPC_ID)->EnableWindow(FALSE);
	GetDlgItem(IDC_EPC_BLOCK_NUM)->EnableWindow(FALSE);
	GetDlgItem(IDC_EPC_DATA)->EnableWindow(FALSE);
	// Selectively enable controls
	switch (m_iEPCCommandRB) {
	  case BEGIN:
		break;
	  case RESET:
		break;
	  case WRITE:
		GetDlgItem(IDC_EPC_BLOCK_NUM)->EnableWindow(TRUE);
		GetDlgItem(IDC_EPC_DATA)->EnableWindow(TRUE);
		break;
	  case LOCK:
		GetDlgItem(IDC_EPC_BLOCK_NUM)->EnableWindow(TRUE);
		break;
	  case LOCK_ALL:
		break;
	  case DESTROY:
		GetDlgItem(IDC_EPC_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_EPC_PASS)->EnableWindow(FALSE);
		break;
	  }
}

static int send() 
{
	char cmd[CMD_LEN];
#if ! SETUP_7961 || SETUP_7960
	strcpy(cmd, "0B");		// configure temporary ASIC
	if (tEPCiFullPower)	strcat(cmd, "00");
	else				strcat(cmd, "FF");
	if (portWrite(cmd, 1, 0))
		return 1;
#endif

	strcpy(cmd, "10");		// write to register
#if SETUP_7961
	if (tEPCiFullPower)	strcat(cmd, "0031");
	else				strcat(cmd, "0021");
#endif
	strcat(cmd, "01");		// register #
	strcat(cmd, "14");
	portWrite(cmd, 0, 0);
	portRead(cmd, CMD_LEN);
	selectedProtocol = 1;
	newProtocol = 0;
	if (topResetFlags())
		return 1;
	return 0;
}

void CTabEPC::OnEPCSend() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	tEPCiFullPower = m_iEPCFullPower;
	if (! send())
		topResetFlagsGUI();
}

static int execute(int cmdno, char *reply)
{
	if ((selectedProtocol == -1 || newProtocol) && ! expert) {
		logAdd("Please, set Protocol", 0, 0);
		return 1;
		}
	char cmd[CMD_LEN], hex[8];
	short unsigned int h = 0;
	CString info;
	switch (cmdno) {
	  case RESET:		h = CC_RESET;	break;
	  case WRITE:		h = CC_WRITE;	break;
	  case LOCK:		h = CC_LOCK;	break;
	  case LOCK_ALL:	h = CC_LOCK_ALL;	break;
	  case DESTROY:		h = CC_DESTROY;	break;
	  }
	sprintf(hex, "%4.4x", h);
	int err = 0, size = 1;
	if (! cmdno)	strcpy(cmd, "54");
	else		strcpy(cmd, "18");
	if (! cmdno) {
		if (! strcmp(tEPCSlotNo, "0"))	strcat(cmd, "00");
		else				strcat(cmd, "01");
		}
	else		strcat(cmd, hex);
	switch (cmdno) {
	  case WRITE:
		err += addHex(cmd, CMD_LEN, 2, tEPCBlockNum, "Block Number");
		err += addHex(cmd, CMD_LEN, 2*size, tEPCData, "Data");
		break;
	  case LOCK:
		err += addHex(cmd, CMD_LEN, 2, tEPCBlockNum, "Block Number");
		break;
	  case DESTROY:
		err += addHex(cmd, CMD_LEN, 0, tEPCID, "EPC ID");
		err += addHex(cmd, CMD_LEN, 6, tEPCPass, "Password");
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


void CTabEPC::OnEPCExecute() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	if ((selectedProtocol == -1 || newProtocol) && ! expert) {
		MessageBox("Please, set Protocol");
		return;
		}
	if (m_iEPCCommandRB == -1) {
		MessageBox("Pleasse, select Command");
		return;
		}
	// Save data
	tEPCID = m_sEPCID;
	tEPCSlotNo = m_sEPCSlotNo;
	tEPCPass = m_sEPCPass;
	tEPCBlockNum = m_sEPCBlockNum;
	tEPCData = m_sEPCData;

	char reply[CMD_LEN];
	if (execute(m_iEPCCommandRB, reply))
		return;

	// check error code
	char *p;
	if ((p = strstr(reply, "[x"))) {
		return;
		}
	else
		p = reply;

	char data[DATA_LEN+4], uid[34];
	// Show data
	if (! m_iEPCCommandRB) {	// ID poll: load available IDs
		m_EPCID.ResetContent();
		int index, cnt = 0, i;
		p = reply;
		for (i = 0; i < 4*MAX_ID; i++) {
			if (! (p = strchr(p, '[')))
				break;
			p++;
			if (*p && (*p == ']' || *p == 'z' || *p == 'Z'))
				continue;
			p += 4;
			if (getXWord(&p, data))	continue;
			strcpy(uid, data);
			index = m_EPCID.AddString(uid);
			m_EPCID.SetItemData(index, cnt);
			strcpy(IDs[cnt], uid);
			cnt++;
			if (cnt >= MAX_ID)
				break;
			}
		m_EPCID.SetCurSel(0);
		m_sEPCID = IDs[0];
		char num[8];
		sprintf(num, "%d", cnt);
		m_sEPCIDNo = num;
		if (cnt) {
			GetDlgItem(IDC_EPC_RESET_RB)->EnableWindow(FALSE);
			GetDlgItem(IDC_EPC_WRITE_RB)->EnableWindow(FALSE);
			GetDlgItem(IDC_EPC_LOCK_RB)->EnableWindow(FALSE);
			GetDlgItem(IDC_EPC_LOCK_ALL_RB)->EnableWindow(FALSE);
			GetDlgItem(IDC_EPC_DESTROY_RB)->EnableWindow(FALSE);
			}
		UpdateData(FALSE);	// Set screen values
 		}
}

int parseEPC (int doit, char *line, char *path, int lineno, char * reply)
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
	  case BEGIN: case RESET: case WRITE: case LOCK:
	  case LOCK_ALL: case DESTROY:
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
		if (cmd == D_POWER) {
			cmdx = lookup(word, dataPowerMode);
			if (cmdx == -1) {
				msg.Format("Invalid power option '%s' in line %d", word, lineno);
				hopa(msg);
				return 1;
				}
			}
		if (cmd == D_SLOT_NO) {
			cmdx = lookup(word, slotNoOptions);
			if (cmdx == -1) {
				msg.Format("Invalid number_of_slots option '%s' in line %d", word, lineno);
				hopa(msg);
				return 1;
				}
			}
		if (doit) {
			switch (cmd) {
			  case D_ID:
				tEPCID = data;
				break;
			  case D_SLOT_NO:
				tEPCSlotNo = data;
				break;
			  case D_PASS:
				tEPCPass = data;
				break;
			  case D_BLOCK_NUM:
				tEPCBlockNum = data;
				break;
			  case D_DATA:
				tEPCData = data;
				break;
			  case D_POWER:
				tEPCiFullPower = cmdx;
				break;
			  }
			}
		break;
	  }
	return 0;
}

int findEPCTags (TagID *ids, int num)
{
	if (MAX_ID_LEN < ID_LEN)
		return 1;
	if (send())
		return 1;
	char reply[CMD_LEN];
#if NO_COM
	strcpy(reply, "_EPC_");
#endif
	tEPCSlotNo = "4";
	if (execute(BEGIN, reply))
		return 1;
//	char data[DATA_LEN+4], uid[34], *p;
	char data[DATA_LEN+4], *p;
	int cnt = 0, i;
	TagID *id = ids;
	p = reply;
	for (i = 0; i < 4*MAX_ID; i++) {
		if (! (p = strchr(p, '[')))
			break;
		p++;
		if (*p && (*p == ']' || *p == 'z' || *p == 'Z'))
			continue;
		if (getXWord(&p, data))	continue;
		strcpy(id->id, data);
		id++;
		cnt++;
		if (cnt >= num)	break;
		}
	id->id[0] = 0;
	return 0;
}
