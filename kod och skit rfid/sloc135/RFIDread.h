// RFIDread.h : main header file for the RFIDREAD application
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

#if !defined(AFX_RFIDREAD_H__F89127AF_4C60_4F47_95F3_0CB82A0C0785__INCLUDED_)
#define AFX_RFIDREAD_H__F89127AF_4C60_4F47_95F3_0CB82A0C0785__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

//Harsha #define KLICAJI		1 
#define KLICAJI		0 
#define SETUP_7960	1
#define SETUP_7961	1
#define SETUP_NFC	0		// set SETUP_7961 to 0
#define SETUP_NFC2	0		// set SETUP_NFC to 0
#define SETUP_IDS0	0		// set this for two protocols
#define SETUP_IDS2	0		// set this for all protocols

//#define SETUP_FELICA	0
#define SETUP_FELICA	1

#define SHOW_TEST	1
#define	NO_COM		0
#define	DIGITEST	0

#define LOG_TO_FILE	1
#define LOG_LIMIT	100
#define	CMD_LEN		2048
#define MAX_ID_LEN	100

#define	MEM_LOG		0

#if SETUP_7960
#if ! SETUP_7961
When setting SETUP_7960 set SETUP_7961 as well
#endif
#endif

#if SETUP_NFC
#if SETUP_7961
Do not set SETUP_7961 and SETUP_NFC
#endif
#if SETUP_7960
Do not set SETUP_7960 and SETUP_NFC
#endif
#endif

#if SETUP_NF2
#if ! SETUP_7960
When setting SETUP_NFC2 set SETUP_7960 as well
#endif
#endif

/////////////////////////////////////////////////////////////////////////////
// CRFIDreadApp:
// See RFIDread.cpp for the implementation of this class
//

class CRFIDreadApp : public CWinApp
{
public:
	CRFIDreadApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRFIDreadApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRFIDreadApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

typedef struct {
	char	id[MAX_ID_LEN];
	} TagID;

extern void logAddFileOnly();
extern void logAddFileScreen();
extern void logAdd(CString, int, int);
extern void hopa(CString);
extern int lookup (char *, char **);
extern int getWord (char **, char *, char *, int *, char *, int, CString *);
extern int getXWord (char **, char *);
extern BOOL portFindPort();
extern BOOL portFindSinglePort();
extern void portSetReadMode();
extern void portSetScanMode();
extern void portSetFastMode();
extern void portSetXFastMode();
extern void portSetLogging();
extern void portNoLogging();
extern int portWrite(char *, int, int);
extern int portRead(char *, int);
extern int portScan(char *, int);
extern int addHex(char *, int, int, CString, char *);
extern char * hex2rssi (char *);
extern char *hex2bin(char *);
extern void tabRegistersRead();
extern void topSetComNum(char *);
extern int topResetFlags();
extern void topResetFlagsGUI();
extern void topUpdateFlagsGUI(char *);
extern int parse15693(int, char *, char *, int, char *);
extern int find15693Tags(TagID *, int);
extern int parse14443A(int, char *, char *, int, char *);
extern int find14443ATags(TagID *, int);
extern int parse14443B(int, char *, char *, int, char *);
extern int find14443BTags(TagID *, int);
extern int parseTagit(int, char *, char *, int, char *);
extern int findTagitTags(TagID *, int);
extern int parseFeliCa(int, char *, char *, int, char *);
extern int findFeliCaTags(TagID *, int);
extern int parseEPC(int, char *, char *, int, char *);
extern int findEPCTags(TagID *, int);
extern int parseNFC(int, char *, char *, int, char *);
extern int findTagitTags(TagID *, int);
extern void NFCHideTagit();
#if SETUP_NFC || SETUP_NFC2
extern void NFCHideRAM();
extern void NFCActivateRF();
extern void NFCDeactivateRF();
#endif
extern void I14AuidRemoveBCC(char *, char *);
extern int FlcGetTagInfo(char *, char *, char *, char *);
extern void I14AResetTab();
extern void I14BResetTab();
extern void FLCResetTab();
extern void EPCResetTab();
extern void TResetTab();
extern void findtagsHideFeliCa();
extern void findtagsHideTagit();
extern void findtagsHide14443B();
extern void findtagsHide14443AAndMore();
extern void registersNFCSetup();
extern void registersRead();

extern CString  comPort;
extern int newProtocol;
extern int searchingTags;
extern BOOL expert;
#if MEM_LOG
extern void memlogAdd(char *);
#endif

#endif
