/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   contacts:
              http://secu-3.org
              email: shabelnikov@secu-3.org
*/

/** \file LogPlayerTabDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "LogPlayerTabDlg.h"

#include "common/FastDelegate.h"
#include "common/MathHelpers.h"
#include "LPControlPanelDlg.h"
#include "MIDesk/LMDeskDlg.h"
#include "MIDesk/MIDeskDlg.h"
#include "MIDesk/CEDeskDlg.h"
#include "ui-core/OScopeCtrl.h"
#include "ui-core/ToolTipCtrlEx.h"

using namespace std;
using namespace fastdelegate;

const UINT CLogPlayerTabDlg::IDD = IDD_LOG_PLAYER;

BEGIN_MESSAGE_MAP(CLogPlayerTabDlg, Super)
 ON_WM_DROPFILES()
END_MESSAGE_MAP()

CLogPlayerTabDlg::CLogPlayerTabDlg(CWnd* pParent /*=NULL*/)
: Super(CLogPlayerTabDlg::IDD, pParent)
, mp_CEDeskDlg(new CCEDeskDlg())
, mp_LMDeskDlg(new CLMDeskDlg())
, mp_MIDeskDlg(new CMIDeskDlg())
, mp_LPPanelDlg(new CLPControlPanelDlg)
, mp_OScopeCtrl(new COScopeCtrl())
, m_enlarged(false)
, m_exfixtures(false)
{
 //empty
}

void CLogPlayerTabDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
}

void CLogPlayerTabDlg::OnDropFiles(HDROP hDropInfo)
{
 TCHAR fileName[MAX_PATH+1];
 if (DragQueryFile(hDropInfo, 0, fileName, MAX_PATH))
 {
  if (m_OnDropFile)
   m_OnDropFile(fileName);
 }
 else
  MessageBeep(MB_ICONEXCLAMATION);
}

LPCTSTR CLogPlayerTabDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

