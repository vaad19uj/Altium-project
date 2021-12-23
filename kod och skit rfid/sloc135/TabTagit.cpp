// TabTagit.cpp : implementation file
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
#include "TabTagit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	DATA_LEN	256
#define MAX_SID		32
#define SID_LEN		68

// TAGIT Command codes
#define	CC_SID_POLL	0x0A
#define	CC_GET_VERSION	0x03
#define	CC_GET_BLOCK	0x01
#define	CC_PUT_BLOCK	0x05
#define	CC_PUT_B_LOCK	0x07
#define	CC_LOCK_BLOCK	0x08
#define	CC_QUIET	0x0B

#define	SID_POLL	0
#define	GET_VERSION	1
#define	GET_BLOCK	2
#define	PUT_BLOCK	3
#define	PUT_B_LOCK	4
#define	LOCK_BLOCK	5
#define	QUIET		6
#define	PROTOCOL	7
#define	FLAG		8
#define	SET		9


static char *commands[] = {	"SID_poll",
				"get_version",
				"get_block",
				"put_block",
				"put_block_lock",
				"lock_block",
				"quiet",
				"set_protocol",
				"flag",
				"set",
				NULL};

static char *dataPowerMode[] = {	"full",	"half",
				NULL};

#define	F_ADDRESS	0
#define	F_INFO	1

static char *flags[] = {	"addressed",
				"info",
				NULL};

#define	D_SID		0
#define	D_BLOCK_SIZE	1
#define	D_BLOCK_NUM	2
#define	D_DATA		3
#define	D_POWER		4

static char *dataKeys[] = {	"SID",
				"block_size",
				"block_number",
				"data",
				"power",
				NULL};


static char SIDs[MAX_SID][SID_LEN];
static unsigned char manufs[MAX_SID], blSizes[MAX_SID], numBls[MAX_SID];
static unsigned short versions[MAX_SID];
static int selectedProtocol = -1,
		tTIbAddressed, tTIbInfo, tTIiFullPower;
static CString tTISID, tTIBlockNum, tTIBlSize, tTIData;

/////////////////////////////////////////////////////////////////////////////
// CTabTagit dialog


CTabTagit::CTabTagit(CWnd* pParent /*=NULL*/)
	: CDialog(CTabTagit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabTagit)
	m_iTICommandRB = -1;
	m_sTIVersion = _T("");
	m_sTISIDNo = _T("");
	m_sTISID = _T("");
	m_sTINumBl = _T("");
	m_sTIManuf = _T("");
	m_sTILockStatus = _T("");
	m_bTIAddressed = FALSE;
	m_sTIData = _T("");
	m_sTIBlSize = _T("");
	m_sTIBlockNum = _T("");
	m_bTIInfo = FALSE;
	m_iTIFullPower = 0;
	m_iTICommandRB = 0;
	//}}AFX_DATA_INIT
	selectedProtocol = -1;
}


