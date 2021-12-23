// TabFeliCa.cpp : implementation file
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
#include "TabFeliCa.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	DATA_LEN	256
#define MAX_MID		32
#define MID_LEN		20

// FeliCa Command codes
#define	CC_POLLING	0x00
#define	CC_CHECK	0x06
#define	CC_UPDATE	0x08

#define	POLLING		0
#define	CHECK		1
#define	UPDATE		2
#define	PROTOCOL	3
#define	SET		4


static char *commands[] = {	"polling",
				"check",
				"update",
				"set_protocol",
				"set",
				NULL};

static char *speedOptions[] = {	"212",	"424",
				NULL};
static char *dataPowerMode[] = {	"full",	"half",
				NULL};

#define	D_MID		0
#define	D_SLOT_NO	1
#define	D_SPEED		2
#define	D_POWER		3

static char *dataKeys[] = {	"MID",
				"number_of_slots",
				"speed",
				"power",
				NULL};


static char MIDs[MAX_MID][MID_LEN],
		manufParmas[MAX_MID][20], reqDatas[MAX_MID][8];
static int selectedProtocol = -1,
		tFLCiFullPower;
static CString tFLCMID, tFLCSlotNo, tFLCSpeed;

/////////////////////////////////////////////////////////////////////////////
// CTabFeliCa dialog


// make it avalable to external functions:
static CTabFeliCa *RRdialog;

CTabFeliCa::CTabFeliCa(CWnd* pParent /*=NULL*/)
	: CDialog(CTabFeliCa::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabFeliCa)
	m_iFLCCommandRB = -1;
	m_sFLCMIDNo = _T("");
	m_sFLCMID = _T("");
	m_sFLCManufParam = _T("");
	m_sFLCReqData = _T("");
	m_sFLCSlotNo = _T("00");
	m_sFLCSpeed = _T("212 kb/s");
	m_iFLCFullPower = 0;
	//}}AFX_DATA_INIT
	selectedProtocol = -1;
	RRdialog = this;
}


void CTabFeliCa::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabFeliCa)
	DDX_Control(pDX, IDC_FLC_MID, m_FLCMID);
	DDX_Radio(pDX, IDC_FLC_POLLING_RB, m_iFLCCommandRB);
	DDX_Text(pDX, IDC_FLC_MID_NO, m_sFLCMIDNo);
	DDX_CBString(pDX, IDC_FLC_MID, m_sFLCMID);
	DDX_Text(pDX, IDC_FLC_MANUF_PARAM, m_sFLCManufParam);
	DDX_Text(pDX, IDC_FLC_REQ_DATA, m_sFLCReqData);
	DDX_Text(pDX, IDC_FLC_SLOT_NO, m_sFLCSlotNo);
	DDX_Text(pDX, IDC_FLC_SPEED, m_sFLCSpeed);
	DDX_Radio(pDX, IDC_FLC_FULL_POWER, m_iFLCFullPower);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabFeliCa, CDialog)
	//{{AFX_MSG_MAP(CTabFeliCa)
	ON_BN_CLICKED(IDC_FLC_PROTOCOL, OnFLCSend)
	ON_BN_CLICKED(IDC_FLC_POLLING_RB, OnFLCCommandRB)
	ON_BN_CLICKED(IDC_FLC_CHECK_RB, OnFLCCommandRB)
	ON_BN_CLICKED(IDC_FLC_UPDATE_RB, OnFLCCommandRB)
	ON_CBN_SELCHANGE(IDC_FLC_MID, OnFLCMID)
	ON_BN_CLICKED(IDC_FLC_EXECUTE, OnFLCExecute)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabFeliCa message handlers

void CTabFeliCa::FLCResetTab() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	GetDlgItem(IDC_FLC_POLLING_RB)->EnableWindow(TRUE);
	GetDlgItem(IDC_FLC_CHECK_RB)->EnableWindow(FALSE);
	GetDlgItem(IDC_FLC_UPDATE_RB)->EnableWindow(FALSE);
	m_iFLCCommandRB = 0;
	UpdateData(FALSE);	// Set screen values
	OnFLCCommandRB();
}

