// Tab15693.cpp : implementation file
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
#include "Tab15693.h"
#include <string>
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	DATA_LEN	256
#define MAX_UID		32
#define UID_LEN		68
#define	DSFID_LEN	4

// 15693 Command codes
#define	CC_INVENTORY	"01"
#define	CC_QUIET	"02"
#define	CC_READ_SB	"20"
#define	CC_WRITE_SB	"21"
#define	CC_LOCK		"22"
#define	CC_READ_MP	"23"
#define	CC_WRITE_MP	"24"
#define	CC_SELECT	"25"
#define	CC_READY	"26"
#define	CC_WRITE_API	"27"
#define	CC_LOCK_API	"28"
#define	CC_WRITE_DSFID	"29"
#define	CC_LOCK_DSFID	"2A"
#define	CC_GET_INFO	"2B"
#define	CC_GET_SEC_STAT	"2C"
#define CC_WRITE_TWO_BLOCKS "A2"
#define CC_LOCK_TWO_BLOCKS "A3"

#define	INVENTORY	0
#define	READ_SB		1
#define	WRITE_SB	2
#define	LOCK_B		3
#define	READ_MB		4
#define	WRITE_MB	5
#define	QUIET		6
#define	SELECT		7
#define	READY		8
#define	WRITE_AFI	9
#define	LOCK_AFI	10
#define	WRITE_DSFID	11
#define	LOCK_DSFID	12
#define	SYSTEM_INFO	13
#define	MBS_STATUS	14
#define	PROTOCOL	15
#define	FLAG		16
#define	SET			17

#define WRITE_TWO_BLOCKS	15
#define LOCK_TWO_BLOCKS		16

#define MFG_CODE "07"

static char *commands[] = {	"inventory",
				"read_single_block",
				"write_single_block",
				"lock_block",
				"read_multiple_blocks",
				"write_multiple_blocks",
				"stay_quiet",
				"select",
				"reset_to_ready",
				"write_AFI",
				"lock_AFI",
				"write_DSFID",
				"lock_DSFID",
				"get_system_info",
				"get_multi_blk_sel_status",
				"set_protocol",
				"flag",
				"set",
				NULL};

static char *dataCodingMode[] = {	"4",	"256",
				NULL};

static char *dataPowerMode[] = {	"full",	"half",
				NULL};

#define	F_SUB_CARRIER	0
#define	F_DATA_RATE	1
#define	F_SELECT	2
#define	F_ADDRESS	3
#define	F_OPTION	4
#define	F_AFI		5
#define	F_ONE_SLOT	6

static char *flags[] = {	"double_sub_carrier",
				"high_data_rate",
				"select",
				"address",
				"option",
				"AFI_is_present",
				"one_slot",
				NULL};

#define	D_UID		0
#define	D_FIRST_B	1
#define	D_NUM_BL	2
#define	D_DATA		3
#define	D_DSFID		4
#define	D_AFI		5
#define	D_CODING	6
#define	D_POWER		7

static char *dataKeys[] = {	"UID",
				"first_block",
				"number_of_blocks",
				"data",
				"DSFID",
				"AFI",
				"data_coding_mode",
				"power",
				NULL};

static char DSFIDs[MAX_UID][DSFID_LEN], UIDs[MAX_UID][UID_LEN];
static int selectedProtocol = -1,
		t15bSubCarrier, t15bDataRate, t15bInventory, t15bSelect,
		t15bAddress, t15bOption, t15bNextSlotC, t15iFullPower;
static CString t15UID, t15FirstBN, t15NumBl, t15Data, t15DSFID, t15AFI,
		t15Protocol;

static bool set;

void changeByteOrder_MB(char *, char *, int);
/////////////////////////////////////////////////////////////////////////////
// CTab15693 dialog


CTab15693::CTab15693(CWnd* pParent /*=NULL*/)
	: CDialog(CTab15693::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTab15693)
	m_i15CommandRB = -1;
	m_s15NumBl = _T("");
	m_s15AFI = _T("");
	m_s15Data = _T("");
	m_s15DSFID = _T("");
	m_b15DataRate = FALSE;
	m_s15Protocol = _T("1 out of 4");
	m_s15FirstBN = _T("");
	m_b15Select = FALSE;
	m_s15UID = _T("");
	m_b15Option = FALSE;
	m_b15Address = FALSE;
	m_b15SubCarrier = FALSE;
	m_15UIDNo = _T("");
	m_s15TagBS = _T("");
	m_s15TagNB = _T("");
	m_i15FullPower = 0;
	//}}AFX_DATA_INIT
	selectedProtocol = -1;
}

