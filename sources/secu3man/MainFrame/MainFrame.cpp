 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "Resources\resource.h"
#include "MainFrame.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning( disable : 4800 ) //: forcing value to bool 'true' or 'false' (performance warning)

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
 ON_WM_CREATE()
 ON_WM_SETFOCUS()
 ON_WM_CLOSE()
 ON_COMMAND(ID_APP_ABOUT, OnAppAbout)		
 ON_COMMAND(ID_APP_SETTINGS, OnAppSettings)
 ON_COMMAND(ID_APP_BEGIN_LOG, OnAppBeginLog)
 ON_COMMAND(ID_APP_END_LOG, OnAppEndLog)
 ON_COMMAND(ID_FULL_SCREEN, OnFullScreen)
 ON_UPDATE_COMMAND_UI(ID_APP_BEGIN_LOG,OnUpdateOnAppBeginLog)
 ON_UPDATE_COMMAND_UI(ID_APP_END_LOG,OnUpdateOnAppEndLog)
 ON_UPDATE_COMMAND_UI(ID_FULL_SCREEN, OnUpdateOnFullScreen)
 ON_WM_ACTIVATEAPP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
: m_wnd_initial_size(725,450)
, m_pwndView(NULL)
{
 //na	
}

CMainFrame::~CMainFrame()
{
 //na
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
 if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
  return -1;

 return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
 if( !CFrameWnd::PreCreateWindow(cs) )
  return FALSE;
 
 //������������� ����������� ������ �������� ����
 cs.cx = m_wnd_initial_size.cx;
 cs.cy = m_wnd_initial_size.cy;

 cs.style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
 cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
 cs.lpszClass = AfxRegisterWndClass(0);
 return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
 CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
 CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
 // forward focus to the view window
 if (m_pwndView)
  m_pwndView->SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
 // let the view have first crack at the command
 ASSERT(m_pwndView);
 if (m_pwndView && m_pwndView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
  return TRUE;

 // otherwise, do default handling
 return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::SetView(CChildView* i_pChildView) 
{
 m_pwndView = i_pChildView;
}

void CMainFrame::setOnClose(EventResult i_OnClose)
{
 m_OnClose = i_OnClose;
}

void CMainFrame::setOnAskFullScreen(EventResult i_OnAskFullScreen)
{
 m_OnAskFullScreen = i_OnAskFullScreen;
}

void CMainFrame::setOnFullScreen(EventResult i_OnFullScreen)
{
 m_OnFullScreen = i_OnFullScreen;
}

void CMainFrame::setOnFullScreenNotify(EventHandler1 i_OnFullScreenNotify)
{
 m_OnFullScreenNotify = i_OnFullScreenNotify;
}

void CMainFrame::setOnActivate(EventHandler1 i_OnActivate)
{
 m_OnActivate = i_OnActivate;
}

void CMainFrame::setOnAppAbout(EventHandler i_OnFunction) 
{
 m_OnAppAbout = i_OnFunction;
}

void CMainFrame::setOnAppSettings(EventHandler i_OnFunction) 
{
 m_OnAppSettings = i_OnFunction;
}

void CMainFrame::setOnAppBeginLog(EventHandler i_OnFunction) 
{
 m_OnAppBeginLog = i_OnFunction;
}

void CMainFrame::setOnAppEndLog(EventHandler i_OnFunction) 
{
 m_OnAppEndLog = i_OnFunction;
}

void CMainFrame::setIsBeginLoggingAllowed(EventResult i_OnFunction) 
{
 m_IsBeginLoggingAllowed = i_OnFunction;
}

void CMainFrame::setIsEndLoggingAllowed(EventResult i_OnFunction) 
{
 m_IsEndLoggingAllowed = i_OnFunction;
}

void CMainFrame::OnClose() 
{
 bool result = true;
 if (m_OnClose)
  result = m_OnClose();
	
 if (result)
  CFrameWnd::OnClose();
}

void CMainFrame::OnAppAbout()
{
 if (m_OnAppAbout)
  m_OnAppAbout();
}

void CMainFrame::OnAppSettings()
{
 if (m_OnAppSettings)
  m_OnAppSettings();
}

void CMainFrame::OnAppBeginLog()
{
 if (m_OnAppBeginLog)
  m_OnAppBeginLog();
}

void CMainFrame::OnAppEndLog()
{
 if (m_OnAppEndLog)
  m_OnAppEndLog();
}

void CMainFrame::OnUpdateOnAppBeginLog(CCmdUI* pCmdUI)
{
 BOOL enable = FALSE;
 if (m_IsBeginLoggingAllowed)
  enable = m_IsBeginLoggingAllowed();
 pCmdUI->Enable(enable);
}

void CMainFrame::OnUpdateOnAppEndLog(CCmdUI* pCmdUI)
{
 BOOL enable = FALSE;
 if (m_IsEndLoggingAllowed)
  enable = m_IsEndLoggingAllowed();
 pCmdUI->Enable(enable);
}

void CMainFrame::OnActivateApp(BOOL bActive, HTASK hTask)
{
 ASSERT(m_OnActivate); 
 if (m_OnActivate)
  m_OnActivate(bActive == TRUE);
} 

void CMainFrame::OnFullScreen()
{
 bool what = false;
 if (m_OnFullScreen)
  what = m_OnFullScreen();

 if (m_OnFullScreenNotify)
  m_OnFullScreenNotify(what);
}

void CMainFrame::OnUpdateOnFullScreen(CCmdUI* pCmdUI)
{
 bool enable = false;
 if (m_OnAskFullScreen)
  enable = m_OnAskFullScreen();

 pCmdUI->Enable(enable);
}
