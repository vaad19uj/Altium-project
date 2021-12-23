// RFIDreadDoc.cpp : implementation of the CRFIDreadDoc class
//

#include "stdafx.h"
#include "RFIDread.h"

#include "RFIDreadDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRFIDreadDoc

IMPLEMENT_DYNCREATE(CRFIDreadDoc, CDocument)

BEGIN_MESSAGE_MAP(CRFIDreadDoc, CDocument)
	//{{AFX_MSG_MAP(CRFIDreadDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRFIDreadDoc construction/destruction

CRFIDreadDoc::CRFIDreadDoc()
{
	// TODO: add one-time construction code here

}

CRFIDreadDoc::~CRFIDreadDoc()
{
}

BOOL CRFIDreadDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CRFIDreadDoc serialization

void CRFIDreadDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRFIDreadDoc diagnostics

#ifdef _DEBUG
void CRFIDreadDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRFIDreadDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRFIDreadDoc commands