void CTab15693::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTab15693)
	DDX_Control(pDX, IDC_15_RSSI, m_15RSSI);
	DDX_Control(pDX, IDC_15_F_ADDRESS, m_15Address);
	DDX_Control(pDX, IDC_15_F_SELECT, m_15Select);
	DDX_Control(pDX, IDC_15_PROTOCOL, m_15Protocol);
	DDX_Control(pDX, IDC_15_UID, m_15UID);
	DDX_Radio(pDX, IDC_15_INVENTORY_RB, m_i15CommandRB);
	DDX_Text(pDX, IDC_15_NUM_BL, m_s15NumBl);
	DDV_MaxChars(pDX, m_s15NumBl, 2);
	DDX_Text(pDX, IDC_15_AFI, m_s15AFI);
	DDV_MaxChars(pDX, m_s15AFI, 2);
	DDX_Text(pDX, IDC_15_DATA, m_s15Data);
	DDX_Text(pDX, IDC_15_DSFID, m_s15DSFID);
	DDV_MaxChars(pDX, m_s15DSFID, 2);
	DDX_Check(pDX, IDC_15_F_DATA_RATE, m_b15DataRate);
	DDX_CBString(pDX, IDC_15_PROTOCOL, m_s15Protocol);
	DDX_Text(pDX, IDC_15_FIRST_BN, m_s15FirstBN);
	DDV_MaxChars(pDX, m_s15FirstBN, 2);
	DDX_Check(pDX, IDC_15_F_SELECT, m_b15Select);
	DDX_CBString(pDX, IDC_15_UID, m_s15UID);
	DDV_MaxChars(pDX, m_s15UID, 16);
	DDX_Check(pDX, IDC_15_F_OPTIONS, m_b15Option);
	DDX_Check(pDX, IDC_15_F_ADDRESS, m_b15Address);
	DDX_Check(pDX, IDC_15_F_SUB_CARRIER, m_b15SubCarrier);
	DDX_Text(pDX, IDC_15_UID_NO, m_15UIDNo);
	DDX_Text(pDX, IDC_15_TAG_BS, m_s15TagBS);
	DDX_Text(pDX, IDC_15_TAG_NB, m_s15TagNB);
	DDX_Radio(pDX, IDC_15_FULL_POWER, m_i15FullPower);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTab15693, CDialog)
	//{{AFX_MSG_MAP(CTab15693)
	ON_BN_CLICKED(IDC_15_SEND, On15Send)
	ON_BN_CLICKED(IDC_15_INVENTORY_RB, On15CommandRB)
	ON_BN_CLICKED(IDC_15_EXECUTE, On15Execute)
	ON_BN_CLICKED(IDC_15_F_SELECT, On15FSelect)
	ON_CBN_SELCHANGE(IDC_15_UID, On15UID)
	ON_BN_CLICKED(IDC_15_READ_SB_RB, On15CommandRB)
	ON_BN_CLICKED(IDC_15_WRITE_SB_RB, On15CommandRB)
	ON_BN_CLICKED(IDC_15_LOCK_RB, On15CommandRB)
	ON_BN_CLICKED(IDC_15_READ_MB_RB, On15CommandRB)
	ON_BN_CLICKED(IDC_15_WRITE_MB_RB, On15CommandRB)
	ON_BN_CLICKED(IDC_15_QUIET_RB, On15CommandRB)
	ON_BN_CLICKED(IDC_15_SELECT_RB, On15CommandRB)
	ON_BN_CLICKED(IDC_15_READY_RB, On15CommandRB)
	ON_BN_CLICKED(IDC_15_WRITE_AFI_RB, On15CommandRB)
	ON_BN_CLICKED(IDC_15_LOCK_AFI_RB, On15CommandRB)
	ON_BN_CLICKED(IDC_15_WRITE_DSIF_RB, On15CommandRB)
	ON_BN_CLICKED(IDC_15_LOCK_DSIF_RB, On15CommandRB)
	ON_BN_CLICKED(IDC_15_SYS_INFO_RB, On15CommandRB)
	ON_BN_CLICKED(IDC_15_SECURITY_RB, On15CommandRB)
	ON_BN_CLICKED(IDC_TICC, OnTicc)
	ON_BN_CLICKED(IDC_WRITE_TWO, On15CommandRB)
	ON_BN_CLICKED(IDC_LOCK_TWO, On15CommandRB)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTab15693 message handlers


void CTab15693::On15FSelect() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	if (! m_i15CommandRB) {
		if (m_b15Select)
			GetDlgItem(IDC_15_AFI)->EnableWindow(TRUE);
		else
			GetDlgItem(IDC_15_AFI)->EnableWindow(FALSE);
		}
}

