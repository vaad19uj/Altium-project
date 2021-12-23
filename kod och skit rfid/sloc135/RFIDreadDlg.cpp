/*$T RFIDreadDlg.cpp GC 1.140 10/11/06 16:19:35 */

/*
 * RFIDreadDlg.cpp : implementation file ;
 * This file is part of an RFID GUI. It is distributed as is ;
 * and Texas Instruments does not provide any support. ;
 * It is distributed in the hope that it will be useful, ;
 * but WITHOUT ANY WARRANTY;
 * without even the implied warranty of ;
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * 
 */
#include "stdafx.h"
#include "RFIDread.h"
#include "RFIDreadDlg.h"
#include "TabTest.h"
#include "CP210xManufacturingDLL.h"
#include <fstream>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char		THIS_FILE[] = __FILE__;
#endif
int				topEnableTRF = 1;
static CString	logBuf;
/*
 =======================================================================================================================
    CAboutDlg dialog used for App About
 =======================================================================================================================
 */
class CAboutDlg :
	public CDialog
{
/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	CAboutDlg();

	/* Dialog Data */
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA
	/* ClassWizard generated virtual function overrides */
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void	DoDataExchange(CDataExchange *pDX); // DDX/DDV support
	//}}AFX_VIRTUAL
/*
 -----------------------------------------------------------------------------------------------------------------------
    Implementation
 -----------------------------------------------------------------------------------------------------------------------
 */
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */

CAboutDlg::CAboutDlg() :
	CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
// No message handlers
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/*
 * CRFIDreadDlg dialog ;
 * make it avalable to external functions:
 */
static CRFIDreadDlg *RRdialog;

/*
 =======================================================================================================================
 =======================================================================================================================
 */

CRFIDreadDlg::CRFIDreadDlg(CWnd *pParent /* NULL */ ) :
	CDialog(CRFIDreadDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRFIDreadDlg)
	m_sLogger = _T("");
	m_sIRQTx = _T("");
	m_sIRQRx = _T("");
	m_sIRQFIFO = _T("");
	m_sIRQCRC = _T("");
	m_sIRQParity = _T("");
	m_sIRQFraming = _T("");
	m_sIRQSOF = _T("");
	m_sIRQColl = _T("");
	m_sFIFOLevH = _T("");
	m_sFIFOLevL = _T("");
	m_sFIFOFull = _T("");
	m_bAGCon = FALSE;
	m_bMainChannelAM = TRUE;
	m_bEnableTRF = TRUE;
	//}}AFX_DATA_INIT
	/* Note that LoadIcon does not require a subsequent DestroyIcon in Win32 */
	m_hIcon = AfxGetApp()->LoadIcon(IDR_TI_EVM);
	logFileOnly = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CRFIDreadDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRFIDreadDlg)
	DDX_Control(pDX, IDC_ENABLE_TRF, m_EnableTRF);
	DDX_Control(pDX, IDC_COM_PORT, m_ComPort);
	DDX_Control(pDX, IDC_LOGGER, m_Logger);
	DDX_Control(pDX, IDC_EDIT1, m_ComPortMan);
	DDX_Control(pDX, IDC_MY_TAB, m_tabMyTabCtrl);
	DDX_Text(pDX, IDC_LOGGER, m_sLogger);
	DDX_Text(pDX, IDC_IRQ_TX, m_sIRQTx);
	DDX_Text(pDX, IDC_IRQ_RX, m_sIRQRx);
	DDX_Text(pDX, IDC_IRQ_FIFO, m_sIRQFIFO);
	DDX_Text(pDX, IDC_IRQ_CRC, m_sIRQCRC);
	DDX_Text(pDX, IDC_IRQ_PARITY, m_sIRQParity);
	DDX_Text(pDX, IDC_IRQ_FRAMING, m_sIRQFraming);
	DDX_Text(pDX, IDC_IRQ_SOF, m_sIRQSOF);
	DDX_Text(pDX, IDC_IRQ_COLL, m_sIRQColl);
	DDX_Text(pDX, IDC_FIFO_LEV_H, m_sFIFOLevH);
	DDX_Text(pDX, IDC_FIFO_LEV_L, m_sFIFOLevL);
	DDX_Text(pDX, IDC_FIFO_FULL, m_sFIFOFull);
	DDX_Check(pDX, IDC_AGC_ON, m_bAGCon);
	DDX_Check(pDX, IDC_MCHAN_AM, m_bMainChannelAM);
	DDX_Check(pDX, IDC_ENABLE_TRF, m_bEnableTRF);

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRFIDreadDlg, CDialog)
//{{AFX_MSG_MAP(CRFIDreadDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_CBN_EDITCHANGE(IDC_COM_PORT, OnComPort)
	ON_BN_CLICKED(IDC_MCHAN_AM, OnMainChannelAM)
	ON_BN_CLICKED(IDC_AGC_ON, OnAGCOn)
	ON_BN_CLICKED(IDC_ENABLE_TRF, OnEnableTRF)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_CBN_SELCHANGE(IDC_COM_PORT, OnComPort)
	ON_BN_CLICKED(IDC_AUTOCOMPORT, OnAutocomport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*
 =======================================================================================================================
    CRFIDreadDlg message handlers
 =======================================================================================================================
 */
BOOL CRFIDreadDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	/*
	 * Add "About..." menu item to system menu. ;
	 * IDM_ABOUTBOX must be in the system command range.
	 */
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CMenu	*pSysMenu = GetSystemMenu(FALSE);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(pSysMenu != NULL)
	{
		/*~~~~~~~~~~~~~~~~~*/
		CString strAboutMenu;
		/*~~~~~~~~~~~~~~~~~*/

		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if(!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	/*
	 * Set the icon for this dialog. The framework does this automatically ;
	 * when the application's main window is not a dialog
	 */
	SetIcon(m_hIcon, TRUE);		/* Set big icon */
	SetIcon(m_hIcon, FALSE);	/* Set small icon */

#if SETUP_7960
	SetWindowText("TRF7960 EVM Control");
#elif SETUP_7961
	SetWindowText("TRF7961 EVM Control");
#endif
#if SETUP_NFC || SETUP_NFC2
	SetWindowText("TRF7965 NFC Demonstrator Control");
#endif
#if SETUP_IDS0 || SETUP_IDS2
	SetWindowText("RFID Reader Control");
	GetDlgItem(IDR_IDS_ICON)->ShowWindow(SW_SHOWNA);
	GetDlgItem(IDR_TI_ICON)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_TOP_IRQ1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_TOP_IRQ2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_TOP_IRQ3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_TOP_IRQ4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_TOP_IRQ5)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_TOP_IRQ6)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_TOP_IRQ7)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_TOP_IRQ8)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_TOP_IRQ9)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_IRQ_TX)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_IRQ_RX)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_IRQ_FIFO)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_IRQ_CRC)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_IRQ_PARITY)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_IRQ_FRAMING)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_IRQ_SOF)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_IRQ_COLL)->ShowWindow(SW_HIDE);
