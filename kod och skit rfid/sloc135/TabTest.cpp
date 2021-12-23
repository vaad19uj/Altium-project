/*$T TabTest.cpp GC 1.140 10/11/06 11:53:05 */

/*
 * TabTest.cpp : implementation file ;
 * This file is parth of an RFID GUI. It is distributed as is ;
 * and Texas Instruments does not provide any support. ;
 * It is distributed in the hope that it will be useful, ;
 * but WITHOUT ANY WARRANTY;
 * without even the implied warranty of ;
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. Shreharsha Rao ;
 * 
 */
#include "stdafx.h"
#include "RFIDread.h"
#include "TabTest.h"
#include <fstream>
#include <windows.h>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char		THIS_FILE[] = __FILE__;
#endif
#define MAX_LINE		256
#define MAX_LABEL		128
#define MAX_GOTO		(2 * MAX_LABEL)
#define MAX_ERRLEN		512
#define INIT_TP_SIZE	2048
#define TP_STACK_SIZE	10
#define MAX_PATTERN		32
#define DIGI_PACK		0
#define MAX_O_PATTERN	(2 * MAX_PATTERN + 1)

static char		**testProgram;
static int		tpSize = 0, tpLen = 0, tpErr = 0;
static char		*modules[] = { "15693", "14443A", "14443B", "TAGIT", "FELICA", "EPC", "NFC", NULL };
static int (*parsers[]) (int, char *, char *, int, char *) =
{
	parse15693,
	parse14443A,
	parse14443B,
	parseTagit,
	parseFeliCa,
	parseEPC,
	parseNFC,
	0
};

#define CONT	0
#define ERRSTR	1
#define GOTO	2
#define INCLUDE 3
#define INFO	4
#define LABEL	5
#define MODULE	6
#define ONERROR 7
#define SEND	8
#define BERI	9
#define BERI2	10
#define DIGITST 11

static char *commands[] =
{
	"continue",
	"error",
	"goto",
	"include",
	"info",
	"label",
	"module",
	"onerror",
	"send",
	"?",
	"??",
	"digitest",
	NULL
};

typedef struct
{
	char	*label;
	int		pos;
} Charint;
Charint			labels[MAX_LABEL];
int				labelCnt = 0;

CString			comPort = "COM1";
BOOL			expert = 0;
int				logIO = 1;
int				portReadMode = 0, portReadCount = 0;	/* readMode */
int				makeDigitest(char *e, ofstream *, int, int *);

/*
 * CTabTest dialog ;
 * make it avalable to external functions:
 */
static CTabTest *RRdialog;

/*
 =======================================================================================================================
 =======================================================================================================================
 */

CTabTest::CTabTest(CWnd *pParent /* NULL */ ) :
	CDialog(CTabTest::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabTest)
	m_sTCurrentTP = _T("");
	m_sTString = _T("");
	m_sTErrors = _T("");
	m_bTExpert = FALSE;
	//}}AFX_DATA_INIT
	RRdialog = this;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CTabTest::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabTest)
	DDX_Text(pDX, IDC_T_CURR_TP, m_sTCurrentTP);
	DDX_Text(pDX, IDC_T_STRING, m_sTString);
	DDX_Text(pDX, IDC_T_DIGIERR, m_sTErrors);
	DDX_Check(pDX, IDC_T_EXEPRT, m_bTExpert);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTabTest, CDialog)
//{{AFX_MSG_MAP(CTabTest)
	ON_BN_CLICKED(IDC_T_NEW_TP, OnTNewTp)
	ON_BN_CLICKED(IDC_T_RUN_TP, OnTRunTp)
	ON_BN_CLICKED(IDC_T_SEND, OnTSend)
	ON_BN_CLICKED(IDC_T_EXEPRT, OnTExpert)
	ON_BN_CLICKED(IDC_T_DIGITEST, OnTDigitest)
	ON_BN_CLICKED(IDC_T_SEND_RAW, OnTSendRaw)
	ON_BN_CLICKED(IDC_FIRMVER, OnFirmver)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*
 =======================================================================================================================
    CTabTest message handlers
 =======================================================================================================================
 */