void CTab15693::On15CommandRB() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	// Change 2 flag captions
	if (! m_i15CommandRB) {
		m_15Select.SetWindowText("AFI is present");
		m_15Address.SetWindowText("One slot");
		}
	else {
		m_15Select.SetWindowText("Select");
		m_15Address.SetWindowText("Addressed");
		}

	// Disable all command dependant controls
	GetDlgItem(IDC_15_UID)->EnableWindow(FALSE);
	GetDlgItem(IDC_15_FIRST_BN)->EnableWindow(FALSE);
	GetDlgItem(IDC_15_NUM_BL)->EnableWindow(FALSE);
	GetDlgItem(IDC_15_DATA)->EnableWindow(FALSE);
	GetDlgItem(IDC_15_DSFID)->EnableWindow(FALSE);
	GetDlgItem(IDC_15_AFI)->EnableWindow(FALSE);
	// Selectively enable controls
	switch (m_i15CommandRB) {
	  case 0:
		if (m_b15Select)
			GetDlgItem(IDC_15_AFI)->EnableWindow(TRUE);
		break;
	  case 1:
		GetDlgItem(IDC_15_UID)->EnableWindow(TRUE);
		GetDlgItem(IDC_15_FIRST_BN)->EnableWindow(TRUE);
		break;
	  case 2:
		GetDlgItem(IDC_15_UID)->EnableWindow(TRUE);
		GetDlgItem(IDC_15_FIRST_BN)->EnableWindow(TRUE);
		GetDlgItem(IDC_15_DATA)->EnableWindow(TRUE);
		break;
	  case 3:
		GetDlgItem(IDC_15_UID)->EnableWindow(TRUE);
		GetDlgItem(IDC_15_FIRST_BN)->EnableWindow(TRUE);
		break;
	  case 4:
		GetDlgItem(IDC_15_UID)->EnableWindow(TRUE);
		GetDlgItem(IDC_15_FIRST_BN)->EnableWindow(TRUE);
		GetDlgItem(IDC_15_NUM_BL)->EnableWindow(TRUE);
		break;
	  case 5:
		GetDlgItem(IDC_15_UID)->EnableWindow(TRUE);
		GetDlgItem(IDC_15_FIRST_BN)->EnableWindow(TRUE);
		GetDlgItem(IDC_15_NUM_BL)->EnableWindow(TRUE);
		GetDlgItem(IDC_15_DATA)->EnableWindow(TRUE);
		break;
	  case 6:
		GetDlgItem(IDC_15_UID)->EnableWindow(TRUE);
		break;
	  case 7:
		GetDlgItem(IDC_15_UID)->EnableWindow(TRUE);
		break;
	  case 8:
		GetDlgItem(IDC_15_UID)->EnableWindow(TRUE);
		break;
	  case 9:
		GetDlgItem(IDC_15_UID)->EnableWindow(TRUE);
		GetDlgItem(IDC_15_AFI)->EnableWindow(TRUE);
		break;
	  case 10:
		GetDlgItem(IDC_15_UID)->EnableWindow(TRUE);
		break;
	  case 11:
		GetDlgItem(IDC_15_UID)->EnableWindow(TRUE);
		GetDlgItem(IDC_15_DSFID)->EnableWindow(TRUE);
		break;
	  case 12:
		GetDlgItem(IDC_15_UID)->EnableWindow(TRUE);
		break;
	  case 13:
		GetDlgItem(IDC_15_UID)->EnableWindow(TRUE);
		break;
	  case 14:
		GetDlgItem(IDC_15_UID)->EnableWindow(TRUE);
		GetDlgItem(IDC_15_FIRST_BN)->EnableWindow(TRUE);
		GetDlgItem(IDC_15_NUM_BL)->EnableWindow(TRUE);
		break;
	case 15:
		GetDlgItem(IDC_15_UID)->EnableWindow(TRUE);
		GetDlgItem(IDC_15_FIRST_BN)->EnableWindow(TRUE);
		GetDlgItem(IDC_15_DATA)->EnableWindow(TRUE);
		break;

	case 16:
		GetDlgItem(IDC_15_UID)->EnableWindow(TRUE);
		GetDlgItem(IDC_15_FIRST_BN)->EnableWindow(TRUE);		
		break;
	  }
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
static int send(int selection) 
{
	char cmd[CMD_LEN], hex[4];

//Commented (harsha) - since thecommand type 0C is not supported

/*#if ! SETUP_7961 || SETUP_7960
	strcpy(cmd, "0C");		// configure temporary ASIC
	if (t15iFullPower)	strcat(cmd, "00");
	else				strcat(cmd, "FF");
	if (portWrite(cmd, 1, 0))
		return 1;
#endif
		*/

	selectedProtocol = selection;
	newProtocol = 0;
	int proto;
	if (! t15bDataRate) {
		if (! t15bSubCarrier) {
			if (! selection)	proto = 0x00;
			else				proto = 0x01;
			}
		else {
			if (! selection)	proto = 0x04;
			else				proto = 0x05;
			}
		}
	else {
		if (! t15bSubCarrier) {
			if (! selection)	proto = 0x02;
			else				proto = 0x03;
			}
		else {
			if (! selection)	proto = 0x06;
			else				proto = 0x07;
			}
		}
	int reg01 = 0;	// Kje ga dobim?
	reg01 |= proto;
	if (t15bNextSlotC)
		reg01 |= 0x20;
	sprintf(hex, "%2.2X", reg01);
	cmd[0] = 0;
#if SETUP_7961
	if (t15iFullPower)	strcpy(cmd, "100031");		// write to register
	else				strcpy(cmd, "100021");
#endif
	strcat(cmd, "01");		// register #
	strcat(cmd, hex);
	portWrite(cmd, 0, 0);
	portRead(cmd, CMD_LEN);
	if (topResetFlags())
		return 1;
	return 0;
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

void CTab15693::On15Send() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	int selection = m_15Protocol.GetCurSel();
	if (selection == -1) {
		MessageBox("Please, select Protocol");
		return;
		}
	t15bSubCarrier = m_b15SubCarrier;
	t15bDataRate = m_b15DataRate;
	t15iFullPower = m_i15FullPower;
	if (! send(selection))
		topResetFlagsGUI();
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

static int execute(int cmdno, char *reply)
{
	if ((selectedProtocol == -1 || newProtocol) && ! expert) {
		logAdd("Please, set Protocol", 0, 0);
		return 1;
		}
	// Assemble request
	char cmd[CMD_LEN], mwData[CMD_LEN], hex[4], *p;
	int err = 0, flags = 0, len = 0, bn, i;
	if (! cmdno)	strcpy(cmd, "14");
	else		strcpy(cmd, "18");
	if (t15bSubCarrier)	flags |= 0x01;
	if (t15bDataRate)	flags |= 0x02;
	if (t15bInventory)	flags |= 0x04;
	if (t15bSelect)		flags |= 0x10;
	if (t15bAddress)	flags |= 0x20;
	if (t15bOption)		flags |= 0x40;
	flags &= 0xff;
	sprintf(hex, "%2.2X", flags);
	strcat(cmd, hex);
	switch (cmdno) {
	  case INVENTORY:
		strcat(cmd, CC_INVENTORY);
		if (t15bSelect)
			err += addHex(cmd, CMD_LEN, 2, t15AFI, "AFI");
		strcat(cmd, "00");	// mask
		break;
	  case READ_SB:
		strcat(cmd, CC_READ_SB);
		if (t15bAddress)
			err += addHex(cmd, CMD_LEN, 0, t15UID, "UID");
		err += addHex(cmd, CMD_LEN, 2, t15FirstBN, "First Block Number");
		break;
	  case WRITE_SB:
		strcat(cmd, CC_WRITE_SB);
		if (t15bAddress)
			err += addHex(cmd, CMD_LEN, 0, t15UID, "UID");
		err += addHex(cmd, CMD_LEN, 2, t15FirstBN, "First Block Number");
		err += addHex(cmd, CMD_LEN, 2 * 4, t15Data, "Data");
		break;
	  case LOCK_B:
		strcat(cmd, CC_LOCK);
		if (t15bAddress)
			err += addHex(cmd, CMD_LEN, 0, t15UID, "UID");
		err += addHex(cmd, CMD_LEN, 2, t15FirstBN, "First Block Number");
		break;
	  case READ_MB:
		strcat(cmd, CC_READ_MP);
		if (t15bAddress)
			err += addHex(cmd, CMD_LEN, 0, t15UID, "UID");
		err += addHex(cmd, CMD_LEN, 2, t15FirstBN, "First Block Number");
		err += addHex(cmd, CMD_LEN, 2, t15NumBl, "Number of Blocks");
		break;
	  case WRITE_MB:
		sscanf(t15NumBl.GetBuffer(0), "%x", &len);
		len++;
		/* The tags availale at IDS do not support 'Write Multiple Blocks' 
		strcat(cmd, CC_WRITE_MP);
		if (t15bAddress)
			err += addHex(cmd, CMD_LEN, 0, t15UID, "UID");
		err += addHex(cmd, CMD_LEN, 2, t15FirstBN, "First Block Number");
		err += addHex(cmd, CMD_LEN, 2, t15NumBl, "Number of Blocks");
		err += addHex(cmd, CMD_LEN, 2 * 4 * len, t15Data, "Data");*/
		 
		// Write each block with 'Write Single Block'
		
		sscanf(t15FirstBN.GetBuffer(0), "%x", &bn);
		mwData[0] = 0;
		err += addHex(mwData, CMD_LEN, 2 * 4 * len, t15Data, "Data");

		p = mwData;
		p += 8*(len-1);

		for (i = 0; i < len; i++, bn++, p -= 8) {
			
			if(i)
			{
				strcpy(cmd, "18");
				sprintf(hex, "%2.2X", flags);
				strcat(cmd, hex);
			}


		strcat(cmd, CC_WRITE_SB);
		//changeByteOrder_MB(mwData,mwDataOrdered,len);
		if (t15bAddress)
			err += addHex(cmd, CMD_LEN, 0, t15UID, "UID");
		//strcpy(mwCmd, cmd);
			//strcpy(cmd, mwCmd);
		    //strcat(cmd, mwCmd);
			sprintf(hex, "%02X", bn);
			strcat(cmd, hex);
			//err += addHex(cmd, CMD_LEN, 2, t15FirstBN, "First Block Number");
			strncat(cmd, p, 8);
			if (portWrite(cmd, 0, 0))
				return 1;
			// Read the reply
			int nn;
			nn = portRead(reply, CMD_LEN);
			}
		return 0;
		break;
	  case QUIET:
		strcat(cmd, CC_QUIET);
		if (t15bAddress)
			err += addHex(cmd, CMD_LEN, 0, t15UID, "UID");
		break;
	  case SELECT:
		strcat(cmd, CC_SELECT);
		if (t15bAddress)
			err += addHex(cmd, CMD_LEN, 0, t15UID, "UID");
		break;
	  case READY:
		strcat(cmd, CC_READY);
		if (t15bAddress)
			err += addHex(cmd, CMD_LEN, 0, t15UID, "UID");
		break;
	  case WRITE_AFI:
		strcat(cmd, CC_WRITE_API);
		if (t15bAddress)
			err += addHex(cmd, CMD_LEN, 0, t15UID, "UID");
		err += addHex(cmd, CMD_LEN, 2, t15AFI, "AFI");
		break;
	  case LOCK_AFI:
		strcat(cmd, CC_LOCK_API);
		if (t15bAddress)
			err += addHex(cmd, CMD_LEN, 0, t15UID, "UID");
		break;
	  case WRITE_DSFID:
		strcat(cmd, CC_WRITE_DSFID);
		if (t15bAddress)
			err += addHex(cmd, CMD_LEN, 0, t15UID, "UID");
		err += addHex(cmd, CMD_LEN, 2, t15DSFID, "DSFID");
		break;
	  case LOCK_DSFID:
		strcat(cmd, CC_LOCK_DSFID);
		if (t15bAddress)
			err += addHex(cmd, CMD_LEN, 0, t15UID, "UID");
		break;
	  case SYSTEM_INFO:
		strcat(cmd, CC_GET_INFO);
		if (t15bAddress)
			err += addHex(cmd, CMD_LEN, 0, t15UID, "UID");
		break;
	  case MBS_STATUS:
		strcat(cmd, CC_GET_SEC_STAT);
		if (t15bAddress)
			err += addHex(cmd, CMD_LEN, 0, t15UID, "UID");
		err += addHex(cmd, CMD_LEN, 2, t15FirstBN, "First Block Number");
		err += addHex(cmd, CMD_LEN, 2, t15NumBl, "Number of Blocks");
		break;
	  case LOCK_TWO_BLOCKS:
		  strcat(cmd, CC_LOCK_TWO_BLOCKS);
		  strcat(cmd, MFG_CODE);
		if(t15bAddress) err += addHex(cmd, CMD_LEN, 0, t15UID, "UID");
		err += addHex(cmd, CMD_LEN, 2, t15FirstBN, "First Block Number");
		break;
	  case WRITE_TWO_BLOCKS:
		strcat(cmd, CC_WRITE_TWO_BLOCKS);
		strcat(cmd, MFG_CODE);
		if(t15bAddress) err += addHex(cmd, CMD_LEN, 0, t15UID, "UID");
		err += addHex(cmd, CMD_LEN, 2, t15FirstBN, "First Block Number");
		CString temp;
		temp = t15Data.Right(8) + t15Data.Left(8);		
		
		err += addHex(cmd, CMD_LEN, 2 * 8, temp, "Data");
		break;
	  }
	if (err) {
		hopa("Send aborted");
		return 1;
		}
	if (portWrite(cmd, 0, 0))
		return 1;

	//Sleep(100);
	// Read the reply
	int nn;
	nn = portRead(reply, CMD_LEN);
	return 0;
}

void changeByteOrder(char *out, char *in)
{
	int len = strlen(in), i;
	for (i = 0; i < len; i += 2) {
		out[len-2-i] = in[i];
		out[len-1-i] = in[i+1];
		}
	out[len] = 0;
}

static int getTagInfo(char *data, char *uid, unsigned char *dsfid,
			unsigned char *afi, unsigned char *blSize,
			unsigned char *numBl)
{
	char tmp[8], *p = data, *d;
	int i;
	for (d = uid, i = 0; i < 16; i++)	*d++ = *p++;
	*d = 0;
	for (d = tmp, i = 0; i < 2; i++)	*d++ = *p++;
	*d = 0;
	sscanf(tmp, "%x", dsfid);
	for (d = tmp, i = 0; i < 2; i++)	*d++ = *p++;
	*d = 0;
	sscanf(tmp, "%x", afi);
	for (d = tmp, i = 0; i < 2; i++)	*d++ = *p++;
	*d = 0;
	sscanf(tmp, "%x", numBl);
	for (d = tmp, i = 0; i < 2; i++)	*d++ = *p++;
	*d = 0;
	sscanf(tmp, "%x", blSize);
	return 0;
}

void CTab15693::On15Execute() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	if ((selectedProtocol == -1 || newProtocol) && ! expert ) {
		MessageBox("Please, set Protocol");
		return;
		}
	if (m_i15CommandRB == -1) {
		MessageBox("Pleasse, select Command");
		return;
		}
	// Save flags
	t15bSubCarrier = m_b15SubCarrier;
	t15bDataRate = m_b15DataRate;
	t15bInventory = m_i15CommandRB ? 0 : 1;
	t15bSelect = m_b15Select;
	t15bAddress = m_b15Address;
	t15bOption = m_b15Option;
	// Save data
	char data[CMD_LEN];
	changeByteOrder(data, m_s15UID.GetBuffer(0));	// UID byte order
	t15UID = data;
	t15FirstBN = m_s15FirstBN;
	t15NumBl = m_s15NumBl;
	changeByteOrder(data, m_s15Data.GetBuffer(0));	// data byte order
	t15Data = data;
	t15DSFID = m_s15DSFID;
	t15AFI = m_s15AFI;

	char reply[CMD_LEN];
	if (execute(m_i15CommandRB, reply))
		return;

	char uid[36], uid2[36], buf[36], rssi[12], rssi2[12], *urssi, *p;
	int collision, ii;
//	CString cuid;
	if (! m_i15CommandRB) {	// Inventory: load available UIDs
		m_15RSSI.DeleteAllItems();
		m_15UID.ResetContent();

		//Clear all screen values
		strcpy(UIDs[0],"");
		m_s15DSFID = "";
		m_s15AFI = "";
		m_s15TagNB = "";
		m_s15TagBS = "";
		m_s15Data  = "";
		m_s15FirstBN = "";
		m_s15NumBl = "";

		int index, cnt = 0, i;
		for (p = reply, i = 0; i < 4*MAX_UID; i++) {
			if (! (p = strchr(p, '[')))
				break;
			p++;
//			if (*p && (*p == ']' || *p == 'z' || *p == 'Z'))
			if (*p && (*p == ']'))
				continue;
			if (*p && (*p == 'z' || *p == 'Z')) {
				p++;
				strcpy(uid, "");
				}
			else
				if (getXWord(&p, uid))	continue;
			if (strlen(uid)) {
				changeByteOrder(uid2, uid);
				strcpy(UIDs[cnt], uid2);
				index = m_15UID.AddString(uid2);
				m_15UID.SetItemData(index, cnt);
				cnt++;
				}
			// RSSI
			if (*p != ',')	continue;
			p++;
			if (getXWord(&p, rssi))	continue;
			strcpy(rssi2, hex2rssi(rssi));
			if (rssi2[2] == '0' && rssi2[4] == '0' &&
							! strlen(uid))
				continue;
			sprintf(buf, "%d", i % 16);
			collision = i / 16;
			switch (collision) {
				case 1: strcat(buf, "A"); break;
				case 2: strcat(buf, "B"); break;
				case 3: strcat(buf, "C"); break;
				case 4: strcat(buf, "D"); break;
				case 5: strcat(buf, "E"); break;
				}
			ii = m_15RSSI.InsertItem(i, buf);
			urssi = strlen(uid) ? uid2 : uid;
			m_15RSSI.SetItemText(ii, 1, urssi);
			buf[0] = rssi2[2]; buf[1] = 0;
			m_15RSSI.SetItemText(ii, 2, buf);
			buf[0] = rssi2[4];
			m_15RSSI.SetItemText(ii, 3, buf);
			if (cnt >= MAX_UID)
				break;
			}
		m_15UID.SetCurSel(0);
		m_s15UID = UIDs[0];
		char num[8];
		sprintf(num, "%d", cnt);
		m_15UIDNo = num;
		UpdateData(FALSE);	// Set screen values
 		}
	if (m_i15CommandRB == READ_SB) 
	{
		char data[CMD_LEN], *p, *e;
		if ((p = strchr(reply, '[')) && (e = strchr(p, ']'))) {
			p++;
			p += 2;
			*e = 0;
			
			changeByteOrder(data, p);
			m_s15Data = data;			

			UpdateData(FALSE);	// Set screen values
			}
 		}
	if (m_i15CommandRB == READ_MB) {	// Read Multiple Block
		char data[CMD_LEN], *p, *e;
		CString d;
		int numblocks;
		if ((p = strchr(reply, '[')) && (e = strchr(p, ']')))
		{
			p++;
			p += 2;
			*e = 0;
			numblocks = atoi(t15NumBl);			
			
			changeByteOrder_MB(data, p,numblocks+1);
			m_s15Data = data;			

			UpdateData(FALSE);	// Set screen values
			
 		}
	}
	if (m_i15CommandRB == SYSTEM_INFO) {	// Get system information
		if (! (p = strchr(reply, '[')))	return;
		p++;
		if (*p && (*p == ']' || *p == 'z' || *p == 'Z'))	return;
		p += 4;
		if (getXWord(&p, data))	return;
		unsigned char dsfid, afi, blSize, numBl;
		getTagInfo(data, uid, &dsfid, &afi, &blSize, &numBl);
		changeByteOrder(uid2, uid);
		if (m_b15Address && strcmp(m_s15UID.GetBuffer(0), uid2)) {
			char msg[80];
			sprintf(msg, "Invalid UID number in reply %s !- %s",
				uid2, m_s15UID.GetBuffer(0));
			hopa(msg);
			return;
			}
		m_s15DSFID.Format("%2.2x", dsfid);
		m_s15AFI.Format("%2.2x", afi);
		m_s15TagNB.Format("%2.2x", numBl);
		m_s15TagBS.Format("%2.2x", blSize);
		UpdateData(FALSE);	// Set screen values
 		}
}