BOOL CLogPlayerTabDlg::OnInitDialog()
{
 Super::OnInitDialog();

 CRect rect;
 GetDlgItem(IDC_LP_MIDESK_FRAME)->GetWindowRect(rect);
 ScreenToClient(rect);

 mp_MIDeskDlg->Create(CMIDeskDlg::IDD, this);
 mp_MIDeskDlg->SetWindowPos(NULL,rect.TopLeft().x,rect.TopLeft().y, rect.Width(), rect.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);
 mp_MIDeskDlg->Show(true);

 GetDlgItem(IDC_LP_CEDESK_FRAME)->GetWindowRect(rect);
 ScreenToClient(rect);
 mp_CEDeskDlg->Create(CCEDeskDlg::IDD, this);
 mp_CEDeskDlg->SetWindowPos(NULL,rect.TopLeft().x,rect.TopLeft().y,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
 mp_CEDeskDlg->Show(true);

 GetDlgItem(IDC_LP_LOGCONTROL_PANEL_FRAME)->GetWindowRect(rect);
 ScreenToClient(rect);
 mp_LPPanelDlg->Create(CLPControlPanelDlg::IDD, this);
 mp_LPPanelDlg->SetWindowPos(NULL,rect.TopLeft().x,rect.TopLeft().y,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
 
 GetDlgItem(IDC_LP_LOG_MARKS_DESK_HOLDER)->GetWindowRect(rect);
 ScreenToClient(rect);
 mp_LMDeskDlg->Create(CLMDeskDlg::IDD, this);
 mp_LMDeskDlg->SetWindowPos(NULL,rect.TopLeft().x,rect.TopLeft().y,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
 mp_LMDeskDlg->Show(true);

 //�������������� �����������
 _InitializeOscilloscopeControl();

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(mp_OScopeCtrl.get(), MLL::GetString(IDS_LP_SIGNAL_OSCILLOSCOPE_TT)));
 mp_ttc->SetMaxTipWidth(100); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 //Enable drap & drop functionality
 DragAcceptFiles(true);

 UpdateDialogControls(this,TRUE);
 return TRUE;
}

void CLogPlayerTabDlg::EnlargeMonitor(bool i_enlarge, bool i_exfixtures)
{
 CRect rect;
 GetClientRect(rect);
 m_enlarged = i_enlarge;
 m_exfixtures = i_exfixtures;

 if (i_enlarge)
 {//remember original positions
  mp_MIDeskDlg->GetWindowRect(m_original_mi_rect);
  ScreenToClient(m_original_mi_rect);
  mp_CEDeskDlg->GetWindowRect(m_original_ce_rect);
  ScreenToClient(m_original_ce_rect);
  //restrict size of MI desk
  CRect cp_rect;
  mp_LPPanelDlg->GetWindowRect(cp_rect);
  ScreenToClient(cp_rect);
  rect.left+=cp_rect.right;
  rect.bottom-=m_original_ce_rect.Height();

  //move and resize MI desk
  CRect mi_rect, mi_rect_o;
  if (i_exfixtures)
  { //extended
   GetDlgItem(IDC_LP_MIDESK_FRAME_F)->GetWindowRect(mi_rect);
   ScreenToClient(mi_rect);
  }
  else //regular
   mi_rect = m_original_mi_rect;
  mi_rect_o = mi_rect;
  _ResizeRect(rect, mi_rect);
  mp_MIDeskDlg->Resize(mi_rect, mi_rect_o);
  mp_MIDeskDlg->ShowExFixtures(i_exfixtures);

  //move CE panel, don't resize it
  int ce_panel_x = mi_rect.CenterPoint().x - (m_original_ce_rect.Width()/2);
  mp_CEDeskDlg->SetWindowPos(0, ce_panel_x, mi_rect.bottom, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
 }
 else
 {//restore original positions
  CRect rc_s;
  mp_MIDeskDlg->GetWindowRect(rc_s); 
  ScreenToClient(rc_s); 
  if (i_exfixtures)
  {
   CRect rc_o, rc_f;
   GetDlgItem(IDC_LP_MIDESK_FRAME)->GetWindowRect(rc_o);
   GetDlgItem(IDC_LP_MIDESK_FRAME_F)->GetWindowRect(rc_f);   
   float rx = (((float)rc_f.Width())/rc_o.Width());
   rc_s.right = MathHelpers::Round(((float)rc_s.left) + (((float)rc_s.Width())/rx));
  }
  mp_MIDeskDlg->Resize(m_original_mi_rect, rc_s);
  mp_CEDeskDlg->Resize(m_original_ce_rect);
  mp_MIDeskDlg->ShowExFixtures(false);
 }
}

void CLogPlayerTabDlg::ShowExFixtures(bool i_exfixtures)
{
 if (m_enlarged && (m_exfixtures!=i_exfixtures))
 {
  EnlargeMonitor(false, !i_exfixtures);
  EnlargeMonitor(true, i_exfixtures);
 }
}

void CLogPlayerTabDlg::_ResizeRect(const CRect& i_external, CRect& io_victim)
{
 float Xf = ((float)i_external.Width()) / io_victim.Width();
 float Yf = ((float)i_external.Height()) / io_victim.Height();
 float factor = min(Xf, Yf);
 //������������
 io_victim.right = MathHelpers::Round((io_victim.Width() * factor));
 io_victim.bottom = MathHelpers::Round((io_victim.Height() * factor));
 io_victim.left = 0;
 io_victim.top = 0;
 //����������
 CPoint center_external = i_external.CenterPoint();
 CPoint center_victim = io_victim.CenterPoint();
 io_victim.OffsetRect(center_external - center_victim);
}

//������������� ������������ ��� ������� ��
void CLogPlayerTabDlg::_InitializeOscilloscopeControl(void)
{
 CRect rect;
 GetDlgItem(IDC_LP_SIGNAL_OSCILLOSCOPE_HOLDER)->GetWindowRect(rect);
 ScreenToClient(rect);

 // create the control
 mp_OScopeCtrl->Create(WS_VISIBLE | WS_CHILD | WS_DISABLED, rect, this);

 // customize the control
 mp_OScopeCtrl->SetRange(0.0, 5.0, 1);
 mp_OScopeCtrl->SetYUnits(MLL::LoadString(IDS_KC_OSCILLOSCOPE_V_UNIT));
 mp_OScopeCtrl->SetXUnits(MLL::LoadString(IDS_KC_OSCILLOSCOPE_H_UNIT));
 mp_OScopeCtrl->SetBackgroundColor(RGB(0, 0, 64));
 mp_OScopeCtrl->SetGridColor(RGB(192, 192, 255));
 mp_OScopeCtrl->SetPlotColor(RGB(255, 255, 255));
}

void CLogPlayerTabDlg::AppendKnockValue(double i_value, bool i_reverse)
{
 mp_OScopeCtrl->AppendPoint(i_value, i_reverse);
}

void CLogPlayerTabDlg::ResetKnockOscilloscope(void)
{
 mp_OScopeCtrl->Reset();
}

void CLogPlayerTabDlg::setOnDropFile(EventString OnFunction)
{m_OnDropFile = OnFunction;}