void FLCResetTab() 
{
	RRdialog->FLCResetTab();
}

void CTabFeliCa::OnFLCCommandRB() 
{
	UpdateData(TRUE);	// Get curret values from the screen
#if 0
	// Disable all command dependant controls
	GetDlgItem(IDC_FLC_SOD)->EnableWindow(FALSE);
	GetDlgItem(IDC_FLC_BL_SIZE)->EnableWindow(FALSE);
#endif
	GetDlgItem(IDC_FLC_SLOT_NO)->EnableWindow(FALSE);
	// Selectively enable controls
	switch (m_iFLCCommandRB) {
	  case POLLING:
		GetDlgItem(IDC_FLC_SLOT_NO)->EnableWindow(TRUE);
		break;
	  case CHECK:
		break;
	  case UPDATE:
		break;
	  }
}

static int send() 
{
	char cmd[CMD_LEN];
#if ! SETUP_7961 || SETUP_7960
	strcpy(cmd, "0B");		// configure temporary ASIC
	if (tFLCiFullPower)	strcat(cmd, "00");
	else				strcat(cmd, "FF");
	if (portWrite(cmd, 1, 0))
		return 1;
#endif

	strcpy(cmd, "10");		// write to register
#if SETUP_7961
	if (tFLCiFullPower)	strcat(cmd, "0031");
	else				strcat(cmd, "0021");
#endif
	strcat(cmd, "01");		// register #
	if (! strncmp(tFLCSpeed, "212", 3))	strcat(cmd, "1A");
	else					strcat(cmd, "1B");
	portWrite(cmd, 0, 0);
	portRead(cmd, CMD_LEN);
	selectedProtocol = 1;
	newProtocol = 0;
	if (topResetFlags())
		return 1;
	return 0;
}