void CTabTagit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabTagit)
	DDX_Control(pDX, IDC_TI_SOD, m_TISID);
	DDX_Radio(pDX, IDC_TI_SID_POLL_RB, m_iTICommandRB);
	DDX_Text(pDX, IDC_TI_VERSION, m_sTIVersion);
	DDX_Text(pDX, IDC_TI_SID_NO, m_sTISIDNo);
	DDX_CBString(pDX, IDC_TI_SOD, m_sTISID);
	DDX_Text(pDX, IDC_TI_NUM_BL, m_sTINumBl);
	DDX_Text(pDX, IDC_TI_MANUF, m_sTIManuf);
	DDX_Text(pDX, IDC_TI_LOCK_ST, m_sTILockStatus);
	DDX_Check(pDX, IDC_TI_F_ADDRESSED, m_bTIAddressed);
	DDX_Text(pDX, IDC_TI_DATA, m_sTIData);
	DDX_Text(pDX, IDC_TI_BL_SIZE, m_sTIBlSize);
	DDX_Text(pDX, IDC_TI_BLOCK_NUM, m_sTIBlockNum);
	DDX_Check(pDX, IDC_TI_F_INFO, m_bTIInfo);
	DDX_Radio(pDX, IDC_TI_FULL_POWER, m_iTIFullPower);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabTagit, CDialog)
	//{{AFX_MSG_MAP(CTabTagit)
	ON_BN_CLICKED(IDC_TI_F_ADDRESSED, OnTIFAddressed)
	ON_BN_CLICKED(IDC_TI_EXECUTE, OnTIExecute)
	ON_BN_CLICKED(IDC_TI_SID_POLL_RB, OnTICommandRB)
	ON_CBN_SELCHANGE(IDC_TI_SOD, OnTISID)
	ON_BN_CLICKED(IDC_TI_VERSION_RB, OnTICommandRB)
	ON_BN_CLICKED(IDC_TI_PUT_BL_LOCK_RB, OnTICommandRB)
	ON_BN_CLICKED(IDC_TI_PUT_BL_RB, OnTICommandRB)
	ON_BN_CLICKED(IDC_TI_GET_BL_RB, OnTICommandRB)
	ON_BN_CLICKED(IDC_TI_LOCK_BL_RB, OnTICommandRB)
	ON_BN_CLICKED(IDC_TI_QUIET_RB, OnTICommandRB)
	ON_BN_CLICKED(IDC_TI_PROTOCOL, OnTISend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabTagit message handlers

void CTabTagit::OnTIFAddressed() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	if (m_iTICommandRB) {
		if (m_bTIAddressed)
			GetDlgItem(IDC_TI_SOD)->EnableWindow(TRUE);
		else
			GetDlgItem(IDC_TI_SOD)->EnableWindow(FALSE);
		}
}

void CTabTagit::OnTICommandRB() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	// Disable all command dependant controls
	GetDlgItem(IDC_TI_SOD)->EnableWindow(FALSE);
	GetDlgItem(IDC_TI_BL_SIZE)->EnableWindow(FALSE);
	GetDlgItem(IDC_TI_BLOCK_NUM)->EnableWindow(FALSE);
	GetDlgItem(IDC_TI_DATA)->EnableWindow(FALSE);
	if (m_iTICommandRB) {
		GetDlgItem(IDC_TI_F_ADDRESSED)->EnableWindow(TRUE);
		GetDlgItem(IDC_TI_F_INFO)->EnableWindow(FALSE);
		}
	else {
		GetDlgItem(IDC_TI_F_ADDRESSED)->EnableWindow(FALSE);
		GetDlgItem(IDC_TI_F_INFO)->EnableWindow(TRUE);
		}
	// Selectively enable controls
	switch (m_iTICommandRB) {
	  case SID_POLL:
		break;
	  case GET_VERSION:
		if (m_bTIAddressed)
			GetDlgItem(IDC_TI_SOD)->EnableWindow(TRUE);
		break;
	  case GET_BLOCK:
		if (m_bTIAddressed)
			GetDlgItem(IDC_TI_SOD)->EnableWindow(TRUE);
		GetDlgItem(IDC_TI_BL_SIZE)->EnableWindow(TRUE);
		GetDlgItem(IDC_TI_BLOCK_NUM)->EnableWindow(TRUE);
		break;
	  case PUT_BLOCK:
		if (m_bTIAddressed)
			GetDlgItem(IDC_TI_SOD)->EnableWindow(TRUE);
		GetDlgItem(IDC_TI_BL_SIZE)->EnableWindow(TRUE);
		GetDlgItem(IDC_TI_BLOCK_NUM)->EnableWindow(TRUE);
		GetDlgItem(IDC_TI_DATA)->EnableWindow(TRUE);
		break;
	  case PUT_B_LOCK:
		if (m_bTIAddressed)
			GetDlgItem(IDC_TI_SOD)->EnableWindow(TRUE);
		GetDlgItem(IDC_TI_BL_SIZE)->EnableWindow(TRUE);
		GetDlgItem(IDC_TI_BLOCK_NUM)->EnableWindow(TRUE);
		GetDlgItem(IDC_TI_DATA)->EnableWindow(TRUE);
		break;
	  case LOCK_BLOCK:
		if (m_bTIAddressed)
			GetDlgItem(IDC_TI_SOD)->EnableWindow(TRUE);
		GetDlgItem(IDC_TI_BLOCK_NUM)->EnableWindow(TRUE);
		break;
	  case QUIET:
		if (m_bTIAddressed)
			GetDlgItem(IDC_TI_SOD)->EnableWindow(TRUE);
		break;
	  }
}