void changeByteOrder_MB(char *out, char *in, int numblocks)
{
	int i,j,z;
	int len = strlen(in)/numblocks;
	for (j = 0; j < numblocks; j++)
	{
		z = j*8;
		for (i = 0; i < len; i += 2) {
			out[len-2-i+z] = in[i+z];
			out[len-1-i+z] = in[i+1+z];
			}
	}

	out[strlen(in)] = 0;
}

int getXWord (char **str, char *word)
{
	char *p = *str, *d;
	int nogood = 0;
	for (d = word; *p && *p != ',' && *p != ']'; ) {
		if (! isxdigit(*p)) {
			nogood = 1;
			break;
			}
		*d++ = *p++;
		}
	*d = 0;
	*str = p;
	return nogood;
}

int addHex(char *buf, int bufLen, int expectedLen, CString  str, char *what)
{
	char *p;
	int odd = 0, err = 0, i;
	CString msg;
	if (! strlen(str)) {
		msg.Format("Empty %s", what);
		hopa(msg); err++;
		}
	if (expectedLen < 0) {
		odd = 1;
		expectedLen = -expectedLen;
		}
	if (expectedLen && str.GetLength() != expectedLen) {
		if (str.GetLength() < expectedLen)
			msg.Format("Short %s", what);
		else
			msg.Format("Long %s", what);
		hopa(msg); err++;
		}
	for (i = 0, p = str.GetBuffer(0); *p; p++, i++) {
		if (! isxdigit(*p)) {
			msg.Format("Invalid characters in %s", what);
			hopa(msg); err++;
			break;
			}
		}
	if (! odd && i % 2) {
		msg.Format("Enter even number of characters in %s", what);
		hopa(msg); err++;
		}
	if ((int)(strlen(buf) + str.GetLength()) > bufLen) {
		msg.Format("Command too long (%s)", what);
		hopa(msg); err++;
		}
	if (! err)
		strcat(buf, str.GetBuffer(0));
	return err;
}