#endif

	/*~~~~~~~~*/
	int cnt = 0;
	/*~~~~~~~~*/

	m_tabMyTabCtrl.InsertItem(cnt++, _T("15693"));
#if SETUP_7960
	m_tabMyTabCtrl.InsertItem(cnt++, _T("14443A"));
#endif
#if !SETUP_7961 || SETUP_7960
	m_tabMyTabCtrl.InsertItem(cnt++, _T("14443B"));
#endif
#if !SETUP_NFC && !SETUP_NFC2 && !SETUP_IDS0
	m_tabMyTabCtrl.InsertItem(cnt++, _T("Tagit"));
#endif
#if SETUP_7960
#if SETUP_FELICA
	m_tabMyTabCtrl.InsertItem(cnt++, _T("FeliCa"));
#endif
	m_tabMyTabCtrl.InsertItem(cnt++, _T("EPC"));
#endif
	m_tabMyTabCtrl.InsertItem(cnt++, _T("Find tags"));
	m_tabMyTabCtrl.InsertItem(cnt++, _T("Registers"));
#if SETUP_NFC || SETUP_NFC2
	m_tabMyTabCtrl.InsertItem(cnt++, _T("NFC"));
#endif
#if SHOW_TEST
	m_tabMyTabCtrl.InsertItem(cnt++, _T("Test"));
#endif
	m_tabMyTabCtrl.Init();

	RRdialog = this;
	m_ComPort.SetCurSel(0);