static int send() 
{
	char cmd[CMD_LEN], hex[4];
#if ! SETUP_7961 || SETUP_7960
	strcpy(cmd, "0C");		// configure temporary ASIC
	if (tTIiFullPower)	strcat(cmd, "00");
	else				strcat(cmd, "FF");
	if (portWrite(cmd, 1, 0))
		return 1;
#endif

	int reg01 = 0;	// Kje ga dobim?
	reg01 |= 0x13;
	sprintf(hex, "%2.2X", reg01);
	strcpy(cmd, "10");		// write to register
#if SETUP_7961
	if (tTIiFullPower)	strcat(cmd, "0031");
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

void CTabTagit::OnTISend() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	tTIiFullPower = m_iTIFullPower;
	if (! send())
		topResetFlagsGUI();
}

static int execute(int cmdno, char *reply)
{
	if ((selectedProtocol == -1 || newProtocol) && ! expert) {
		logAdd("Please, set Protocol", 0, 0);
		return 1;
		}
	// Assemble request # of bits  Field Name
	//			   3	Padding (000)
	//			   2	Request Code
	//			   8	Command Code
	//			   1	Format Type
	//			   1	Address Flag
	//			   1  	Reserved
	// Move 5 bits of the first byte left by 3
	//			 (32)	(SID)
	//			   x	Parameters
	char cmd[CMD_LEN], hex[8];
	short unsigned int header, h;
	CString info;
	switch (cmdno) {
	  case SID_POLL:	h = CC_SID_POLL;	break;
	  case GET_VERSION:	h = CC_GET_VERSION;	break;
	  case GET_BLOCK:	h = CC_GET_BLOCK;	break;
	  case PUT_BLOCK:	h = CC_PUT_BLOCK;	break;
	  case PUT_B_LOCK:	h = CC_PUT_B_LOCK;	break;
	  case LOCK_BLOCK:	h = CC_LOCK_BLOCK;	break;
	  case QUIET:		h = CC_QUIET;	break;
	  }
	header = h << 3;
	if (tTIbAddressed)	header |= 0x0002;
	header = header & 0x00ff | (header & 0xff00) << 3;
	sprintf(hex, "%4.4x", header);
	int err = 0, size = 1;
	if (strlen(tTIBlSize.GetBuffer(0))) {
		sscanf(tTIBlSize.GetBuffer(0), "%x", &size);
		// size++;
		}
	if (! cmdno)	strcpy(cmd, "34");
	else		strcpy(cmd, "18");
	strcat(cmd, hex);
	if (cmdno && tTIbAddressed)
		err += addHex(cmd, CMD_LEN, 8, tTISID, "SID");
	switch (cmdno) {
	  case SID_POLL:
		if (tTIbInfo)	info.Format("80");
		else			info.Format("00");
		err += addHex(cmd, CMD_LEN, 2, info, "Info");
		break;
	  case GET_BLOCK:
		err += addHex(cmd, CMD_LEN, 2, tTIBlockNum, "Block Number");
		break;
	  case PUT_BLOCK:
		err += addHex(cmd, CMD_LEN, 2, tTIBlockNum, "Block Number");
		err += addHex(cmd, CMD_LEN, 2*size, tTIData, "Data");
		break;
	  case PUT_B_LOCK:
		err += addHex(cmd, CMD_LEN, 2, tTIBlockNum, "Block Number");
		err += addHex(cmd, CMD_LEN, 2*size, tTIData, "Data");
		break;
	  case LOCK_BLOCK:
		err += addHex(cmd, CMD_LEN, 2, tTIBlockNum, "Block Number");
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

static int getTagInfo (char *data, char *uid, unsigned char *manuf,
			unsigned short *version, unsigned char *blSize,
			unsigned char *numBl, int addressed)
{
	char tmp[8], *p = data, *d;
	unsigned short int chip;
	int i;
	if (addressed) {
		for (d = uid, i = 0; i < 8; i++)	*d++ = *p++;
		*d = 0;
		}
	for (d = tmp, i = 0; i < 4; i++)	*d++ = *p++;
	*d = 0;
	sscanf(tmp, "%x", &chip);
	*manuf = chip >> 9;
	*version = chip & 0x01ff;
	for (d = tmp, i = 0; i < 2; i++)	*d++ = *p++;
	*d = 0;
	sscanf(tmp, "%x", blSize);
	*blSize &= 0x01f;
	(*blSize)++;
	sscanf(p, "%x", numBl);
	(*numBl)++;
	return 0;
}

char *hex2bin(char *hex)
{
	static char bin[84];
	bin[0] = 0;
	int srclen = strlen(hex);
	if (srclen > 10) {
		logAdd("*** Too long hex number for binary conversion ***",0,0);
		return bin;
		}
	int i;
	char *h;
	for (h = hex, i = 0; i < srclen; i++, h++) {
		switch (*h) {
		  case '0': strcat(bin, "0000");	break;
		  case '1': strcat(bin, "0001");	break;
		  case '2': strcat(bin, "0010");	break;
		  case '3': strcat(bin, "0011");	break;
		  case '4': strcat(bin, "0100");	break;
		  case '5': strcat(bin, "0101");	break;
		  case '6': strcat(bin, "0110");	break;
		  case '7': strcat(bin, "0111");	break;
		  case '8': strcat(bin, "1000");	break;
		  case '9': strcat(bin, "1001");	break;
		  case 'A':
		  case 'a': strcat(bin, "1010");	break;
		  case 'B':
		  case 'b': strcat(bin, "1011");	break;
		  case 'C':
		  case 'c': strcat(bin, "1100");	break;
		  case 'D':
		  case 'd': strcat(bin, "1101");	break;
		  case 'E':
		  case 'e': strcat(bin, "1110");	break;
		  case 'F':
		  case 'f': strcat(bin, "1111");	break;
		  }
		}
	return bin;
}

char *bin2hex(char *bin, int len)
{
	static char hex[12];
	hex[0] = 0;
	int srclen = strlen(hex);
	if ((srclen / 8) > 10) {
		logAdd("*** Too long binary number for hex conversion ***",0,0);
		return bin;
		}
	char *b;
	int x, i, j;
	for (b = bin, i = 0; i < len; i++) {
		x = 0;
		for (j = 0; j < 4; j++) {
			if (*b) {
				if (*b == '1') {
					switch (j) {
					  case 0: x += 8;	break;
					  case 1: x += 4;	break;
					  case 2: x += 2;	break;
					  case 3: x += 1;	break;
					  }
					}
				b++;
				}
			}
		switch (x) {
		  case  0: strcat(hex, "0");	break;
		  case  1: strcat(hex, "1");	break;
		  case  2: strcat(hex, "2");	break;
		  case  3: strcat(hex, "3");	break;
		  case  4: strcat(hex, "4");	break;
		  case  5: strcat(hex, "5");	break;
		  case  6: strcat(hex, "6");	break;
		  case  7: strcat(hex, "7");	break;
		  case  8: strcat(hex, "8");	break;
		  case  9: strcat(hex, "9");	break;
		  case 10: strcat(hex, "A");	break;
		  case 11: strcat(hex, "B");	break;
		  case 12: strcat(hex, "C");	break;
		  case 13: strcat(hex, "D");	break;
		  case 14: strcat(hex, "E");	break;
		  case 15: strcat(hex, "F");	break;
		  }
		}
	return hex;
}

void CTabTagit::OnTIExecute() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	if ((selectedProtocol == -1 || newProtocol) && ! expert) {
		MessageBox("Please, set Protocol");
		return;
		}
	if (m_iTICommandRB == -1) {
		MessageBox("Pleasse, select Command");
		return;
		}
	// Save flags
	tTIbAddressed = m_bTIAddressed;
	tTIbInfo = m_bTIInfo;
	// Save data
	tTISID = m_sTISID;
	tTIBlockNum = m_sTIBlockNum;
	tTIData = m_sTIData;
	tTIBlSize = m_sTIBlSize;

	char reply[CMD_LEN];
	if (execute(m_iTICommandRB, reply))
		return;

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
	else
		p = reply;

	char data[DATA_LEN+4], uid[34], *d;
	unsigned char manuf, blSize, numBl;
	unsigned short version;
	int i;
	// Show data
	if (! m_iTICommandRB) {	// SID poll: load available SIDs
		m_TISID.ResetContent();
		
		//Clear all screen values
		strcpy(SIDs[0],"");
		m_sTIManuf = "";
		m_sTIVersion = "";
		m_sTIBlSize = "";
		m_sTINumBl = "";
		m_sTIBlockNum = " ";
		m_sTIData = "";
		m_sTILockStatus = "";

//		CString cuid;
		int index, cnt = 0, i;
		p = reply;
		for (i = 0; i < 4*MAX_SID; i++) {
			if (! (p = strchr(p, '[')))
				break;
			p++;
			if (*p && (*p == ']' || *p == 'z' || *p == 'Z'))
				continue;
			p += 4;
			if (getXWord(&p, data))	continue;
			getTagInfo(data, uid, &manuf, &version, &blSize,
								&numBl, 1);
//			cuid = uid;
			index = m_TISID.AddString(uid);
			m_TISID.SetItemData(index, cnt);
			strcpy(SIDs[cnt], uid);
			manufs[cnt] = manuf;
			versions[cnt] = version;
			blSizes[cnt] = blSize;
			numBls[cnt] = numBl;
			cnt++;
			if (cnt >= MAX_SID)
				break;
			}
		m_TISID.SetCurSel(0);
		m_sTISID = SIDs[0];
		char num[8];
		sprintf(num, "%d", cnt);
//		cuid = num;
//		m_sTISIDNo = cuid;
		m_sTISIDNo = num;
		UpdateData(FALSE);	// Set screen values
 		}
	if (m_iTICommandRB == GET_VERSION && (p = strchr(p, '['))) {
		p += 5;
		if (getXWord(&p, data))	return;
		if (strlen(data) < 8) {
			hopa("Short version reply");
			return;
			}
		getTagInfo(data, uid, &manuf, &version, &blSize,
							&numBl, 1);
		if (m_bTIAddressed && strcmp(m_sTISID.GetBuffer(0), uid)) {
			hopa("Invalid SID number in reply");
			return;
			}
		m_sTIManuf.Format("%2.2x", manuf);
		m_sTIVersion.Format("%3.3x", version);
		m_sTIBlSize.Format("%2.2x", blSize);
		m_sTINumBl.Format("%2.2x", numBl);
		UpdateData(FALSE);	// Set screen values
 		}
	if (m_iTICommandRB == GET_BLOCK && (p = strchr(p, '['))) {
		char uid[10], blockNum[4], lock[4], *b, *h;
		int size, len;
		p += 5;
		if (m_bTIAddressed)	{
			for (d = uid, i = 0; i < 8; i++)	*d++ = *p++;
			*d = 0;
			if (strcmp(m_sTISID.GetBuffer(0), uid)) {
				hopa("Invalid SID number in reply");
				return;
				}
			}
		sscanf(m_sTIBlSize, "%x", &size);
		if (size > 128) {
			hopa("Block too long");
			return;
			}
		size *= 2;
		len = strlen(p);
		if (len < (4 + size)) {
			hopa("Short get_block reply");
			return;
			}
		for (d = blockNum, i = 0; i < 2; i++)	*d++ = *p++;
		*d = 0;
		if (strcmp(m_sTIBlockNum.GetBuffer(0), blockNum)) {
			hopa("Invalid block number in reply");
			return;
			}
		for (d = data, i = 0; i < 10 && *d; i++)
			*d++ = *p++;
		*d = 0;
		b = hex2bin(data);
		lock[0] = *b++, lock[1] = *b++; lock[2] = 0;
		h = bin2hex(lock, 1);
		m_sTILockStatus = h;
		h = bin2hex(b, size);
		m_sTIData = h;
		UpdateData(FALSE);	// Set screen values
 		}
}

int parseTagit (int doit, char *line, char *path, int lineno, char * reply)
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
	  case SID_POLL: case GET_VERSION: case GET_BLOCK: case PUT_BLOCK:
	  case PUT_B_LOCK: case LOCK_BLOCK: case QUIET:
		if (doit)
			execute(cmd, reply);
		break;
	  case PROTOCOL:
		if (doit)
			send();
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
			  case F_ADDRESS:
				tTIbAddressed = flag;
				break;
			  case F_INFO:
				tTIbInfo = flag;
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
			  case D_SID:
				tTISID = data;
				break;
			  case D_BLOCK_SIZE:
				tTIBlSize = data;
				break;
			  case D_BLOCK_NUM:
				tTIBlockNum = data;
				break;
			  case D_DATA:
				tTIData = data;
				break;
			  case D_POWER:
				tTIiFullPower = cmdx;
				break;
			  }
			}
		break;
	  }
	return 0;
}

