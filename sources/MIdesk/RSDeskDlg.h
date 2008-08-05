
#pragma once

#include "resource.h"

#include "io-core\SECU3IO.h"
#include "IRSView.h"

/////////////////////////////////////////////////////////////////////////////
// CRSDeskDlg dialog

class AFX_EXT_CLASS CRSDeskDlg : public CDialog, public IRSView
{
// Construction
public:
	CRSDeskDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRSDeskDlg)
	enum { IDD = IDD_RAW_SENSORS_DESK };
	float m_map_value;
	float m_ubat_value;
	float m_temp_value;

	CStatic m_map_field;
	CStatic m_ubat_field;
	CStatic m_temp_field;

	CStatic m_map_caption;
	CStatic m_ubat_caption;
	CStatic m_temp_caption;

	CStatic m_map_unit;
	CStatic m_ubat_unit;
	CStatic m_temp_unit;
    
	//}}AFX_DATA

   //--------interface implementation---------------
	virtual void Show(bool show);
	virtual void Enable(bool enable);
    virtual void SetValues(const SECU3IO::RawSensDat* i_values);
    virtual void GetValues(SECU3IO::RawSensDat* o_values);
   //-----------------------------------------------


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRSDeskDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRSDeskDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////