int parse15693 (int doit, char *line, char *path, int lineno, char *reply)
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
	  case INVENTORY: case READ_SB: case WRITE_SB: case LOCK_B:
	  case READ_MB: case WRITE_MB: case QUIET: case SELECT:
	  case READY: case WRITE_AFI: case LOCK_AFI: case WRITE_DSFID:
	  case LOCK_DSFID: case SYSTEM_INFO: case MBS_STATUS:
		t15bInventory = (cmd != INVENTORY) ? 0 : 1;
		if (doit)
			execute(cmd, reply);
		break;
	  case PROTOCOL:
		p = t15Protocol.GetBuffer(0);
		cmd = lookup(p, dataCodingMode);
		if (doit)
			send(cmd);
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
			  case F_SUB_CARRIER:
				t15bSubCarrier = flag;
				break;
			  case F_DATA_RATE:
				t15bDataRate = flag;
				break;
			  case F_SELECT:
			  case F_AFI:
				t15bSelect = flag;
				break;
			  case F_ADDRESS:
			  case F_ONE_SLOT:
				t15bAddress = flag;
				break;
			  case F_OPTION:
				t15bOption = flag;
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
		if (cmd == D_CODING){
			int cmdx = lookup(word, dataCodingMode);
			if (cmdx == -1) {
				msg.Format("Invalid data coding '%s' in line %d", word, lineno);
				hopa(msg);
				return 1;
				}
			}
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
				t15UID = data;
				break;
			  case D_FIRST_B:
				t15FirstBN = data;
				break;
			  case D_NUM_BL:
				t15NumBl = data;
				break;
			  case D_DATA:
				t15Data = data;
				break;
			  case D_DSFID:
				t15DSFID = data;
				break;
			  case D_AFI:
				t15AFI = data;
				break;
			  case D_CODING:
				t15Protocol = data;
				break;
			  case D_POWER:
				t15iFullPower = cmdx;
				break;
			  }
			}
		break;
	  }
	return 0;
}

