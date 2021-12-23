#if !defined(AFX_LIGHTS_H__6C1FECA8_7FE7_4F53_A3D2_C14F108A0F7B__INCLUDED_)
#define AFX_LIGHTS_H__6C1FECA8_7FE7_4F53_A3D2_C14F108A0F7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Lights.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Lights window

class Lights : public CStatic
{
// Construction
public:
	Lights();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Lights)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~Lights();

	// Generated message map functions
protected:
	//{{AFX_MSG(Lights)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(Lights)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIGHTS_H__6C1FECA8_7FE7_4F53_A3D2_C14F108A0F7B__INCLUDED_)