#if MEM_LOG
	memlogAdd((char *) 0);
#endif
#if SETUP_7960
	m_EnableTRF.SetWindowText("Enable TRF7960");
#endif
#if (SETUP_7960 || SETUP_7961) && !SETUP_IDS0
	GetDlgItem(IDC_ENABLE_TRF)->ShowWindow(SW_SHOWNA);

#endif
/*	if(!portFindPort())
	{
		MessageBox("Port not found! Please select port manually.", "Error", MB_OK);
		GetDlgItem(IDC_EDIT1)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
		m_ComPortMan.SetLimitText(5);
	}*/

/*Added Harsha - Disable Automatic Comport Recognition*/
		GetDlgItem(IDC_EDIT1)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
		m_ComPortMan.SetLimitText(5);


	UpdateData(FALSE);			/* Set screen values */


//Auto ComPort Code - BEGIN *********************************
		m_hUSBDevice = INVALID_HANDLE_VALUE; //Initialize 

	//////////////////////////////////////////////////////////////
	CP210x_STATUS status = CP210x_GetNumDevices(&m_dwNumDevices);
	
	//////////////////////////////////////////////////////////////
	
	UpdateData(TRUE);
	CString m_Data = "";
   // BOOL portstatus;


	//Variables to store Vid, Pid and SerialNumbers
	WORD Vid, Pid;
	CP210x_SERIAL_STRING serialNum;


	// Query the Vid, Pid and Serial number of each device.
		// We will use this information to look up the device
		// in the registry to find the port number. There are also
		// examples of displaying other items in the registry as well.

     	//Loop through and display data for each connected device
	for (int i = 0; i < m_dwNumDevices; i++)
	{

		// Open device
		CP210x_STATUS status = CP210x_Open(0, &m_hUSBDevice);
		if (status == CP210x_SUCCESS)
		{
			// Get Vid
			status = CP210x_GetDeviceVid(m_hUSBDevice, &Vid);
			if (status == CP210x_SUCCESS)
			{
				// Get Pid
				status = CP210x_GetDevicePid(m_hUSBDevice, &Pid);
				if (status == CP210x_SUCCESS)
				{
					BYTE bLength = 0;
					CString tmpDevStr;

					// Get Serial Number
					status = CP210x_GetDeviceSerialNumber(m_hUSBDevice, serialNum, &bLength);

					// Null terminate to make it a string
					serialNum[bLength] = '\0';

				
				}
			}
		}

		// This is a function call to obtain the port number
		// by passing the Vid, Pid, and SN of the device in
		// question. An integer value with the port is passed back
	    	int Port;
 			Port = GetPortNumXP2000(Vid, Pid, serialNum);
		//	Port = 10;

		char* tmpPortString;
		tmpPortString = new char[10];
		m_Data += "com";
		m_Data += itoa(Port, tmpPortString, 10);


        //Paste the comPort data into the edit box
		

		m_ComPortMan.SetWindowText(m_Data);

/*
   
		if (Port != -1)
		{
		
  		comPort.Format("\\\\.\\%s", m_Data);
		 portstatus = portFindSinglePort();	//Open the port specified

		}
*/		 

        	//Close the USB Device
		CP210x_Close(m_hUSBDevice);
		

	}

	UpdateData(FALSE);
	// autoComPort Code End *********************************


	CRFIDreadDlg::OnButton1();




	return TRUE;				/* return TRUE unless you set the focus to a control */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CRFIDreadDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		/*~~~~~~~~~~~~~~~~~*/
		CAboutDlg	dlgAbout;
		/*~~~~~~~~~~~~~~~~~*/

		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

/*
 =======================================================================================================================
    If you add a minimize button to your dialog, you will need the code below ;
    to draw the icon. For MFC applications using the document/view model, ;
    this is automatically done for you by the framework.
 =======================================================================================================================
 */