int find15693Tags (TagID *ids, int num)
{
	if (MAX_ID_LEN < UID_LEN)
		return 1;
	t15bSubCarrier = 0;
	t15bDataRate = 1;
	t15bInventory = 1;
	t15bSelect = 0;
	t15bAddress = 0;
	t15bOption = 0;
	if (send(0))			/* 1 out of 4 */
		return 1;
	char reply[CMD_LEN];
#if NO_COM
	strcpy(reply, "_15693_");
#endif
	if (execute(INVENTORY, reply))
		return 1;
	char uid[34], uid2[34], *p;
	int cnt = 0, i;
	TagID *id = ids;
	for (p = reply, i = 0; i < 4*MAX_UID; i++) {
		if (! (p = strchr(p, '[')))
			break;
		p++;
		if (*p && (*p == ']' || *p == 'z' || *p == 'Z'))
			continue;
		if (getXWord(&p, uid))	continue;
		if (strlen(uid) == 16) {
			changeByteOrder(uid2, uid);
			strcpy(id->id, uid2);
			id++;
			cnt++;
			}
		if (*p != ',')	continue;
		p++;
		if (getXWord(&p, uid))	continue;
		if (cnt >= num)
			break;
		}
	id->id[0] = 0;
	return 0;
}

void CTab15693::On15UID() 
{
	UpdateData(TRUE);	// Get curret values from the screen
	int index = m_15UID.GetCurSel(), i;
	char sel[UID_LEN];
	m_15UID.GetLBText(index, sel);
	for (i = 0; i < MAX_UID; i++) {
		if (! strcmp(sel, UIDs[i]))
			break;
		}
	if (i < MAX_UID) {
		m_s15UID = UIDs[i];
//		m_s15DSFID = DSFIDs[i];
		}
	UpdateData(FALSE);	// Set screen values
}

