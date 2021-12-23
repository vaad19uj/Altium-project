// Lights.cpp : implementation file
//

#include "stdafx.h"
#include "RFIDread.h"
#include "Lights.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Lights

Lights::Lights()
{
	EnableAutomation();
}

Lights::~Lights()
{
}

void Lights::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CStatic::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(Lights, CStatic)
	//{{AFX_MSG_MAP(Lights)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(Lights, CStatic)
	//{{AFX_DISPATCH_MAP(Lights)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ILights to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {12E1833F-C591-427D-BC0D-A2E0EB467397}
static const IID IID_ILights =
{ 0x12e1833f, 0xc591, 0x427d, { 0xbc, 0xd, 0xa2, 0xe0, 0xeb, 0x46, 0x73, 0x97 } };

BEGIN_INTERFACE_MAP(Lights, CStatic)
	INTERFACE_PART(Lights, IID_ILights, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Lights message handlers

BOOL Lights::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

HBRUSH Lights::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CStatic::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}