void CRFIDreadDlg::OnPaint()
{
	if(IsIconic())
	{
		/*~~~~~~~~~~~~~~~~~*/
		CPaintDC	dc(this);	/* device context for painting */
		/*~~~~~~~~~~~~~~~~~*/

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		/* Center icon in client rectangle */
		int		cxIcon = GetSystemMetrics(SM_CXICON);
		int		cyIcon = GetSystemMetrics(SM_CYICON);
		CRect	rect;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		GetClientRect(&rect);

		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* Draw the icon */
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

/*
 =======================================================================================================================
    The system calls this to obtain the cursor to display while the user drags ;
    the minimized window.
 =======================================================================================================================
 */
HCURSOR CRFIDreadDlg::OnQueryDragIcon()
{
	return(HCURSOR) m_hIcon;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CRFIDreadDlg::logAddFileOnly()
{
	logFileOnly = 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CRFIDreadDlg::logAddFileScreen()
{
	logFileOnly = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CRFIDreadDlg::logAdd(CString msg, int where, int severity)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static ofstream log;
	static int		logToFile = LOG_TO_FILE, logOpen = 0;
	static CString	zapri = "-99";
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!logOpen)
	{
		log.open("rfid-reader.log");
		logOpen = 1;
	}

	if(msg == "-99")
	{
		log.close();
		logOpen = 0;
		return;
	}

	/*~~~~~~~~~~~~*/
	/* Write to log file */
	SYSTEMTIME	now;
	/*~~~~~~~~~~~~*/

	/*
	 * GetSystemTime(&now);
	 */
	GetLocalTime(&now);

	/*~~~~~~~~~~~~~~~~*/
	char	timeStr[16];
	/*~~~~~~~~~~~~~~~~*/

	/*
	 * sprintf(timeStr, "%2.2d:%2.2d:%2.2d", now.wHour, now.wMinute, now.wSecond);
	 */
	sprintf(timeStr, "%2.2d:%2.2d:%2.2d.%3.3d", now.wHour, now.wMinute, now.wSecond, now.wMilliseconds);

	if(logToFile)
	{
		log << timeStr << "\t" << msg.GetBuffer(0) << endl;
		log.flush();
	}

	if(where) return;
	if(logFileOnly) return;

	/* Show the information in the log window */
	UpdateData(TRUE);

	/*~~~~~~~~~~~~~~~~~~~~~*/

	static			init = 1;
	int				n;
	CString			add;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	n = m_Logger.GetLineCount();
	if(n > LOG_LIMIT)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		int		n2, i;
		char	*p = logBuf.GetBuffer(0);
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		for(i = n2 = LOG_LIMIT / 8 + 1; n2; n2--) p = strchr(p, '\r');
		p++;

		/*~~~~~~~~~~~*/
		/*
		 * if ((i = strlen(p)) > LOG_LIMIT * 100) { ;
		 * i++;
		 * } ;
		 * char tmp[LOG_LIMIT * 100];
		 * strcpy(tmp, p);
		 * p = tmp;
		 */
		CString tmpBuf;
		/*~~~~~~~~~~~*/

		tmpBuf = p;
		p = tmpBuf.GetBuffer(0);
		logBuf = p;
		UpdateData(FALSE);
		n -= n2;
	}

	add.Format("%s\t%s\r\n", timeStr, msg);
	if(init)
	{
		init = 0;
		logBuf = add;
		m_Logger.SetLimitText(LOG_LIMIT * 100);
	}
	else
		logBuf = logBuf + add;
	m_sLogger = logBuf;

	
	UpdateData(FALSE);

	m_Logger.LineScroll(n-1,0);
	

		
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CRFIDreadDlg::showErr(CString msg)
{
	MessageBox(msg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void logAddFileOnly()
{
	RRdialog->logAddFileOnly();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void logAddFileScreen()
{
	RRdialog->logAddFileScreen();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void logAdd(CString msg, int where, int severity)
{
	RRdialog->logAdd(msg, where, severity);
}

#if MEM_LOG
char	memlog[20000];

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void memlogAdd(char *str)
{
	if(str == (char *) 0)
	{
		memlog[0] = 0;
		return;
	}
	else if(str == (char *) 9)
	{
		/*~~~~~~~~~~~~*/
		ofstream	mem;
		/*~~~~~~~~~~~~*/

		mem.open("memlog.log");
		mem << memlog << endl;
		mem.close();
		return;
	}

	/*~~~~~~~~~~~~*/
	SYSTEMTIME	now;
	/*~~~~~~~~~~~~*/

	GetLocalTime(&now);

	/*~~~~~~~~~~~~~~~~*/
	char	timeStr[16];
	/*~~~~~~~~~~~~~~~~*/

	sprintf(timeStr, "%2.2d:%2.2d:%2.2d.%3.3d ", now.wHour, now.wMinute, now.wSecond, now.wMilliseconds);
	strcat(memlog, timeStr);
	strcat(memlog, str);
	strcat(memlog, "\r\n");
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void hopa(CString msg)
{
	RRdialog->showErr(msg);
	RRdialog->logAdd(msg, 0, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CRFIDreadDlg::OnExit()
{
#if !SETUP_7961
	portWrite("00F", 9, 0); /* reset temporary ASIC */
#endif
	RRdialog->logAdd((CString) "-99", 0, 0);
	OnOK();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CRFIDreadDlg::OnReset()
{
	/*~~~~~~~~~~~~~~~~~*/
	char	cmd[CMD_LEN];
	CString text;
	/*~~~~~~~~~~~~~~~~~*/

	strcpy(cmd, "15");	/* read register */
	strcat(cmd, "0F");	/* register # */
	if(portWrite(cmd, 0, 0)) return;
	portRead(cmd, CMD_LEN);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CRFIDreadDlg::OnUpdate()
{
	UpdateData(TRUE);

	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/*
	 * Get curret values from the screen ;
	 * Read IC status register (0C)
	 */
	char	cmd[CMD_LEN], *p, *e;
	int		x;
	CString text;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	strcpy(cmd, "12");		/* read register */
	strcat(cmd, "0C");		/* register # */
	if(portWrite(cmd, 0, 0)) return;
	portRead(cmd, CMD_LEN);
	if((p = strchr(cmd, '[')) && (e = strchr(p, ']')))
	{
		p++;
		*e = 0;
		sscanf(p, "%x", &x);
		text = (x & 0x80) ? "1" : "0";
		m_sIRQTx = text;
		text = (x & 0x40) ? "1" : "0";
		m_sIRQRx = text;
		text = (x & 0x20) ? "1" : "0";
		m_sIRQFIFO = text;
		text = (x & 0x10) ? "1" : "0";
		m_sIRQCRC = text;
		text = (x & 0x08) ? "1" : "0";
		m_sIRQParity = text;
		text = (x & 0x04) ? "1" : "0";
		m_sIRQFraming = text;
		text = (x & 0x02) ? "1" : "0";
		m_sIRQSOF = text;
		text = (x & 0x01) ? "1" : "0";
		m_sIRQColl = text;
		UpdateData(FALSE);	/* Set screen values */
	}

	/* Read FIFO status register (1C) */
	strcpy(cmd, "12");		/* read register */
	strcat(cmd, "1C");		/* register # */
	portWrite(cmd, 0, 0);
	portRead(cmd, CMD_LEN);
	if((p = strchr(cmd, '[')) && (e = strchr(p, ']')))
	{
		p++;
		*e = 0;
		sscanf(p, "%x", &x);
		text = (x & 0x40) ? "1" : "0";
		m_sFIFOLevH = text;
		text = (x & 0x20) ? "1" : "0";
		m_sFIFOLevL = text;
		x &= 0x0F;
		text.Format("%x", x & 0x0F);
		m_sFIFOFull = text;
		UpdateData(FALSE);	/* Set screen values */
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CRFIDreadDlg::OnComPort()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int selection = m_ComPort.GetCurSel();
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(selection == -1)
	{
		MessageBox("Please, select Protocol");
		return;
	}

	comPort.Format("com%d", selection + 1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CRFIDreadDlg::topSetComNum(char *str)
{
	/*~~~~~~~~~~~~~~~~~*/
	char	newstring[5];
	/*~~~~~~~~~~~~~~~~~*/

	comPort.Format(str);
	strcpy(newstring, str);
	str += 3;

	/*~~~~*/
	int val;
	/*~~~~*/

	if(sscanf(str, "%d", &val) == 1)
	{
		val--;
		if(m_ComPort.SetCurSel(val) == CB_ERR)
		{
			m_ComPort.AddString(newstring);
			val = m_ComPort.GetCount();
			m_ComPort.SetCurSel(val - 1);
		}

		UpdateData(FALSE);	/* Set screen values */
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void topSetComNum(char *str)
{
	RRdialog->topSetComNum(str);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int topResetFlags()
{
#if SETUP_7961
	if(searchingTags) return 0;
	if(portWrite("F000", 0, 0)) return 1;

	/*~~~~~~~~~~~~~~~~~*/
	char	cmd[CMD_LEN];
	/*~~~~~~~~~~~~~~~~~*/

	portRead(cmd, CMD_LEN);
	if(portWrite("F1FF", 0, 0)) return 1;
	portRead(cmd, CMD_LEN);
#endif
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CRFIDreadDlg::topResetFlagsGUI()
{
	m_bMainChannelAM = TRUE;
	m_bAGCon = FALSE;
	UpdateData(FALSE);	/* Set screen values */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void topResetFlagsGUI()
{
	RRdialog->topResetFlagsGUI();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CRFIDreadDlg::topUpdateFlagsGUI(char *reg01)
{
	/*~~~~*/
	int val;
	/*~~~~*/

	if(sscanf(reg01, "%x", &val) != 1) return;

	/*~~~~~~~~~~~~~*/
	char	msg[256];
	/*~~~~~~~~~~~~~*/

	sprintf(msg, "'%s' --> '%x'", reg01, val);
	logAdd(msg, 0, 0);
	if(val & 0x08)
		m_bMainChannelAM = FALSE;
	else
		m_bMainChannelAM = TRUE;
	if(val & 0x04)
		m_bAGCon = TRUE;
	else
		m_bAGCon = FALSE;
	UpdateData(FALSE);	/* Set screen */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void topUpdateFlagsGUI(char *reg01)
{
	logAdd(reg01, 0, 0);
	RRdialog->topUpdateFlagsGUI(reg01);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CRFIDreadDlg::OnAGCOn()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	reply[CMD_LEN], *cmd;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	UpdateData(TRUE);	/* Get curret values from the screen */
	if(m_bAGCon)
		cmd = "F0FF";
	else
		cmd = "F000";
	portWrite(cmd, 0, 0);
	portRead(reply, CMD_LEN);
	tabRegistersRead();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CRFIDreadDlg::OnMainChannelAM()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	reply[CMD_LEN], *cmd;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	UpdateData(TRUE);	/* Get curret values from the screen */
	if(m_bMainChannelAM)
		cmd = "F1FF";
	else
		cmd = "F100";
	portWrite(cmd, 0, 0);
	portRead(reply, CMD_LEN);
	tabRegistersRead();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CRFIDreadDlg::OnEnableTRF()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	reply[CMD_LEN], *cmd;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	UpdateData(TRUE);	/* Get curret values from the screen */
	if(m_bEnableTRF)
	{
		cmd = "0300";
		topEnableTRF = 1;
	}
	else
	{
		cmd = "03FF";
		topEnableTRF = 0;
	}

	logAdd("write", 0, 0);
	portWrite(cmd, 0, 0);
	logAdd("read", 0, 0);
	portRead(reply, CMD_LEN);
	logAdd("okokok", 0, 0);

	/*~~~~~~~~~~~~~*/
	char	msg[256];
	/*~~~~~~~~~~~~~*/

	sprintf(msg, "topEnableTRF = %d", topEnableTRF);
	logAdd(msg, 0, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CRFIDreadDlg::OnButton1() //On clicking on 'Select Port' button
{
	/*~~~~~~~~*/
	CString str;
	/*~~~~~~~~*/

	m_ComPortMan.GetWindowText(str); //Get text from EditBox

	if(!strstr(str, "com") && !strstr(str, "COM"))
		MessageBox("Invalid Entry!", "Error", MB_OK);
	else
	{
		comPort.Format("\\\\.\\%s", str);	
		portFindSinglePort();	//Open the port specified
	}



}

void CRFIDreadDlg::OnClear() 
{
	// TODO: Add your control notification handler code here
	m_sLogger = " ";
	logBuf = "";
	UpdateData(FALSE);
	
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

void CRFIDreadDlg::OnAutocomport() 
{

	/*m_hUSBDevice = INVALID_HANDLE_VALUE; //Initialize 

	//////////////////////////////////////////////////////////////
	CP210x_STATUS status = CP210x_GetNumDevices(&m_dwNumDevices);
	
	//////////////////////////////////////////////////////////////
	
	UpdateData(TRUE);
	CString m_Data = "";
   // BOOL portstatus;


	//Variables to store Vid, Pid and SerialNumbers
	WORD Vid, Pid;
	CP210x_SERIAL_STRING serialNum;


	// Query the Vid, Pid and Serial number of each device.
		// We will use this information to look up the device
		// in the registry to find the port number. There are also
		// examples of displaying other items in the registry as well.

     	//Loop through and display data for each connected device
	for (int i = 0; i < m_dwNumDevices; i++)
	{

		// Open device
		CP210x_STATUS status = CP210x_Open(0, &m_hUSBDevice);
		if (status == CP210x_SUCCESS)
		{
			// Get Vid
			status = CP210x_GetDeviceVid(m_hUSBDevice, &Vid);
			if (status == CP210x_SUCCESS)
			{
				// Get Pid
				status = CP210x_GetDevicePid(m_hUSBDevice, &Pid);
				if (status == CP210x_SUCCESS)
				{
					BYTE bLength = 0;
					CString tmpDevStr;

					// Get Serial Number
					status = CP210x_GetDeviceSerialNumber(m_hUSBDevice, serialNum, &bLength);

					// Null terminate to make it a string
					serialNum[bLength] = '\0';

				
				}
			}
		}

		// This is a function call to obtain the port number
		// by passing the Vid, Pid, and SN of the device in
		// question. An integer value with the port is passed back
	    	int Port;
 			Port = GetPortNumXP2000(Vid, Pid, serialNum);
		//	Port = 10;

		char* tmpPortString;
		tmpPortString = new char[10];
		m_Data += "com";
		m_Data += itoa(Port, tmpPortString, 10);


        //Paste the comPort data into the edit box
		

		m_ComPortMan.SetWindowText(m_Data);*/

/*
   
		if (Port != -1)
		{
		
  		comPort.Format("\\\\.\\%s", m_Data);
		 portstatus = portFindSinglePort();	//Open the port specified

		}
*/		 

        	//Close the USB Device
		/*CP210x_Close(m_hUSBDevice);
		

	}

	UpdateData(FALSE);*/
}


///////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////


int CRFIDreadDlg::GetPortNumXP2000(WORD vid, WORD pid, char* ser)
{
// Variables used for Registry access
HKEY tmpKey, tmpSubKey, tmpPortKey;
CString portKeyString;
DWORD valtype;
char* portString;
DWORD length = 100;
portString = new char[101];

//Set portnum to -1, so if there is an error we will
//know by returning a negative port value

int portNum = -1;
// Open keys to get to the key where the port number is located. This key is:
// HKLM\System\CurrentControlSet\Enum\USB\Vid_xxxx&Pid_yyyy&Mi_00\zzzz_00\
// DeviceParameters\PortName

if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\",0,KEY_ALL_ACCESS,&tmpKey))
{
if (ERROR_SUCCESS == RegOpenKey(tmpKey, "Enum\\USB\\", &tmpSubKey))
{
// Loop through and replace spaces for WinXP2000
int i = 0;
while (ser[i] != '\0')
{
if (ser[i] == 0x20)
ser[i] = '_';
i++;
}
// The portkey string should look like this
// "Vid_XXXX&Pid_XXXX&MI_00\\XXXX_00" where the XXXX's are Vid, Pid
// and serial string
portKeyString.Format("Vid_%04x&Pid_%04x&Mi_00\\%s_00\\Device Parameters\\",
vid, pid, ser);
// If the portkey string is in the registry, then go ahead and open the
// portname
if (ERROR_SUCCESS == RegOpenKey(tmpSubKey, portKeyString, &tmpPortKey))
{
if (ERROR_SUCCESS == RegQueryValueEx(tmpPortKey,"PortName",NULL,&valtype,(unsigned char *)portString,&length))

{
// When we obtain this key, it will be in string format of
// "COMXX" where XX is the port. Simply make the first three
// elements of the string 0, and call the atoi function to obtain
// the number of the port.
portString[0] = '0';
portString[1] = '0';
portString[2] = '0';
portNum = atoi(portString);
}
// Make sure to close all open keys for cleanup
RegCloseKey(tmpPortKey);
}
RegCloseKey(tmpSubKey);
}
RegCloseKey(tmpKey);
}
RegCloseKey(HKEY_LOCAL_MACHINE);
// Return the number of the port the device is connected too
return portNum;
}