void CTabTest::TResetTab()
{
	UpdateData(TRUE);	/* Get curret values from the screen */
	GetDlgItem(IDC_T_DIGITEST)->ShowWindow(SW_SHOWNA);
	GetDlgItem(IDC_T_DIGIERR)->ShowWindow(SW_SHOWNA);
	UpdateData(FALSE);	/* Set screen values */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TResetTab()
{
	RRdialog->TResetTab();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int lookup(char *str, char **table)
{
	/*~~~~~~~~~~*/
	char	**t;
	int		index;
	/*~~~~~~~~~~*/

	for(index = 0, t = table; *t; t++, index++)
	{
		if(!strcmp(str, *t)) return index;
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int getWord(char **str, char *word, char *key, int *err, char *fn, int ln, CString *msg)
{
	/*~~~~~~~~~~~*/
	char	*p, *d;
	/*~~~~~~~~~~~*/

	p = *str;
	while(isspace(*p) || *p == ',') p++;
	*str = p;
	if(!*p)
	{
		msg->Format("Short '%s' (%s:%d)", key, fn, ln);
		*err++;
		return 1;
	}

	for(d = word; *p && !isspace(*p) && *p != ',';) *d++ = *p++;
	*d = 0;
	*str = p;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int maskWord(char *word, char *mask, char op)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	result[MAX_LABEL], ws[4], ms[4], rs[4], *p, *m;
	int		wi, mi, x;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* pad mask with '0' */
	ws[3] = ms[3] = result[0] = 0;
	for(p = word, m = mask; *p; m += 2, p += 2)
	{
		ws[0] = *p;
		ws[1] = *(p + 1);
		ms[0] = *p;
		ms[1] = *(p + 1);
		sscanf(ws, "%x", &wi);
		sscanf(ms, "%x", &mi);
		if(op == '|') x = wi | mi;
		if(op == '&') x = wi & mi;
		if(op == '^') x = wi ^ mi;
		sprintf(rs, "%2.2X", x);
		strcat(result, rs);
	}

	strcpy(word, result);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int getWordQuiet(char **str, char *word)
{
	/*~~~~~~~~~~~*/
	char	*p, *d;
	/*~~~~~~~~~~~*/

	p = *str;
	while(isspace(*p) || *p == ',') p++;
	if(!*p) return 1;
	for(d = word; *p && !isspace(*p) && *p != ',';) *d++ = *p++;
	*d = 0;
	*str = p;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CTabTest::OnTNewTp()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		*types = "Test Program (*.tp)|*.tp|All Files (*.*)|*.*||";
	CFileDialog m_ldFile(TRUE, "*.tp", NULL, 0, types);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * CFileDialog m_ldFile(TRUE);
	 * * Show the File open dialog and capture the result
	 */
	if(m_ldFile.DoModal() != IDOK) return;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/* Get the selected filename */
	CString cs = m_ldFile.GetPathName();
	int		i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	m_sTCurrentTP = cs;
	UpdateData(FALSE);

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/* Set screen values */

	/* Open current test program file */
	int			tpStackDepth = 0;
	ifstream	zz, *tpp;
	ifstream	tpStack[TP_STACK_SIZE];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	tpp = tpStack;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	pathStack[TP_STACK_SIZE][256], *path;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	path = pathStack[tpStackDepth];
	strcpy(path, m_sTCurrentTP.GetBuffer(0));
	tpp->open(path);
	if(tpp->fail())
	{
		MessageBox("Can not open " + m_sTCurrentTP);
		return;
	}

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int		linenoStack[TP_STACK_SIZE], lineno = 0;
	int		gotoCnt = 0, err = 0, fatal = 0, cmd, haveMask;
	char	line[MAX_LINE], reply[CMD_LEN], word[MAX_LABEL], mask[MAX_LABEL], *start, *p, *p2, *l, **t;
	Charint gotos[MAX_GOTO];
	CString msg;
	int (*moduleParser) (int, char *, char *, int, char *) = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < labelCnt; i++)
	{
		free(labels[i].label);
	}

	labelCnt = 0;
	tpLen = 0;
	t = testProgram;

	while(tpp->eof() == 0 && !fatal)
	{
more:
		tpp->getline(line, MAX_LINE, '\n');
		lineno++;
		p = line;
		while(isspace(*p)) p++;
		if(*p == '#') continue; /* comment line */
		start = p;
		if((p = strchr(start, '#'))) *p = 0;	/* inline comment */
		p = start;
		p += strlen(p) - 1;						/* remove trailing spaces */
		while(isspace(*p)) p--;
		p++;
		*p = 0;
		if(!strlen(start)) continue;			/* empty line */
		p = start;
		if(getWord(&p, word, "command", &err, path, lineno, &msg))
		{
			MessageBox(msg, "", MB_OK);
		}

		while(isspace(*p)) p++;
		cmd = lookup(word, commands);
		if(cmd == -1 && !moduleParser)
		{
			msg.Format("Invalid keyword '%s' (%s:%d)", word, path, lineno);
			MessageBox(msg, "", MB_OK);
			err++;
			continue;
		}

		switch(cmd)
		{
		case CONT:
			if(getWord(&p, word, "continue", &err, path, lineno, &msg)) MessageBox(msg, "", MB_OK);
			break;

		case ERRSTR:
			if(strlen(p) > MAX_ERRLEN)
			{
				msg.Format("Warning too long (%s:%d)", path, lineno);
				MessageBox(msg, "", MB_OK);
				err++;
			}

			if(getWord(&p, word, "error", &err, path, lineno, &msg)) MessageBox(msg, "", MB_OK);
			break;

		case GOTO:
			(void) getWord(&p, word, "goto", &err, path, lineno, &msg);
			if(labelCnt >= MAX_LABEL)
			{
				msg.Format("Too many gotos at '%s' (%s:%d)", word, path, lineno);
				MessageBox(msg, "", MB_OK);
				continue;
				err++;
			}

			gotos[gotoCnt].label = strdup(word);
			gotos[gotoCnt].pos = lineno;
			gotoCnt++;
			break;

		case INCLUDE:
			tpStackDepth++;
			if(tpStackDepth >= TP_STACK_SIZE)
			{
				msg.Format("Too many levels of include '%s' (%s:%d)", word, path, lineno);
				MessageBox(msg, "", MB_OK);
				return;
			}

			if(getWord(&p, word, "include", &err, path, lineno, &msg)) MessageBox(msg, "", MB_OK);
			strcpy(pathStack[tpStackDepth], path);
			path = pathStack[tpStackDepth];
			p2 = strrchr(path, '\\');
			p2++;
			*p2 = 0;
			strcat(path, word);
			tpp++;
			tpp->open(path);
			if(tpp->fail())
			{
				msg.Format("Can not open '%s' (%s:%d)", word, path, lineno);
				MessageBox(msg);
				return;
			}

			linenoStack[tpStackDepth - 1] = lineno;
			lineno = 0;
			continue;

		case INFO:
			if(getWord(&p, word, "info", &err, path, lineno, &msg)) MessageBox(msg, "", MB_OK);
			break;

		case LABEL:
			if(getWord(&p, word, "label", &err, path, lineno, &msg))
			{
				MessageBox(msg, "", MB_OK);
				continue;
			}

			if(labelCnt >= MAX_LABEL)
			{
				msg.Format("Too many labels at '%s' (%s:%d", word, path, lineno);
				MessageBox(msg, "", MB_OK);
				continue;
				err++;
			}

			for(i = 0; i < labelCnt; i++)
				if(!strcmp(word, labels[i].label)) break;
			if(i < labelCnt && !strcmp(word, labels[i].label))
			{
				msg.Format("Label %s redefined (%s: %d)", word, path, lineno);
				MessageBox(msg, "", MB_OK);
				err++;
			}
			else
			{
				labels[labelCnt].label = strdup(word);
				labels[labelCnt].pos = tpLen;
				labelCnt++;
			}

			continue;

		case MODULE:
			if(getWord(&p, word, "info", &err, path, lineno, &msg)) MessageBox(msg, "", MB_OK);
			cmd = lookup(word, modules);
			if(cmd == -1)
			{
				msg.Format("Invalid module '%s' (%s:%d)", word, path, lineno);
				MessageBox(msg, "", MB_OK);
				err++;
				fatal = 1;
			}

			moduleParser = parsers[cmd];
			break;

		case ONERROR:
			while(isspace(*p)) p++;
			if(!*p) break;
			if(!getWord(&p, word, "onerror", &err, path, lineno, &msg)) break;
			gotos[gotoCnt].label = strdup(word);
			gotos[gotoCnt].pos = lineno;
			gotoCnt++;
			if(strlen(word) > MAX_ERRLEN)
			{
				msg.Format("Address too long (%s:%d)", path, lineno);
				MessageBox(msg, "", MB_OK);
				err++;
			}

			while(isspace(*p)) p++;
			if(*p && strlen(p) > MAX_ERRLEN)
			{
				msg.Format("Warning too long (%s:%d)", path, lineno);
				MessageBox(msg, "", MB_OK);
				err++;
			}
			break;

		case SEND:
			if(getWord(&p, word, "send", &err, path, lineno, &msg)) MessageBox(msg, "", MB_OK);
			break;

		case BERI:
			haveMask = 0;
			if(p2 = strpbrk(p, "|&^"))
			{
				if(getWord(&p, mask, "verify mask", &err, path, lineno, &msg))
				{
					MessageBox(msg, "", MB_OK);
					break;
				}

				i = strlen(mask);
				if(i % 2)
				{
					msg.Format("Mask - expecting even number of characters (%s:%d)", path, lineno);
					MessageBox(msg, "", MB_OK);
					err++;
				}

				haveMask = 1;
				p = p2;
				p++;
			}

			if(getWord(&p, word, "verify", &err, path, lineno, &msg)) MessageBox(msg, "", MB_OK);
			if(haveMask && strlen(word) != strlen(mask))
			{
				msg.Format("Mask and target must be of the same length (%s:%d)", path, lineno);
				MessageBox(msg, "", MB_OK);
				err++;
			}
			break;

		case BERI2:
			if(getWord(&p, word, "verify", &err, path, lineno, &msg)) MessageBox(msg, "", MB_OK);
			break;

		case DIGITST:
			if(getWord(&p, word, "digitest", &err, path, lineno, &msg)) MessageBox(msg, "", MB_OK);
			break;

		default:
			moduleParser(0, line, path, lineno, reply);
		}

		if(tpLen >= tpSize)
		{
			if(tpSize == 0)
			{
				testProgram = new (char *[INIT_TP_SIZE]);
				tpSize = INIT_TP_SIZE;
				t = testProgram;
			}
			else
			{
				MessageBox("Huge test program", "", MB_OK);
				err = 1;
				break;
			}
		}

		l = strdup(start);
		strcpy(l, start);
		*t++ = l;
		tpLen++;
	}

	tpp->close();
	tpp->clear();
	if(tpStackDepth)
	{
		tpStackDepth--;
		tpp--;
		path = pathStack[tpStackDepth];
		lineno = linenoStack[tpStackDepth];
		goto more;
	}

	/*~~~~~~*/
	int ok, j;
	/*~~~~~~*/

	for(i = 0; i < gotoCnt; i++)
	{
		for(ok = 0, j = 0; j < labelCnt; j++)
		{
			if(!strcmp(gotos[i].label, labels[j].label))
			{
				ok = 1;
				break;
			}
		}

		if(!ok)
		{
			msg.Format("Missing label for goto %s (%s)", gotos[i].label, path);
			MessageBox(msg);
			err++;
		}
	}

	if(err)
	{
		msg.Format("Summary: %d erros found.", err);
		MessageBox(msg);
		for(t = testProgram, i = 0; i < tpLen; i++, t++) delete *t;
		tpLen = 0;
	}

	/*
	 * for (t = testProgram, i = 0;
	 * i < tpLen;
	 * i++, t++) ;
	 * z = *t;
	 * * MessageBox(*t);
	 */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CTabTest::OnTRunTp()
{
	/* Run test program */
	tpErr = 0;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	word[68], word2[68], mask[68], errorString[MAX_ERRLEN], onerrorLabel[MAX_ERRLEN], onerrorString[MAX_ERRLEN],
		reply[CMD_LEN], echo[CMD_LEN], gotolab[64], op, **t, *p, *p2, *rb, *re;
	int		pos, cmd, answer, more, haveMask, tooBad, i;
	int		close;
	CString msg;
	int (*moduleParser) (int, char *, char *, int, char *) = 0;

	/*~~~~~~~~~~~~*/
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ofstream	out;
	/*~~~~~~~~~~~~*/

	errorString[0] = 0;
	onerrorLabel[0] = 0;
	onerrorString[0] = 0;
	for(t = testProgram, pos = 0; pos < tpLen; pos++, t++)
	{
		p = *t;
		strcpy(echo, "... ");
		strcat(echo, p);
		logAdd(echo, 0, 0);
		while(isspace(*p)) p++;
		(void) getWordQuiet(&p, word);
		while(isspace(*p)) p++;
		cmd = lookup(word, commands);
		switch(cmd)
		{
		case CONT:
			p2 = p;
			(void) getWordQuiet(&p, word);
			for(i = 0; i < labelCnt; i++)
				if(!strcmp(word, labels[i].label)) break;
			if(i >= labelCnt) p = p2;
			if(*p)
			{
				if(i < labelCnt)
					answer = MessageBox(p, "Continue?", MB_YESNOCANCEL);
				else
					answer = MessageBox(p, "Continue?", MB_OKCANCEL);
				if(answer == IDCANCEL) return;
				if(answer == IDNO) break;
			}

			if(i < labelCnt)
			{
				pos = labels[i].pos - 1;
				t = &testProgram[pos];
			}
			break;

		case ERRSTR:
			strcpy(errorString, p);
			break;

		case GOTO:
			(void) getWordQuiet(&p, word);
			for(i = 0; i < labelCnt; i++)
				if(!strcmp(word, labels[i].label)) break;
			pos = labels[i].pos - 1;
			t = &testProgram[pos];
			break;

		case INFO:
			if(*p)
			{
				MessageBox(p, "Info", MB_OK);
			}
			break;

		case MODULE:
			getWordQuiet(&p, word);
			cmd = lookup(word, modules);
			moduleParser = parsers[cmd];
			break;

		case ONERROR:
			while(isspace(*p)) p++;
			if(!*p)
			{
				onerrorLabel[0] = 0;
				onerrorString[0] = 0;
				break;
			}

			getWordQuiet(&p, word);
			strcpy(onerrorLabel, word);
			while(isspace(*p)) p++;
			if(*p)
				strcpy(onerrorString, p);
			else
				onerrorString[0] = 0;
			break;

		case SEND:
			(void) getWordQuiet(&p, word);
			close = 0;
			if
			(
				!word[2]
			&&	word[0] == '0'
			&&	(
					word[1] == 'A'
				||	word[1] == 'B'
				||	word[1] == 'C'
				||	word[1] == 'a'
				||	word[1] == 'b'
				||	word[1] == 'c'
				)
			) close = 1;
			if(portWrite(word, close, 0)) return;
			if(!close) (void) portRead(reply, CMD_LEN);
			break;

		case BERI:
			re = reply;
			haveMask = 0;
			if(p2 = strpbrk(p, "|&^"))
			{
				haveMask = 1;
				op = *p2;
				*p2 = 0;
				if(getWordQuiet(&p, mask)) break;
				p = p2;
				p++;
			}

			tooBad = 0;
			while(!getWordQuiet(&p, word))
			{
				more = 0;

				/*
				 * 12-OCT-05 if ((rb = strchr(re, '['))) { if ((re = strchr(rb, ']'))) { more = 1;
				 * re = 0;
				 * } } if (rb && *rb) rb++;
				 * 12-OCT-05
				 */
				while((rb = strchr(re, '[')) && (re = strchr(rb, ']')) && ((re - rb) == 1)) more++ /* Nothing */ ;	/* skip
																													 * empty
																													 * braces
																													 * []
																													 * */
				if(rb && re)
				{
					rb++;
					*re++ = 0;
				}

				if(rb && haveMask)
				{
					maskWord(rb, mask, op);
				}

				if(rb && strcmp(word, rb))
				{
					msg.Format("%s ('%s' != '%s')", errorString, rb, word);
					logAdd(msg.GetBuffer(0), 0, 0);
					MessageBox(msg);
					tooBad = 1;
				}
				else if(!rb)
				{
					msg.Format("%s ('%s' expected)", errorString, word);
					logAdd(msg.GetBuffer(0), 0, 0);
					MessageBox(msg);
					tooBad = 1;
				}

				/*
				 * 12-OCT-05 if (more) re++;
				 * 12-OCT-05
				 */
			}

			if(tooBad && onerrorLabel[0])
			{
				if(onerrorString[0])
				{
					answer = MessageBox(onerrorString, "Continue?", MB_YESNOCANCEL);
					if(answer == IDCANCEL) return;
					if(answer == IDNO) break;
				}

				for(i = 0; i < labelCnt; i++)
					if(!strcmp(onerrorLabel, labels[i].label)) break;
				pos = labels[i].pos - 1;
				t = &testProgram[pos];
			}
			break;

		case BERI2:
			tooBad = 0;
			while(!getWordQuiet(&p, word))
			{
				sprintf(word2, "[%s]", word);
				if(!strstr(reply, word2))
				{
					msg.Format("%s ('%s' expected)", errorString, word);
					logAdd(msg.GetBuffer(0), 0, 0);
					MessageBox(msg);
					tooBad = 1;
				}
			}

			if(tooBad && onerrorLabel[0])
			{
				if(onerrorString[0])
				{
					sprintf(gotolab, "Go to label %s?", onerrorLabel);
					answer = MessageBox(onerrorString, gotolab, MB_YESNOCANCEL);
					if(answer == IDCANCEL) return;
					if(answer == IDNO) break;
				}

				for(i = 0; i < labelCnt; i++)
					if(!strcmp(onerrorLabel, labels[i].label)) break;
				pos = labels[i].pos - 1;
				t = &testProgram[pos];
			}
			break;

		case DIGITST:
			getWordQuiet(&p, word);
			i = 0;
			makeDigitest(word, &out, 0, &i);
			if(i)
			{
				msg.Format("%d errors in test '%s'", i, word);
				logAdd(msg.GetBuffer(0), 0, 0);
				MessageBox(msg);
			}
			break;

		default:
			if(moduleParser) moduleParser(1, *t, 0, 0, reply);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CTabTest::OnTSend()
{
	/*~~~~~~~~~~~~~~~~~*/
	char	cmd[CMD_LEN];
	/*~~~~~~~~~~~~~~~~~*/

	UpdateData(TRUE);	/* Get curret values from the screen */
	if(!m_sTString.GetLength())
	{
		MessageBox("Please, enter String");
		return;
	}

	cmd[0] = 0;

	/*
	 * strcpy(cmd, "16");
	 * // direct write
	 */
	if(addHex(cmd, CMD_LEN, 0, m_sTString, "String")) return;

	/*~~~~~~~~~~*/
	int close = 0;
	/*~~~~~~~~~~*/

	if
	(
		!cmd[2]
	&&	cmd[0] == '0'
	&&	(cmd[1] == 'A' || cmd[1] == 'B' || cmd[1] == 'C' || cmd[1] == 'a' || cmd[1] == 'b' || cmd[1] == 'c')
	) close = 1;
	if(portWrite(cmd, close, 0)) return;

	/*~~~*/
	int nn;
	/*~~~*/

	if(!close) nn = portRead(cmd, CMD_LEN);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CTabTest::OnTSendRaw()
{
	/*~~~~~~~~~~~~~~~~~*/
	char	cmd[CMD_LEN];
	/*~~~~~~~~~~~~~~~~~*/

	UpdateData(TRUE);	/* Get curret values from the screen */
	if(!m_sTString.GetLength())
	{
		MessageBox("Please, enter String");
		return;
	}

	cmd[0] = 0;

	/* if (addHex(cmd, CMD_LEN, 0, m_sTString, "String")) */
	if(!strlen(m_sTString)) return;
	strcat(cmd, m_sTString.GetBuffer(0));
	if(portWrite(cmd, 9, 0)) return;

	/*~~~*/
	int nn;
	/*~~~*/

	nn = portRead(cmd, CMD_LEN);
}

static int		portIsOpen = 0;
static HANDLE	com;

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void portSetReadMode()
{
	portReadMode = 0;
	return;
#if !NO_COM
	if(!portIsOpen) return;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COMMTIMEOUTS	commTimeouts;
	int				portReady;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	portReady = GetCommTimeouts(com, &commTimeouts);
	commTimeouts.ReadIntervalTimeout = 100;
	commTimeouts.ReadTotalTimeoutConstant = 100;
	commTimeouts.ReadTotalTimeoutMultiplier = 20;
	commTimeouts.WriteTotalTimeoutConstant = 100;
	commTimeouts.WriteTotalTimeoutMultiplier = 20;
	portReady = SetCommTimeouts(com, &commTimeouts);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void portSetFastMode()
{
	portReadMode = 1;
	return;
#if !NO_COM
	if(!portIsOpen) return;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COMMTIMEOUTS	commTimeouts;
	int				portReady;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	portReady = GetCommTimeouts(com, &commTimeouts);
	commTimeouts.ReadIntervalTimeout = 0;
	commTimeouts.ReadTotalTimeoutConstant = 6;
	commTimeouts.ReadTotalTimeoutMultiplier = 0;

	/*
	 * commTimeouts.ReadIntervalTimeout = 100;
	 * * commTimeouts.ReadTotalTimeoutConstant = 100;
	 * * commTimeouts.ReadTotalTimeoutMultiplier = 20;
	 */
	commTimeouts.WriteTotalTimeoutConstant = 100;
	commTimeouts.WriteTotalTimeoutMultiplier = 20;
	portReady = SetCommTimeouts(com, &commTimeouts);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void portSetScanMode()
{
	portReadMode = 2;
	return;
#if !NO_COM
	if(!portIsOpen) return;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COMMTIMEOUTS	commTimeouts;
	int				portReady;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	portReady = GetCommTimeouts(com, &commTimeouts);
	commTimeouts.ReadIntervalTimeout = 3;
	commTimeouts.ReadTotalTimeoutConstant = 5;
	commTimeouts.ReadTotalTimeoutMultiplier = 1;
	commTimeouts.WriteTotalTimeoutConstant = 100;
	commTimeouts.WriteTotalTimeoutMultiplier = 20;
	portReady = SetCommTimeouts(com, &commTimeouts);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void portSetXFastMode()
{
	portReadMode = 3;
	return;
#if !NO_COM
	if(!portIsOpen) return;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COMMTIMEOUTS	commTimeouts;
	int				portReady;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	portReady = GetCommTimeouts(com, &commTimeouts);
	commTimeouts.ReadIntervalTimeout = 0;
	commTimeouts.ReadTotalTimeoutConstant = 1;
	commTimeouts.ReadTotalTimeoutMultiplier = 0;

	/*
	 * commTimeouts.ReadIntervalTimeout = 1;
	 * * commTimeouts.ReadTotalTimeoutConstant = 3;
	 * * commTimeouts.ReadTotalTimeoutMultiplier = 10;
	 */
	commTimeouts.WriteTotalTimeoutConstant = 100;
	commTimeouts.WriteTotalTimeoutMultiplier = 20;
	portReady = SetCommTimeouts(com, &commTimeouts);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void portSetLogging()
{
	logIO = 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void portNoLogging()
{
	logIO = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL portFindPort()
{
	/*~~~~~~~~*/
	CString msg;
	/*~~~~~~~~*/

#if NO_COM
	return;
#endif

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char			port[20], reply[CMD_LEN];
	int				i;
	BOOL			portReady, flag = 0;
	DCB				dcb;
	COMMTIMEOUTS	commTimeouts;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 12; i >= 0; i--)
	{
		sprintf(port, "COM%d", i);
		logAdd(port, 0, 0);
		sprintf(port, "COM%d", i);
		com = CreateFile
			(
				port,
				GENERIC_READ | GENERIC_WRITE,
				0,		/* exclusive access */
				NULL,	/* no security */
				OPEN_EXISTING,
				0,		/* no overlapped I/O */
				NULL
			);			/* null template */
		if(com == INVALID_HANDLE_VALUE) continue;

		/*
		 * logAdd(" ", 0, 0);
		 */
		portReady = GetCommState(com, &dcb);
		dcb.BaudRate = 115200;
	//	dcb.BaudRate = 38400;
		dcb.ByteSize = 8;
		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT;
		dcb.fAbortOnError = TRUE;
		portReady = SetCommState(com, &dcb);

		commTimeouts.ReadIntervalTimeout = 3;
		commTimeouts.ReadTotalTimeoutConstant = 5;
		commTimeouts.ReadTotalTimeoutMultiplier = 1;
		portReady = SetCommTimeouts(com, &commTimeouts);
		portIsOpen = 1;

		/*
		 * Do a dummy write the first time to allow program ;
		 * execution in the micro to jump to the second loop ;
		 * where it scans for SPI data. A delay of 100ms is ;
		 * given to allow enough time for the micro to complete ;
		 * the first loop.
		 */
/*    	if(portDummyWrite("FF", 0, 0))
		{
			CloseHandle(com);
			continue;
		}

		Sleep(100);  */

		if(portWrite("FF", 0, 0))
		{
			CloseHandle(com);
			continue;
		}


		portRead(reply, CMD_LEN);

		if((strstr(reply, "TRF79")) )//|| *reply)
		{
			sprintf(reply, "COM%d", i);
			topSetComNum(reply);
			CloseHandle(com);
			msg = "**** COM Port found! ****";
			flag = 1;
			if(logIO)
				logAdd(msg, 0, 0);
			else
				logAdd(msg, 1, 0);
			break;
		}

		CloseHandle(com);
	}

	portIsOpen = 0;
	return flag;
}

/////////////////////////////////////////////////
BOOL portFindSinglePort()
{
	/*~~~~~~~~*/
	CString msg;
	/*~~~~~~~~*/

#if NO_COM
	return;
#endif

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char			reply[CMD_LEN];
	BOOL			portReady, flag = 0;
	DCB				dcb;
	COMMTIMEOUTS	commTimeouts;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		com = CreateFile
			(
				comPort,
				GENERIC_READ | GENERIC_WRITE,
				0,		/* exclusive access */
				NULL,	/* no security */
			    OPEN_EXISTING,
				NULL,
				0
			);			/* null template */
		if(com == INVALID_HANDLE_VALUE) 
		{
			msg = "**** Error opening COM port! ****";
			if(logIO)
				logAdd(msg, 0, 0);
			else
				logAdd(msg, 1, 0);
		}


		portReady = GetCommState(com, &dcb);
		dcb.BaudRate = 115200;
		dcb.ByteSize = 8;
		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT;
		dcb.fAbortOnError = TRUE;
		portReady = SetCommState(com, &dcb);

		commTimeouts.ReadIntervalTimeout = 100;
		commTimeouts.ReadTotalTimeoutConstant = 500;
		commTimeouts.ReadTotalTimeoutMultiplier = 20;
		portReady = SetCommTimeouts(com, &commTimeouts);
		portIsOpen = 1;

		/*
		 * Do a dummy write the first time to allow program ;
		 * execution in the micro to jump to the second loop ;
		 * where it scans for SPI data. A delay of 100ms is ;
		 * given to allow enough time for the micro to complete ;
		 * the first loop.
		 */
/*	if(portDummyWrite("FF", 0, 0))
		{
			CloseHandle(com);
			msg = "**** Error opening COM port! ****";
			if(logIO)
				logAdd(msg, 0, 0);
			else
				logAdd(msg, 1, 0);
			

		}

		Sleep(100); */

		if(portWrite("FF", 0, 0))
		{
			CloseHandle(com);
			msg = "**** Error opening COM port! ****";
			if(logIO)
				logAdd(msg, 0, 0);
			else
				logAdd(msg, 1, 0);
		
		}

		portRead(reply, CMD_LEN);

		if((strstr(reply, "TRF79")) ) // || *reply)
		{
			msg = "**** COM Port found! ****";
			flag = 1;
			if(logIO)
				logAdd(msg, 0, 0);
			else
				logAdd(msg, 1, 0);

		}
		else
		{
			msg = "**** COM Port NOT found! Check COM Port! ****";			
			if(logIO)
				logAdd(msg, 0, 0);
			else
				logAdd(msg, 1, 0);
		}

		CloseHandle(com);
	

	portIsOpen = 0;
	return flag;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static HANDLE portOpen(int mode)
{
	if(!portIsOpen)
	{
		/* Initialize serial port */
		if(strlen(comPort.GetBuffer(0)) < 3)
		{
			hopa("Select serial port");
			return 0;
		}

		logAdd(comPort, 0, 0);
		/*com = CreateFile
			(
				comPort,
				GENERIC_READ | GENERIC_WRITE,
				0,		// exclusive access 
				NULL,	// no security 
				OPEN_EXISTING,
				0,		// no overlapped I/O 
				NULL
			);			// null template 

		*/	
			

		com = CreateFile
			(
				comPort,
				GENERIC_READ | GENERIC_WRITE,
				0,		// exclusive access 
				NULL,	// no security 
				OPEN_ALWAYS,
				0,		// no overlapped I/O 
				NULL    // null template 

              );  
        

		if(com == INVALID_HANDLE_VALUE)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			CString err = "Failed to open port " + comPort ;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			hopa(err);
			return 0;
		}

		/*~~~~~~~~~~~~~~*/
		BOOL	portReady;
		DCB		dcb;
		/*~~~~~~~~~~~~~~*/

		portReady = GetCommState(com, &dcb);
		dcb.BaudRate = 115200;

		/*
		 * dcb.BaudRate = 9800;
		 */
		dcb.ByteSize = 8;
		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT;
		dcb.fAbortOnError = TRUE;
		portReady = SetCommState(com, &dcb);

		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COMMTIMEOUTS	commTimeouts;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

		portReady = GetCommTimeouts(com, &commTimeouts);

		/*
		 * commTimeouts.ReadIntervalTimeout = 50;
		 * * commTimeouts.ReadTotalTimeoutConstant = 50;
		 * * commTimeouts.ReadTotalTimeoutMultiplier = 10;
		 * * commTimeouts.WriteTotalTimeoutConstant = 50;
		 * * commTimeouts.WriteTotalTimeoutMultiplier = 10;
		 */

			/*
		 * commTimeouts.ReadIntervalTimeout = 3;
		 * commTimeouts.ReadTotalTimeoutConstant = 5;
		 * commTimeouts.ReadTotalTimeoutMultiplier = 1;
		 * ;
		 * commTimeouts.ReadIntervalTimeout = 1;
		 * * commTimeouts.ReadTotalTimeoutConstant = 10;
		 * * commTimeouts.ReadTotalTimeoutMultiplier = 1;
		 */

		commTimeouts.ReadIntervalTimeout = 100;
		commTimeouts.ReadTotalTimeoutConstant = 500;
		commTimeouts.ReadTotalTimeoutMultiplier = 20;
		commTimeouts.WriteTotalTimeoutConstant = 100;
		commTimeouts.WriteTotalTimeoutMultiplier = 20;

		/*commTimeouts.ReadIntervalTimeout = 0;
		commTimeouts.ReadTotalTimeoutConstant = 0;
		commTimeouts.ReadTotalTimeoutMultiplier = 0;
		commTimeouts.WriteTotalTimeoutConstant = 0;
		commTimeouts.WriteTotalTimeoutMultiplier = 0;*/



		if(mode)
		{
			commTimeouts.ReadIntervalTimeout = 3;
			commTimeouts.ReadTotalTimeoutConstant = 5;
			commTimeouts.ReadTotalTimeoutMultiplier = 1;
		}

		portReady = SetCommTimeouts(com, &commTimeouts);
		portIsOpen = 1;
	}

	return com;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int portWrite(char *data, int close, int binlen)
/* close = 1: close ; = 9: no header or trailer */
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	extern int	topEnableTRF;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	if(!topEnableTRF && strcmp(data, "03FF"))
	{
#if SETUP_7960
		hopa("Enable TRF7960");
#elif SETUP_7861
		hopa("Enable TRF7961");
#else
		hopa("Enable TRF circuit");
#endif
		return 1;
	}

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	sendBuf[CMD_LEN + 16], hex[8];
	int		len, lenoff;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * memlogAdd("portWrite");
	 */
	if(strlen(data) >= CMD_LEN)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~*/
		char	msg[CMD_LEN + 64];
		/*~~~~~~~~~~~~~~~~~~~~~~*/

		data[CMD_LEN] = 0;
		sprintf(msg, "Output buffer overflow at %s", data);
		hopa(msg);
		return 1;
	}

	if(close == 9)
	{
		strcpy(sendBuf, data);

		/*
		 * memlogAdd(" 1");
		 */
	}
	else
	{
		sendBuf[0] = 0;

#if KLICAJI
		strcat(sendBuf, "000");
#endif
		strcat(sendBuf, "0100000304");


		strcat(sendBuf, data);
#if KLICAJI
		strcat(sendBuf, "!!");
#else
		strcat(sendBuf, "0000");

		/*
		 * strcat(sendBuf, "1234");
		 */
#endif
		len = strlen(sendBuf);
		lenoff = 2;
#if KLICAJI
		len -= 3;
		lenoff += 3;
#endif
		len /= 2;
		len += (binlen + 1) / 2;	/* BIN data exch. */
		sprintf(hex, "%4.4X", len);
		sendBuf[lenoff] = hex[2];
		sendBuf[lenoff + 1] = hex[3];
		sendBuf[lenoff + 3] = hex[1];
	}

	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
#if NO_COM
	static ofstream out;
	static int		outOpen = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!outOpen)
	{
		out.open("rfid-reader.out");
		outOpen = 1;
	}

	out << sendBuf << endl;
#endif

	/*~~~~~~~~~~~~~~~~~*/
	/*
	 * memlogAdd(" 3");
	 */
	CString msg = "--> ";
	/*~~~~~~~~~~~~~~~~~*/

	msg = msg + sendBuf;

	/*
	 * memlogAdd(" 5");
	 */
	if(logIO)
		logAdd(msg, 0, 0);
	else
		logAdd(msg, 1, 0);
#if NO_COM
	return 0;
#endif

	/*~~~~~~~~*/
	/*
	 * memlogAdd(" 6");
	 */
	HANDLE	com;
	/*~~~~~~~~*/

	if(!(com = portOpen(0))) return 1;

	/*~~~~~~~*/
	DWORD	nn;
	int		i;
	/*~~~~~~~*/

	/*
	 * memlogAdd(" 8");
	 */
	i = WriteFile(com, sendBuf, strlen(sendBuf), &nn, NULL);

	/*
	 * memlogAdd(sendBuf);
	 * *memlogAdd(" 9");
	 */
	if(i == 0 || nn != strlen(sendBuf)) msg = "*** Write to port failed ***";
	if(!close || close == 9) return 0;

	/*
	 * memlogAdd(" 99");
	 */
	CloseHandle(com);
	portIsOpen = 0;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int portDummyWrite(char *data, int close, int binlen)
/* close = 1: close ; = 9: no header or trailer */
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	extern int	topEnableTRF;
	char		sendBuf[1];
	/*~~~~~~~~~~~~~~~~~~~~~*/

	strcpy(sendBuf, data);

	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
#if NO_COM
	static ofstream out;
	static int		outOpen = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!outOpen)
	{
		out.open("rfid-reader.out");
		outOpen = 1;
	}

	out << sendBuf << endl;
#endif

	/*~~~~~~~~~~~~~~~~~*/
	CString msg = "--> ";
	/*~~~~~~~~~~~~~~~~~*/

	msg = msg + sendBuf;

	if(logIO)
		logAdd(msg, 0, 0);
	else
		logAdd(msg, 1, 0);
#if NO_COM
	return 0;
#endif

	/*~~~~~~~~*/
	HANDLE	com;
	/*~~~~~~~~*/

	if(!(com = portOpen(0))) return 1;

	/*~~~~~~~*/
	DWORD	nn;
	int		i;
	/*~~~~~~~*/

	i = WriteFile(com, sendBuf, strlen(sendBuf), &nn, NULL);

	if(i == 0 || nn != strlen(sendBuf)) msg = "*** Write to port failed ***";
	if(!close || close == 9) return 0;

	CloseHandle(com);
	portIsOpen = 0;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int portRead(char *data, int len)
{
	portReadCount = len;

	/*~~~~~~~~~~~~~*/
	CString msg;
#if NO_COM
	char	what[12];
	/*~~~~~~~~~~~~~*/

	strncpy(what, data, 10);
	data[10] = 0;

	/*~~~~~~~~~~~~~~~~~~~~~*/
	static int	count = 0;
	/*
	 * if (count < 0) { ;
	 * strcpy(data, "--");
	 * * return 2;
	 * * }
	 */
#define IDENTITY			0
#define x15693_TP			0
#define x15693_TP2			0
#define TAGIT_GET_BLOCK		0
#define TAGIT_GET_BLOCK_SID 0
#define TAGIT_				0
#define TAGIT_VERSION		0
#define FIND				0
#define RG_READ				0
#define x14443A_UID			0
#define x14443A_RATS		0
#define x14443A_PPS			0
#define x14443A_TP			0
#define x14443B				0
#define x14443B_TP			0
#define FLC_POLLING			0
#define NFC_ATTRIBUTE		0
#define NFC_TARGET			0
#define NFC_INITIATOR		0
#define DIGIT				1
	int			ret = 14;
	/*~~~~~~~~~~~~~~~~~~~~~*/

#if LONG_REPLY
	strcpy(data, "01FF");
	strcat(data, "123456789ee0123456789ee0123456789ee01234567890aa");
	strcat(data, "123456789ee0123456789ee0123456789ee01234567890aa");
#endif
#if HIDDEN_REPLY
	strcpy(data, "pazi prihaja[3C]kr neki");
#endif
#if IDENTITY
	strcpy(data, "pazi prihaja[][0123456789ABCD01,01][,00][][][0123456789ABCD03,45]");
	strcat(data, "[][,67][][0123456789ABCD33,89][][0123456789ABCD09,ab]");
	strcat(data, "[,00][0123456789ABCD11,ef][,00][,23][,00][0123456789ABCD15,FF]");
	strcat(data, "pazi prihaja[][0123456789ABCD01,01][,00][][][0123456789ABCD03,45]");
	strcat(data, "[][,00][][0123456789ABCD33,89][][0123456789ABCD09,ab]");
	strcat(data, "[,00][0123456789ABCD11,ef][,00][,00][,45][0123456789ABCD15,FF]");
#endif
#if IDENTITY_DSFID
	strcpy(data, "pazi prihaja[0123456789ABCD01,AA,BB]kr neki");
#endif
#if x15693_TP
	if(count == 0)
	{
		strcpy
		(
			data,
			"[00][02][00][00][c2][00][00][0e][1f][11][40][00][00][00][00][00][00][00][00][00][00][00][00][00][00][00][0][00][00][00][00]"
		);
	}
	else if(count == 2)
		strcpy(data, "[A565A506000007E0]");
	else if(count == 5)
		strcpy(data, "[55443321]");
	else if(count == 6)
		strcpy(data, "[99887766]");
	else
		strcpy(data, "kr neki [FF]");
#endif
#if x15693_TP2
	if(count == 0)
	{
		strcpy(data, "[00]");
	}
	else if(count == 1)
	{
		strcpy(data, "[00]");
	}
	else if(count == 2)
	{
		strcpy(data, "[00]");
	}
	else if(count == 3)
	{
		strcpy(data, "[][][][][][A565A506000007E0][][][][][][][][][][]");
	}
	else if(count == 4)
	{
		strcpy(data, "[00]");
	}
	else if(count == 5)
	{
		strcpy(data, "[][][][][][50061F643120381933002185][][][][][][][][][][]");
	}
#endif
#if TAGIT_GET_BLOCK
	strcpy(data, "v redu[x00][0200112233445566]");
#endif
#if TAGIT_GET_BLOCK_SID
	strcpy(data, "v redu[AABBCCDD020011223344]");
#endif
#if TAGIT_VERSION
	strcpy(data, "v redu[C030009A4C8202020307]");
#endif
#if TAGIT_SID_POLL
	strcpy(data, "v redu[FFEEDDCC02050307][][][3344556607050307][][][6677889902050103][][][][][][][][]");
#endif
#if TAGIT_TP
	if(count == 0)
		strcpy(data, "v redu[FFEEDDCC02050307][][][3344556607050307][][][][][][][][][][][]");
	else if(count == 4)
		strcpy(data, "[01223344]");
	else if(count == 5)
		strcpy(data, "[55667788]");
	else
		sprintf(data, "kr neki [FF%d]", count);

	/*~~~~~~~~~~~~~~~~~*/
#endif
#if FIND
	static int	pass = 0;
	/*~~~~~~~~~~~~~~~~~*/

	if(!strcmp(what, "_15693_"))
	{
		if(pass == 0)
		{
			strcpy(data, "pazi prihaja 15963[CCCCCCCCCCCCCCCC,00][0123456789ABCD01,11][][0123456789ABCD03,33]");
			strcat(data, "[][][][0123456789ABCD33,66][][0123456789ABCD09,99]");
			strcat(data, "[][0123456789ABCD11,BB][][][][0123456789ABCD15,FF]");
		}
		else
		{
			strcpy(data, "pazi prihaja 15963[CCCCCCCCCCCCCCCC,00][0123456789ABCD01,11][][]");
			strcat(data, "[][][][0123456789ABCD33,66][][0123456789ABCD09,99]");
			strcat(data, "[][0123456789ABCD11,BB][][][][0123456789ABCD15,FF]");
		}
	}

#if SETUP_7960
#endif
	else if(!strcmp(what, "_14443A_"))
	{
		if(pass == 0)
		{
			strcpy(data, "mamo 14443A [001122AA33445566BB]<garbage>[CCCCCCCCCCCCCCCCCCCCCCCCCC]se neki[33221100AA]");
		}
		else
		{
			strcpy
			(
				data,
				"mamo 14443A [001122AA33445566BB]<garbage>[CCCCCCCCCCCCCCCCCCCCCCCCCC]se neki[33221100AA][0123456789ABCDEF0123456789]"
			);
		}
	}
	else if(!strcmp(what, "_14443B_"))
	{
		if(pass == 0)
			strcpy(data, "gre 14443B [5042C862E100000000002185][00][5000024E3800000000000041]");
		else
			strcpy(data, "gre 14443B [5042C862E100000000002185][00][5000024E3800000000000041]");
	}
	else if(!strcmp(what, "_Tagit_"))
	{
		if(pass == 0)
		{
			strcpy
			(
				data,
				"v redu TAGIT[FFEEDDCC02050307][][][3344556607050307][][][6677889902050103][FFEECCCCCCCC0307][][][][][][][]"
			);
		}
		else
		{
			strcpy(data, "v redu TAGIT[FFEEDDCC02050307][][][][][][6677889902050103][FFEECCCCCCCC0307][][][][][][][]");
		}
	}
	else if(!strcmp(what, "_FeliCa_"))
	{
		if(pass == 0)
		{
			strcpy(data, "se FeliCa [CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC][0123456789ABCDEF0123456789ABCDEF0123]");
		}
		else
		{
			strcpy
			(
				data,
				"se FeliCa [CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC][0123456789ABCDEF0123456789ABCDEF0123][abcdefabcdefabcdefabcdefabcdefabcdef]"
			);
		}
	}
	else if(!strcmp(what, "_EPC_"))
	{
		if(pass == 0)
			strcpy(data, "in EPC [CCCCCCCCCCCCCCCCCCCCCCCC][00][5000024E3800000000000041]");
		else
			strcpy(data, "in EPC [CCCCCCCCCCCCCCCCCCCCCCCC][00][5000024E3800000000000041]");
		pass = (pass) ? 0 : 1;
	}
#endif
#if RG_READ
	/*
	 * strcpy(data, "pazi prihaja[00][01][02][03][04][05][06][07][08][09]");
	 * * strcat(data, "[10][11][12][13][14][15][16][17][18][19]");
	 * * strcat(data, "[20][21][22][23][24][25][26][27][28][29][30]");
	 */
	strcpy(data, "[1][2][0][0][c2][0][0][e][1f][11][40][87][0][3e][0][0]");
	strcat(data, "[0][0][0][0][0][0][0][0][0][0][0][0][0][0][0]");
#endif
#if x14443A_UID
	strcpy(data, "smeti[001122AA33445566BB]<garbage>[001122AA334455BB66778899CC]se neki[33221100AA]");
#endif
#if x14443A_RATS
	strcpy(data, "smeti[001122AA33445566BB]<garbage>[001122AA334455BB66778899CC]se neki[33221100AA]");
	if(count == 5)
		strcpy(data, "smeti[020F] no TA,TB,TC,history");
	else if(count == 6)
		strcpy(data, "smeti [025F7203AABB] TA,TC,history");
	else if(count == 7)
	{
		strcpy(data, "smeti [02107203AABB] TB,history");
		count = -1;
	}
#endif
#if x14443A_PPS
	strcpy(data, "smeti[5042C862E10000]<garbage>[000024E380000000009]se neki[5042C862]");
#endif
#if x14443A_TP
	if(count == 0)
	{
		strcpy(data, "[00]");
	}
	else if(count == 1)
	{
		strcpy(data, "[00]");
	}
	else if(count == 2)
	{
		strcpy(data, "[00]");
	}
	else if(count == 3)
	{
		strcpy(data, "[][][][][][50061F643120381933002185][][][][][][][][][][]");
	}
	else if(count == 4)
	{
		strcpy(data, "[00]");
	}
	else if(count == 5)
	{
		strcpy(data, "[][][][][][50061F643120381933002185][][][][][][][][][][]");
	}
#endif
#if x14443B
	strcpy(data, "[5042C862E100000000002185][5000024E3800000000000041][5042C862E099000099992703]");
#endif
#if x14443B_TP
	if(count == 0)
	{
		strcpy(data, "[00]");
	}
	else if(count == 1)
	{
		strcpy(data, "[00]");
	}
	else if(count == 2)
	{
		strcpy(data, "[00]");
	}
	else if(count == 3)
	{
		strcpy(data, "[][][][][][50061F643120381933002185][][][][][][][][][][]");
	}
	else if(count == 4)
	{
		strcpy(data, "[00]");
	}
	else if(count == 5)
	{
		strcpy(data, "[][][][][][50061F643120381933002185][][][][][][][][][][]");
	}
#endif
#if FLC_POLLING
	strcpy(data, "se FeliCa gre [112233445566778811223344556677881122] in [0123456789ABCDEF0123456789ABCDEF0123]");
#endif
#if NFC_ATTRIBUTE
	strcpy(data, "[vstavi neki]");
#endif
#if NFC_TARGET
	if(count == 2)
		strcpy(data, "Porka flek");
	else if(count == 3)
		strcpy(data, "To ni res");
	else if(count == 4)
		strcpy(data, "<<FILE>>zzz.z3");
	else if(count == 5)
		strcpy(data, "8B");
	else if(count == 6)
		strcpy(data, "prva vrsta\n");
	else if(count == 7)
		strcpy(data, "8C");
	else if(count == 8)
		strcpy(data, "druga vrsta\n");
	else if(count == 9)
		strcpy(data, "0D");
	else if(count == 10)
		strcpy(data, "tretja vrsta\n");
	ret = strlen(data);
#endif
#if NFC_INITIATOR
	strcpy(data, "[00]");
	ret = strlen(data);
#endif
#if DIGIT
	if(!(count % 10))	/* strcpy(data,
						 * "AA");
						 * */
		data[1] = 'A';
	ret = strlen(data);
#endif
	count++;
	msg = "<-- ";
	msg = msg + data;
	if(logIO)
		logAdd(msg, 0, 0);
	else
		logAdd(msg, 1, 0);
	return ret;
#endif

	/*~~~~~~~~*/
	HANDLE	com;
	/*~~~~~~~~*/

	if(!(com = portOpen(0)))
	{
		*data = 0;
		return 0;
	}

	/*~~~~~~~*/
	DWORD	nn;
	int		i;
	/*~~~~~~~*/



	i = ReadFile(com, data, len, &nn, NULL);

	/*
	 * if (i == 0) { int err = GetLastError();
	 * LPVOID lpMsgBuf;
	 * if (! FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
	 * FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
	 * GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
	 * (LPTSTR) &lpMsgBuf, 0, NULL )) err = 0;
	 * char errmsg[256];
	 * sprintf(errmsg, "%s", lpMsgBuf);
	 * LocalFree( lpMsgBuf );
	 * portIsOpen = 0;
	 * CloseHandle(com);
	 * portRead(data, len);
	 * }
	 */
	if(i == 0) nn = 0;
	if(nn == 0)
	{
		*data = 0;
		msg = "*** Read TIMEOUT ***";
	}
	else
	{ 
		if(nn > len) nn = len;
		data[nn] = 0;
		msg = "<-- ";
		msg = msg + data;
	}

	if(logIO)
		logAdd(msg, 0, 0);
	else
		logAdd(msg, 1, 0);
	
	return nn;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int portScan(char *data, int len)
{
	/*~~~~~~~~~~~~~~~~~~*/
#if NO_COM
	static int	count = 0;
	/*~~~~~~~~~~~~~~~~~~*/

	data[0] = 0;
	if(count == 5)
		strcpy(data, "0A");
	else if(count == 100)
		strcpy(data, "09");
	else if(count == 200)
		strcpy(data, "8E");
	count++;
	return strlen(data);
#endif

	/*~~~~~~~~*/
	CString msg;
	HANDLE	com;
	/*~~~~~~~~*/

	if(!(com = portOpen(1)))
	{
		*data = 0;
		return -1;
	}

	/*~~~~~~~*/
	DWORD	nn;
	int		i;
	/*~~~~~~~*/

	i = ReadFile(com, data, len, &nn, NULL);
	if(i == 0) nn = 0;
	if(nn == 0)
	{
		*data = 0;

		/*
		 * msg = " Scan TIMEOUT ";
		 */
	}
	else
	{
		if(nn > len) nn = len;
		data[nn] = 0;
		msg = "<.- ";
		msg = msg + data;
		if(logIO)
			logAdd(msg, 0, 0);
		else
			logAdd(msg, 1, 0);
	}

	return nn;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CTabTest::OnTExpert()
{
	UpdateData(TRUE);	/* Get curret values from the screen */
	expert = m_bTExpert;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int cmpMask(char *ref, char *reply, char *patternMask, char *outPattern, int size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	refB[128], replyB[128], maskB[128], *p, *r, *o, *m;
	int		good = 1, i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	strcpy(refB, hex2bin(ref));
	strcpy(replyB, hex2bin(reply));
	strcpy(maskB, hex2bin(patternMask));
	refB[size] = 0;
	for(i = 0, m = maskB, o = outPattern, r = replyB, p = refB; *p; p++, r++, o += 2, m++, i++)
	{
		if(*p != *r)
		{
			*o = '1';
			if(*m != '1')
			{
				good = 0;
				*(o + 1) = '1';
			}
		}
	}

	if(good) return 0;
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int cmpTol
(
	char	*ref,
	char	*reply,
	char	*outPattern,
	char	**currentPattern,
	int		patternNo,
	int		patternCnt,
	int		tolerances,
	int		*tol,
	int		size
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	refB[128], replyB[128], refBx[128], *p, *r, *o, **cp;
	int		beg, end, badBit, good = 1, i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	strcpy(refB, hex2bin(ref));
	strcpy(replyB, hex2bin(reply));
	refB[size] = 0;
	for(i = 0, o = outPattern, r = replyB, p = refB; *p; p++, r++, o += 2, i++)
	{
		if(*p != *r)
		{
			*o = '1';
			if(!tolerances)
			{
				good = 0;
				*(o + 1) = '1';
				continue;
			}

			beg = patternNo - tol[2 * i];
			if(beg < 0)
				beg = 0;
			else
				beg = -tol[2 * i];
			end = patternNo + tol[2 * i + 1] - patternCnt;
			if(end < 0) end = tol[2 * i + 1];
			for(badBit = 1, j = beg; j <= end; j++)
			{
				cp = currentPattern + j;
				strcpy(refBx, hex2bin(*cp));
				if(refBx[i] == *r)
				{
					badBit = 0;
					break;
				}
			}

			if(badBit)
			{
				good = 0;
				*(o + 1) = '1';
			}
		}
	}

	if(good) return 0;
	return 1;
}

#define TOL_INIT	- 98765

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int decodeTolerances(char *p, int *tol, int size, char **signals, int lineno)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/*
	 * Tolerance syntax example 2 OUT X3 3:4 data 1 ;
	 * OUT, X3 may change 2 clocks before and 2 clocks after reference ;
	 * data may change 3 clocks before and 4 clocks after reference ;
	 * all other signals may change ;
	 * 1 clocks before and 1 clocks after reference
	 */
	char	msg[1024], signal[64], *d, **s;
	int		before, after, i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < size; i++) tol[2 * i] = tol[2 * i + 1] = TOL_INIT;
	while(isspace(*p)) p++;
	while(*p)
	{
		if(!isdigit(*p))
		{
			sprintf(msg, "Digit expected at '%s'", p);
			hopa(msg);
			return 1;
		}

		before = atoi(p);
		while(isdigit(*p)) p++;
		after = before;
		if(*p == ':')
		{
			p++;
			if(!isdigit(*p))
			{
				sprintf(msg, "Digit expected at '%s'", p);
				hopa(msg);
				return 1;
			}

			after = atoi(p);
			while(isdigit(*p)) p++;
		}
		else if(*p && !isspace(*p))
		{
			sprintf(msg, "Space expected at '%s'", p);
			hopa(msg);
			return 1;
		}

		while(isspace(*p)) p++;
		if(isdigit(*p))
		{
			sprintf(msg, "Signal name expected at '%s'", p);
			hopa(msg);
			return 1;
		}

		if(!*p)
		{
			/* set all initilized tolerances */
			for(i = 0; i < size; i++)
			{
				if(tol[2 * i] == TOL_INIT) tol[2 * i] = before;
				if(tol[2 * i + 1] == TOL_INIT) tol[2 * i + 1] = after;
			}
		}

		while(*p && !isdigit(*p))
		{
			for(d = signal; *p && *p != ' ';) *d++ = *p++;
			*d = 0;
			for(s = signals, i = 0; i < size; i++, s++)
				if(!strcmp(signal, *s)) break;
			if(i >= size)
			{
				sprintf(msg, "Unexpected tolerance signal '%s'", signal);
				hopa(msg);
				return 1;
			}

			tol[2 * i] = before;
			tol[2 * i + 1] = after;
			while(isspace(*p)) p++;
		}
	}

	/* set all initilized tolerances */
	for(i = 0; i < size; i++)
	{
		if(tol[2 * i] == TOL_INIT) tol[2 * i] = 0;
		if(tol[2 * i + 1] == TOL_INIT) tol[2 * i + 1] = 0;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int makeDigitest(char *fname, ofstream *out, int open, int *errcnt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/* Open current test program file */
	int		status = 0, errors = 0, wasOpen = open, len = CMD_LEN, pack = 0, sigCnt = 0, tolerances = 0,
		haveTimestep = 0;
	double	timestep = 1, tt;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if DIGI_PACK
	pack = 1;
#endif

	/*~~~~~~~~~~~~~*/
	ofstream	zz;
	ifstream	tpat;
	/*~~~~~~~~~~~~~*/

	tpat.open(fname);

	/*~~~~~~~~~~~~~~~~~~*/
	char	msg[MAX_LINE];
	/*~~~~~~~~~~~~~~~~~~*/

	if(tpat.fail())
	{
		sprintf(msg, "Can not open %s", fname);
		logAdd(msg, 0, 0);
		errors++;
		goto done;
	}

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		line[MAX_LINE], reply[CMD_LEN], mask[CMD_LEN], title[MAX_LINE], packet[MAX_LINE], pp[MAX_LINE], rr[
		MAX_LINE], outPattern[MAX_PATTERN], patternMask[MAX_LINE], IOMask[MAX_LINE], *sigNames[MAX_PATTERN], **
			patterns, **patternsOut, *patternErr, *patternDiff, **patternsMask, **patIOMask, *start, *p, *r, *pm, *dp, *
				dr, *t, *p0, **p2, **p2o, *pe, *pd, **p2m, **p2io;
	int			tol[MAX_PATTERN][2], outputs[MAX_PATTERN], lineno, body, packetLen, dataLen, single, dots, patternSize,
		patternCnt, patternNo, tooBad, i, j;
	unsigned	memsize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	patternSize = 8192;
	memsize = patternSize * sizeof(char *);
	if((patterns = (char **) malloc(memsize)) == (char **) NULL)
	{
		hopa("Out of memory (pat0)");
		exit(1);
	}

	for(i = 0; i < MAX_PATTERN; i++) outputs[i] = 0;

	/* Load the entire pattern */
	if(!wasOpen)
	{
		strcpy(line, fname);
		strcat(line, ".out");
		out->open(line);
		open = 1;
	}

	patternCnt = 0;
	p2 = patterns;
	lineno = 0;
	while(tpat.eof() == 0)
	{
		tpat.getline(line, MAX_LINE, '\n');
		lineno++;
		p = line;
		if(*p == '.')
		{
			/* save outputs */
			strcpy(reply, hex2bin(p + 1));
			for(i = 0, r = reply; *r; r++, i++)
				if(*r == '0') outputs[i] = 1;
		}

		if(*p == ',') continue;

		/* skip comments, IO mask, checking windows */
		if(!(*p >= '0' && *p <= '9') && !(*p >= 'A' && *p <= 'F')) continue;
		start = p;
		if((p = strchr(start, '#'))) *p = 0;	/* inline comment */
		p = start;
		p += strlen(p) - 1;						/* remove trailing spaces */
		while(isspace(*p)) p--;
		p++;
		*p = 0;
		p = start;
		if(!patternCnt || patternCnt >= patternSize)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			char	**tmppat = patterns, **t2;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			/* get 4 times what was not sufficient */
			patternSize *= 4;
			memsize = patternSize * sizeof(char *);
			if((patterns = (char **) malloc(memsize)) == (char **) NULL)
			{
				hopa("Out of memory (pat0)");
				exit(1);
			}

			for(p2 = patterns, t2 = tmppat, i = 0; i < patternCnt; i++) *p2++ = *t2++;
			if(patternCnt)
			{
				free(tmppat);
			}
		}

		*p2++ = strdup(start);
		patternCnt++;
	}

	tpat.close();
	tpat.clear();
	tpat.open(fname);

	/* get memory for output patterns */
	memsize = patternCnt * sizeof(char *);
	if((patternsOut = (char **) malloc(memsize)) == (char **) NULL)
	{
		hopa("Out of memory (pat1)");
		exit(1);
	}

	if((patternDiff = (char *) malloc(patternCnt + 1)) == (char *) NULL)
	{
		hopa("Out of memory (pat2)");
		exit(1);
	}

	if((patternErr = (char *) malloc(patternCnt + 1)) == (char *) NULL)
	{
		hopa("Out of memory (pat3)");
		exit(1);
	}

	if((patternsMask = (char **) malloc(memsize)) == (char **) NULL)
	{
		hopa("Out of memory (pat1)");
		exit(1);
	}

	if((patIOMask = (char **) malloc(memsize)) == (char **) NULL)
	{
		hopa("Out of memory (pat1)");
		exit(1);
	}

	/*
	 * for (p2 = patterns, i = 0;
	 * i < patternCnt;
	 * i++, p2++) { zzx[i] = *(p2-1);
	 * } strcpy(line, fname);
	 * strcat(line, ".zz");
	 * zz.open(line);
	 * for (p2 = patterns, i = 0;
	 * i < patternCnt;
	 * i++, p2++) { zz << i << ": " << *p2 << endl;
	 * //sprintf(msg, "%d: %s", i, *p2);
	 * //logAdd(msg, 0, 0);
	 * zz.flush();
	 * } zz.close();
	 * ;
	 * check all patterns ;
	 * *errcnt = 0;
	 */
	packet[0] = 0;
	title[0] = 0;
	p2 = patterns;
	p2o = patternsOut;
	pd = patternDiff;
	pe = patternErr;
	p2m = patternsMask;
	p2io = patIOMask;
	strcpy(patternMask, "00000000000000000000000000000000");
	if(patternCnt) patternMask[strlen(*p2) - 1] = 0;
	strcpy(IOMask, "00000000000000000000000000000000");
	if(patternCnt) IOMask[strlen(*p2) - 1] = 0;
	patternNo = 0;
	lineno = 0, body = 0;
	while(tpat.eof() == 0)
	{
		tpat.getline(line, MAX_LINE, '\n');
		lineno++;
		p = line;
		while(isspace(*p)) p++;
		if(*p == '#')
		{
			if(*(p + 1) == '>' && *(p + 2) == '>')
			{
				p += 3;
				if(!strncmp(p, "Title:", 6))
				{
					p += 6;
					while(isspace(*p)) p++;
					strcpy(title, p);
				}
				else if(!strncmp(p, "Tolerances:", 11))
				{
					p += 11;
					if(!sigCnt)
					{
						hopa("Signal names not yet defined.\nPlace Tolerances line after the signal names line.");
						return 1;
					}

					while(isspace(*p)) p++;
					if(decodeTolerances(p, (int *) tol, sigCnt, sigNames, lineno))
					{
						hopa("Bad tolerances syntax.");
						return 1;
					}

					tolerances = 1;
				}
				else if(!strncmp(p, "Timestep:", 9))
				{
					if(patternNo)
					{
						hopa("Timestep must preceed patterns.");
						return 1;
					}

					p += 9;
					while(isspace(*p)) p++;
					if(haveTimestep)
					{
						hopa("Invalid timestep redefinition.");
						return 1;
					}

					timestep = atof(p);
					timestep *= 1e-9;
					haveTimestep = 1;
				}
				else
				{
					sprintf(msg, "Invalid #>> keyword at '%s'.", p);
					hopa(msg);
					return 1;
				}
			}

			*out << line << endl;
			continue;	/* comment line */
		}

		start = p;
		if((p = strchr(start, '#'))) *p = 0;	/* inline comment */
		p = start;
		p += strlen(p) - 1;						/* remove trailing spaces */
		while(isspace(*p)) p--;
		p++;
		*p = 0;
		if(!strlen(start)) continue;			/* empty line */
		p = line;
		if(*p == ',') strcpy(patternMask, p + 1);
		if(*p == ':' || body) body++;
		if(!strncmp(p, ">include", 8))
		{
			if(body)
			{
				*out << "\t'" << fname << "' Invalid include inside body." << endl;
				errors++;
				goto done;
			}

			while(!isspace(*p)) p++;
			while(isspace(*p)) p++;
			if(!*p)
			{
				*out << "\t'" << fname << "' Invalid missing file name." << endl;
				errors++;
				goto done;
			}

			*out << "<<<<<<<<<<<<< " << p << endl;
			if(makeDigitest(p, out, open, errcnt)) goto done;
			continue;
		}

		if(body == 1 && *p != ':')
		{
			*out << "\t'" << fname << "' Body must start with a '.' line." << endl;
			errors++;
			goto done;
		}

		if(body == 2 && *p != '.')
		{
			*out << "\t'" << fname << "' Missing pin types." << endl;
			errors++;
			goto done;
		}

		single = 0;
		if(*p == ':')
		{
			portWrite("CC", 0, 0);
			portRead(reply, CMD_LEN);
			portNoLogging();
			portSetXFastMode();
			len = CMD_LEN;

			/*
			 * len = 1;
			 */
			single = 1;
			p0 = p;
			p++;
			while(*p)
			{
				for(r = reply; *p && *p != ',';)
				{
					if(*p == ';')
						p++;
					else
						*r++ = *p++;
				}

				*r = 0;
				sigNames[sigCnt++] = strdup(reply);
				if(*p) p++;
			}

			p = p0;
		}
		else if(*p == '.')
		{
			len = strlen(p) - 1;
			strcpy(IOMask, p + 1);
		}
		else if(*p == ',')
		{
			*out << p << endl;
			continue;
		}
		else if(*p == '$')
		{
			len = CMD_LEN;
			single = 1;
		}
		else
			dataLen = strlen(p);
		if(pack)
		{
			if(*p != ':' && *p != '$')
			{
				strcat(packet, p);
				packetLen++;
			}

			if(packetLen >= DIGI_PACK || single)
			{
				for(dots = 0, p = packet; *p; p++)
					if(*p == '.') dots++;
				len = strlen(packet) - dots;
				portWrite(packet, 9, 0);
				portRead(reply, len);
				*out << packet << endl;
				*out << reply << endl;
				p = packet, r = reply;
				for(i = 0; i < packetLen; i++)
				{
					p0 = p;
					dp = pp, dr = rr;
					for(j = 0; j < dataLen; j++)
					{
						*dp++ = *p++;
					}

					*dp = 0;
					for(j = 0; j < dataLen; j++)
					{
						*dr++ = *r++;
					}

					*dr = 0;
					*out << pp << endl;
					if(*p0 != '.' && strcmp(pp, rr))
					{
						t = (title[0]) ? title : fname;
						*out << "  " << rr << "\t(" << lineno << ") ***** Invalid reply." << endl;
						errors++;
					}
				}

				packet[0] = 0;
				packetLen = 0;
			}

			p = line;
		}

		if(!pack || single)
		{
			portWrite(p, 9, 0);
#if NO_COM
			strcpy(reply, p);
#endif
			if(*p == ':') Sleep(200);
			portRead(reply, len);

			/* everything is OK */
			if(*p != ':' && *p != '.' && *p != '$')
				for(i = 0; i < MAX_O_PATTERN - 1; i++) outPattern[i] = '0';
			outPattern[2 * sigCnt] = 0;
			if(*p != ':' && *p != '$')
			{
				*out << p;
				if(*p == '.' && timestep) *out << "  (" << patternNo * timestep << ')';

				/*
				 * *out << endl;
				 */
				*pd = '0';
				*pe = '0';
				if(*p != '.' && strcmp(p, reply))
				{
					*pd = '1';
#if 0
					if(p != *p2) *out << "???" << " " << *p2;
#endif
					*out << " ???";

					/*
					 * if (patternNo > 11052) ;
					 * j = 0;
					 */
					tooBad = cmpMask(p, reply, patternMask, outPattern, sigCnt);
					if(tolerances && tooBad)
						tooBad = cmpTol
							(
								p,
								reply,
								outPattern,
								p2,
								patternNo,
								patternCnt,
								tolerances,
								(int *) tol,
								sigCnt
							);
					if(tooBad)
					{
						*pe = '1';
						t = (title[0]) ? title : fname;
						*out << "  " << reply << "\t(line=" << lineno << " #" << patternNo;
						if(timestep)
						{
							tt = patternNo * timestep;
							*out << " " << tt;
						}

						*out << ") ***** Invalid reply." << " " << outPattern;
						errors++;
					}
				}

				*out << endl;
			}

			/*
			 * if (*p != ':' && *p != '.' && *p != '$') { p2o++ = strdup(outPattern);
			 * p2m++ = strdup(patternMask);
			 * }
			 */
			if(*p != ':' && *p != '.')
			{
				p2++;
				*p2o++ = strdup(outPattern);
				pd++;
				pe++;
				*p2m++ = strdup(patternMask);
				*p2io++ = strdup(IOMask);
				patternNo++;
				if(!(patternNo % 5000))
				{
					sprintf(msg, "%d", patternNo);
					logAdd(msg, 0, 0);
				}
			}
		}

		if(*p == '$')
		{
			body = 0;
			portSetReadMode();
			portSetLogging();
		}
	}

done:
	*pe = 0;
	tpat.close();
	*out << ".............................................." << endl;
	*out << fname << ": " << errors << " errors." << endl;
	*errcnt += errors;
	if(!wasOpen)
	{
		*out << "----------------------------------------------" << endl;
		*out << "Total of " << *errcnt << " errors." << endl;
		out->close();
	}

	if(timestep)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		char	pline[2048], pline2[2048], pval[4];
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pval[1] = ' ';
		pval[2] = 0;

		/* Write sigtool plot file */
		strcpy(line, fname);
		strcat(line, ".sig");

		/*~~~~~~~~~~~~*/
		ofstream	sig;
		/*~~~~~~~~~~~~*/

		sig.open(line);
		sig << "# Signal file" << '\n';
		sig << "TIME ";
		for(i = 0; i < sigCnt; i++)
		{
			sig << sigNames[i] << " ";
			if(outputs[i]) sig << sigNames[i] << "_d " << sigNames[i] << "_e ";
		}

		for(i = 0; i < sigCnt; i++) sig << "IO." << sigNames[i] << " ";
		sig << "_diff_ _err_" << '\n';
		for
		(
			p2 = patterns, p2o = patternsOut, pd = patternDiff, pe = patternErr, p2m = patternsMask, p2io = patIOMask, i = 0;
			i < patternCnt;
			i++, p2++, p2o++, pd++, pe++, p2m++, p2io++
		)
		{
			strcpy(line, hex2bin(*p2));
			strcpy(reply, *p2o);
			strcpy(mask, hex2bin(*p2m));

			/*
			 * if (i > 9520) ;
			 * j = 0;
			 */
			pline[0] = 0;
			for(j = 0, p = line, r = reply, pm = mask; j < sigCnt; j++, p++, r += 2, pm++)
			{
				if(*pm == '1')
					strcat(pline, "0.5 ");
				else
				{
					pval[0] = *p;
					strcat(pline, pval);
				}

				if(outputs[j])
				{
					pval[0] = *r;
					strcat(pline, pval);
					pval[0] = *(r + 1);
					strcat(pline, pval);
				}
			}

			strcpy(line, hex2bin(*p2io));
			for(j = 0, p = line; j < sigCnt; j++, p++)
			{
				pval[0] = *p;
				strcat(pline, pval);
			}

			pval[0] = *pd;
			strcat(pline, pval);
			pval[0] = *pe;
			strcat(pline, pval);
			if(i && strcmp(pline, pline2))
			{
				tt = i * timestep;
				tt -= timestep * 0.01;
				sig << tt << " " << pline2 << '\n';
			}

			tt = i * timestep;
			sig << tt << " " << pline << '\n';
			strcpy(pline2, pline);
			free(*p2);
			free(*p2o);
		}

		sig.close();
	}

	free(patterns);

	/*
	 * free(patternsOut);
	 */
	free(patternDiff);

	/*
	 * free(patternErr);
	 * * free(patternsMask);
	 */
	for(i = 0; i < sigCnt; i++) free(sigNames[i]);

	sprintf(msg, "DONE (%s)", fname);
	logAdd(msg, 0, 0);
	return status;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CTabTest::OnTDigitest()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		*types = "Test Program (*.tpat)|*.tpat|All Files (*.*)|*.*||";
	CFileDialog m_ldFile(TRUE, "*.tpat", NULL, 0, types);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Show the File open dialog and capture the result */
	if(m_ldFile.DoModal() != IDOK) return;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/* Get the selected filename */
	CString cs = m_ldFile.GetPathName();
	char	*path;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	path = cs.GetBuffer(0);

	/*~~~~~~~~~~~~~~~~~~~*/
	ofstream	out;
	int			errcnt = 0;
	/*~~~~~~~~~~~~~~~~~~~*/

	makeDigitest(path, &out, 0, &errcnt);
	m_sTErrors.Format("%d", errcnt);
	UpdateData(FALSE);	/* Set screen values */
}

void CTabTest::OnFirmver() 
{
	char	cmd[CMD_LEN] = "FE";

	
	if(portWrite(cmd, 0 , 0)) return;

	/*~~~*/
	int nn;
	/*~~~*/

	nn = portRead(cmd, CMD_LEN);
	
}