BOOL CTab15693::OnInitDialog()
{
	CDialog::OnInitDialog();

	int err;
	err = m_15RSSI.InsertColumn(0, "#", LVCFMT_LEFT, 30);
	err = m_15RSSI.InsertColumn(1, "UID", LVCFMT_CENTER, 115);
	err = m_15RSSI.InsertColumn(2, "M", LVCFMT_LEFT, 20);
	err = m_15RSSI.InsertColumn(3, "A", LVCFMT_LEFT, 20);
	m_i15CommandRB = 0;
	UpdateData(FALSE);	// Set screen values
	return TRUE;
}

void CTab15693::OnTicc() 
{
	if(set == 0)
	{
		/* Hide all commands */
		/*
		GetDlgItem(IDC_15_INVENTORY_RB)->ShowWindow(FALSE);
		GetDlgItem(IDC_15_READ_SB_RB)->ShowWindow(FALSE);
		GetDlgItem(IDC_15_WRITE_SB_RB)->ShowWindow(FALSE);
		GetDlgItem(IDC_15_LOCK_RB)->ShowWindow(FALSE);
		GetDlgItem(IDC_15_READ_MB_RB)->ShowWindow(FALSE);
		GetDlgItem(IDC_15_WRITE_MB_RB)->ShowWindow(FALSE);
		GetDlgItem(IDC_15_QUIET_RB)->ShowWindow(FALSE);
		GetDlgItem(IDC_15_SELECT_RB)->ShowWindow(FALSE);
		GetDlgItem(IDC_15_READY_RB)->ShowWindow(FALSE);
		GetDlgItem(IDC_15_WRITE_AFI_RB)->ShowWindow(FALSE);
		GetDlgItem(IDC_15_LOCK_AFI_RB)->ShowWindow(FALSE);
		GetDlgItem(IDC_15_WRITE_DSIF_RB)->ShowWindow(FALSE);
		GetDlgItem(IDC_15_LOCK_DSIF_RB)->ShowWindow(FALSE);
		GetDlgItem(IDC_15_SYS_INFO_RB)->ShowWindow(FALSE);
		GetDlgItem(IDC_15_SECURITY_RB)->ShowWindow(FALSE); */

		/* Show all TI Custom Commands */
		GetDlgItem(IDC_WRITE_TWO)->ShowWindow(TRUE);
		GetDlgItem(IDC_LOCK_TWO)->ShowWindow(TRUE);

		set = 1;
	}
	else
	{
		/*
		GetDlgItem(IDC_15_INVENTORY_RB)->ShowWindow(TRUE);
		GetDlgItem(IDC_15_READ_SB_RB)->ShowWindow(TRUE);
		GetDlgItem(IDC_15_WRITE_SB_RB)->ShowWindow(TRUE);
		GetDlgItem(IDC_15_LOCK_RB)->ShowWindow(TRUE);
		GetDlgItem(IDC_15_READ_MB_RB)->ShowWindow(TRUE);
		GetDlgItem(IDC_15_WRITE_MB_RB)->ShowWindow(TRUE);
		GetDlgItem(IDC_15_QUIET_RB)->ShowWindow(TRUE);
		GetDlgItem(IDC_15_SELECT_RB)->ShowWindow(TRUE);
		GetDlgItem(IDC_15_READY_RB)->ShowWindow(TRUE);
		GetDlgItem(IDC_15_WRITE_AFI_RB)->ShowWindow(TRUE);
		GetDlgItem(IDC_15_LOCK_AFI_RB)->ShowWindow(TRUE);
		GetDlgItem(IDC_15_WRITE_DSIF_RB)->ShowWindow(TRUE);
		GetDlgItem(IDC_15_LOCK_DSIF_RB)->ShowWindow(TRUE);
		GetDlgItem(IDC_15_SYS_INFO_RB)->ShowWindow(TRUE);
		GetDlgItem(IDC_15_SECURITY_RB)->ShowWindow(TRUE);
		*/

		/* Hide all TI Custom Commands */
		GetDlgItem(IDC_WRITE_TWO)->ShowWindow(FALSE);
		GetDlgItem(IDC_LOCK_TWO)->ShowWindow(FALSE);

		set = 0;
	}	
}
