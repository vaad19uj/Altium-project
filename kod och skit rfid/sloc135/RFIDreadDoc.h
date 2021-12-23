// RFIDreadDoc.h : interface of the CRFIDreadDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RFIDREADDOC_H__14CDE613_C632_4ABC_84D7_E6C2A8B025AB__INCLUDED_)
#define AFX_RFIDREADDOC_H__14CDE613_C632_4ABC_84D7_E6C2A8B025AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CRFIDreadDoc : public CDocument
{
protected: // create from serialization only
	CRFIDreadDoc();
	DECLARE_DYNCREATE(CRFIDreadDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRFIDreadDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRFIDreadDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CRFIDreadDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RFIDREADDOC_H__14CDE613_C632_4ABC_84D7_E6C2A8B025AB__INCLUDED_)