void CTabFeliCa::OnFLCSend() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	tFLCiFullPower = m_iFLCFullPower;
	tFLCSpeed = m_sFLCSpeed;
	if (send())
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
	  case CHECK:	h = CC_CHECK;	break;
	  case UPDATE:	h = CC_UPDATE;	break;
	  }
	sprintf(hex, "%4.4x", h);
	int err = 0, size = 1;
	if (! cmdno)	strcpy(cmd, "44");
	else		strcpy(cmd, "18");
	if (cmdno)	strcat(cmd, hex);
	switch (cmdno) {
	  case POLLING:
		err += addHex(cmd, CMD_LEN, 2, tFLCSlotNo, "Number of Slots");
		break;
	  case CHECK:
		err += addHex(cmd, CMD_LEN, 2, tFLCMID, "Manufacturer ID");
		break;
	  case UPDATE:
		err += addHex(cmd, CMD_LEN, 2, tFLCMID, "Manufacturer ID");
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

int FlcGetTagInfo (char *data, char *uid, char *param, char *req)
{
	if (strlen(data) != 36)	return 1;
	char *p, *d;
	p = data;


	p += 4;//harsha

	strcpy(uid, p);
	d = uid;
	p += 16; d += 16;
	*d = 0;
	strcpy(param, p);
	d = param;
	p += 16; d += 16;
	*d = 0;
	strcpy(req, p);
	return 0;
}

void CTabFeliCa::OnFLCExecute() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	if ((selectedProtocol == -1 || newProtocol) && ! expert) {
		MessageBox("Please, set Protocol");
		return;
		}
	if (m_iFLCCommandRB == -1) {
		MessageBox("Pleasse, select Command");
		return;
		}
	// Save data
	tFLCMID = m_sFLCMID;


	//tFLCMID = m_sFLCMID.GetBuffer(0);
	tFLCSlotNo = m_sFLCSlotNo;
	tFLCSpeed = m_sFLCSpeed;

	char reply[CMD_LEN];
	if (execute(m_iFLCCommandRB, reply))
		return;

	// check error code
	char *emsg, *p;
	int err;
	if ((p = strstr(reply, "[x"))) {	// pp. 38
		p += 2;
		sscanf(p, "%x", &err);
		switch (err) {
		  case 0xA1:
			emsg = "Invalid number of services";
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
		return;
		}
	else
		p = reply;

	char data[DATA_LEN+4], uid[40], param[24], req[8];
	// Show data
	if (m_iFLCCommandRB == POLLING) {	// MID poll: load available MIDs
		m_FLCMID.ResetContent();
		int index, cnt = 0, i;
		p = reply;
		for (i = 0; i < 4*MAX_MID; i++) {
			if (! (p = strchr(p, '[')))
				break;
			p++;
			if (*p && (*p == ']' || *p == 'z' || *p == 'Z'))
				continue;
			if (getXWord(&p, data))	continue;
			if (FlcGetTagInfo(data, uid, param, req))
								continue;
			index = m_FLCMID.AddString(uid);
			m_FLCMID.SetItemData(index, cnt);
			strcpy(MIDs[cnt], uid);
			strcpy(manufParmas[cnt], param);
			strcpy(reqDatas[cnt], req);
			cnt++;
			if (cnt >= MAX_MID)
				break;
			}
		m_FLCMID.SetCurSel(0);
		m_sFLCMID = MIDs[0];
		char num[8];
		sprintf(num, "%d", cnt);
		m_sFLCMIDNo = num;
		m_sFLCManufParam = manufParmas[0];
		m_sFLCReqData = reqDatas[0];
		UpdateData(FALSE);	// Set screen values
 		}
	if (m_iFLCCommandRB == CHECK && (p = strchr(p, '['))) {
 		}
	if (m_iFLCCommandRB == UPDATE && (p = strchr(p, '['))) {
		UpdateData(FALSE);	// Set screen values
 		}
}

int parseFeliCa (int doit, char *line, char *path, int lineno, char * reply)
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
	  case POLLING: case CHECK: case UPDATE:
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
		if (cmd == D_SPEED) {
			cmdx = lookup(word, speedOptions);
			if (cmdx == -1) {
				msg.Format("Invalid speed option '%s' in line %d", word, lineno);
				hopa(msg);
				return 1;
				}
			}
		if (doit) {
			switch (cmd) {
			  case D_MID:
				tFLCMID = data;
				break;
			  case D_SLOT_NO:
				tFLCSlotNo = data;
				break;
			  case D_SPEED:
				tFLCSpeed = data;
				break;
			  case D_POWER:
				tFLCiFullPower = cmdx;
				break;
			  }
			}
		break;
	  }
	return 0;
}

int findFeliCaTags (TagID *ids, int num)
{
	if (MAX_ID_LEN < MID_LEN)
		return 1;
	tFLCSlotNo = "15";
	if (send())
		return 1;
	char reply[CMD_LEN];
#if NO_COM
	strcpy(reply, "_FeliCa_");
#endif
	if (execute(POLLING, reply))
		return 1;
	char data[DATA_LEN+4], uid[40], param[24], req[8], *p;
	int cnt = 0, i;
	TagID *id = ids;
	p = reply;
	for (i = 0; i < 4*MAX_MID; i++) {
		if (! (p = strchr(p, '[')))
			break;
		p++;
		if (*p && (*p == ']' || *p == 'z' || *p == 'Z'))
			continue;
		if (getXWord(&p, data))	continue;
		if (FlcGetTagInfo(data, uid, param, req))	continue;
		strcpy(id->id, uid);
		id++;
		cnt++;
		if (cnt >= num)	break;
		}
	id->id[0] = 0;
	return 0;
}

void CTabFeliCa::OnFLCMID() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	int index = m_FLCMID.GetCurSel(), i;
	char sel[MID_LEN];
	m_FLCMID.GetLBText(index, sel);
	CString cstr;
	for (i = 0; i < MAX_MID; i++) {
		if (! strcmp(sel, MIDs[i]))
			break;
		}
	if (i < MAX_MID) {
		m_sFLCMID = MIDs[i];
		m_sFLCManufParam = manufParmas[i];
		m_sFLCReqData = reqDatas[i];
		}
	UpdateData(FALSE);	// Set screen values
}
