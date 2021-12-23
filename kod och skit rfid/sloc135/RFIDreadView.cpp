// RFIDreadView.cpp : implementation of the CRFIDreadView class
//

#include "stdafx.h"
#include "RFIDread.h"

#include "RFIDreadDoc.h"
#include "RFIDreadView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRFIDreadView

IMPLEMENT_DYNCREATE(CRFIDreadView, CView)

BEGIN_MESSAGE_MAP(CRFIDreadView, CView)
	//{{AFX_MSG_MAP(CRFIDreadView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRFIDreadView construction/destruction

CRFIDreadView::CRFIDreadView()
{
	// TODO: add construction code here

}

CRFIDreadView::~CRFIDreadView()
{
}

BOOL CRFIDreadView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CRFIDreadView drawing

void CRFIDreadView::OnDraw(CDC* pDC)
{
	CRFIDreadDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CRFIDreadView printing

BOOL CRFIDreadView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CRFIDreadView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CRFIDreadView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CRFIDreadView diagnostics

#ifdef _DEBUG
void CRFIDreadView::AssertValid() const
{
	CView::AssertValid();
}

void CRFIDreadView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRFIDreadDoc* CRFIDreadView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRFIDreadDoc)));
	return (CRFIDreadDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRFIDreadView message handlers