int findTagitTags (TagID *ids, int num)
{
	if (MAX_ID_LEN < SID_LEN)
		return 1;
	tTIbAddressed = 0;
	tTIbInfo = 0;
	if (send())
		return 1;
	char reply[CMD_LEN];
#if NO_COM
	strcpy(reply, "_Tagit_");
#endif
	if (execute(SID_POLL, reply))
		return 1;
	char data[DATA_LEN+4], uid[34], *p;
	int cnt = 0, i;
	unsigned char manuf, blSize, numBl;
	unsigned short version;
	TagID *id = ids;
	p = reply;
	for (i = 0; i < 4*MAX_SID; i++) {
		if (! (p = strchr(p, '[')))
			break;
		p++;
		if (*p && (*p == ']' || *p == 'z' || *p == 'Z'))
			continue;
		p += 4;
		if (getXWord(&p, data))	continue;
		getTagInfo(data, uid, &manuf, &version, &blSize,
								&numBl, 1);
		strcpy(id->id, uid);
		id++;
		cnt++;
		if (cnt >= num)	break;
		}
	id->id[0] = 0;
	return 0;
}

void CTabTagit::OnTISID() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	int index = m_TISID.GetCurSel(), i;
	char sel[SID_LEN];
	m_TISID.GetLBText(index, sel);
	CString cstr;
	for (i = 0; i < MAX_SID; i++) {
		if (! strcmp(sel, SIDs[i]))
			break;
		}
	if (i < MAX_SID) {
		m_sTISID = SIDs[i];
		m_sTIManuf.Format("%2.2x", manufs[i]);
		m_sTIVersion.Format("%3.3x", versions[i]);
		m_sTIBlSize.Format("%2.2x", blSizes[i]);
		m_sTINumBl.Format("%2.2x", numBls[i]);
		}

	UpdateData(FALSE);	// Set screen values
}
