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

/** \file LogPlayerTabController.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include <shlwapi.h>
#include "Resources/resource.h"
#include "LogPlayerTabController.h"

#include "Application/CommunicationManager.h"
#include "common/FastDelegate.h"
#include "common/MathHelpers.h"
#include "io-core/LogReader.h"
#include "io-core/ufcodes.h"
#include "MainFrame/StatusBarManager.h"
#include "MIDesk/CEDeskDlg.h"
#include "MIDesk/LMDeskDlg.h"
#include "MIDesk/MIDeskDlg.h"
#include "Settings/ISettingsData.h"
#include "TabDialogs/LogPlayerTabDlg.h"
#include "TabDialogs/LPControlPanelDlg.h"
#include "ui-core/OScopeCtrl.h"

using namespace fastdelegate;
using namespace SECU3IO;

#define QUEUE_SIZE 32   //������ ������� ��� ���������� �������
#define EHKEY _T("LogPlayerCntr")

//���������� ������ ����������� � �������
//���������� ������� � �������������.
static unsigned long CalcPeriod(SYSTEMTIME &i_time1, SYSTEMTIME &i_time2)
{
 unsigned long ms1 = (i_time1.wSecond * 1000) + i_time1.wMilliseconds;
 unsigned long ms2 = (i_time2.wSecond * 1000) + i_time2.wMilliseconds;
 return (ms1 <= ms2) ? (ms2 - ms1) : (60000 - ms1) + ms2;
}

CLogPlayerTabController::CLogPlayerTabController(CLogPlayerTabDlg* ip_view, CCommunicationManager* ip_comm, CStatusBarManager* ip_sbar, ISettingsData* ip_settings)
: mp_view(ip_view)
, mp_comm(ip_comm)
, mp_sbar(ip_sbar)
, mp_settings(ip_settings)
, mp_log_reader(new LogReader)
, m_now_tracking(false)
, m_period_before_tracking(0)
, m_playing(false)
, m_current_time_factor(5) //1:1
{
#define _IV(id, name, value) (std::make_pair((id), std::make_pair(_TSTRING(name), (value))))
 m_time_factors.insert(_IV(0, _T("16 : 1"),0.0625f));
 m_time_factors.insert(_IV(1, _T(" 8 : 1"), 0.125f));
 m_time_factors.insert(_IV(2, _T(" 4 : 1"), 0.25f));
 m_time_factors.insert(_IV(3, _T(" 3 : 1"), 0.33f));
 m_time_factors.insert(_IV(4, _T(" 2 : 1"), 0.50f));
 m_time_factors.insert(_IV(5, _T(" 1 : 1"), 1.00f));
 m_time_factors.insert(_IV(6, _T(" 1 : 2"), 2.00f));
 m_time_factors.insert(_IV(7, _T(" 1 : 3"), 3.00f));
 m_time_factors.insert(_IV(8, _T(" 1 : 4"), 4.00f));
 m_time_factors.insert(_IV(9, _T(" 1 : 8"), 8.00f));
#undef _IV

 //������������� �������� (����������� ������� �� �������������)
 mp_view->mp_LPPanelDlg->setOnOpenFileButton(MakeDelegate(this,&CLogPlayerTabController::OnOpenFileButton));
 mp_view->mp_LPPanelDlg->setOnPlayButton(MakeDelegate(this,&CLogPlayerTabController::OnPlayButton));
 mp_view->mp_LPPanelDlg->setOnNextButton(MakeDelegate(this,&CLogPlayerTabController::OnNextButton));
 mp_view->mp_LPPanelDlg->setOnPrevButton(MakeDelegate(this,&CLogPlayerTabController::OnPrevButton));
 mp_view->mp_LPPanelDlg->setOnTimeFactorCombo(MakeDelegate(this,&CLogPlayerTabController::OnTimeFactorCombo));
 mp_view->mp_LPPanelDlg->setOnSliderMoved(MakeDelegate(this,&CLogPlayerTabController::OnSliderMoved));
 mp_view->setOnDropFile(MakeDelegate(this,&CLogPlayerTabController::OnDropFile));

 m_timer.SetMsgHandler(this, &CLogPlayerTabController::OnTimer);
}

CLogPlayerTabController::~CLogPlayerTabController()
{
 //empty
}

//���������� ��������� ���������!
void CLogPlayerTabController::OnSettingsChanged(void)
{
 //�������� ����������� ��� ������� ��������� ���������������� ����������
 mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION, true);
 mp_view->mp_MIDeskDlg->SetUpdatePeriod(mp_settings->GetMIDeskUpdatePeriod());

 //��������� ��������� ��������
 mp_view->mp_MIDeskDlg->SetTachometerMax(mp_settings->GetTachometerMax());
 mp_view->mp_MIDeskDlg->SetPressureMax(mp_settings->GetPressureMax());

 mp_view->ShowExFixtures(mp_settings->GetShowExFixtures());
 mp_view->Invalidate();
}

//from MainTabController
void CLogPlayerTabController::OnActivate(void)
{
 mp_view->mp_MIDeskDlg->SetTachometerMax(mp_settings->GetTachometerMax());
 mp_view->mp_MIDeskDlg->SetPressureMax(mp_settings->GetPressureMax());

 //////////////////////////////////////////////////////////////////
 //���������� ���������� � ������ ������ �� SECU-3
 mp_comm->m_pAppAdapter->AddEventHandler(this,EHKEY);
 mp_comm->setOnSettingsChanged(MakeDelegate(this,&CLogPlayerTabController::OnSettingsChanged));
 //////////////////////////////////////////////////////////////////

 //�������� ����������� ��� ������� ��������� ���������������� ����������
 mp_comm->SwitchOn(CCommunicationManager::OP_ACTIVATE_APPLICATION);

 //���������� ��������� ��������� ��� ���������� ����������, ��� ��� OnConnection ���������� ������ ����
 //���������� ��� ����������� ������������� (����� ����������� ����������������� �����������)
 bool online_status = mp_comm->m_pControlApp->GetOnlineStatus();
 OnConnection(online_status);

 std::vector<_TSTRING> tf_content;
 for(size_t i = 0; i < m_time_factors.size(); ++i)
  tf_content.push_back(m_time_factors[i].first.c_str());
 mp_view->mp_LPPanelDlg->FillTimeFactorCombo(tf_content);

 mp_view->mp_LPPanelDlg->SetTimeFactor(m_current_time_factor);

 mp_view->mp_MIDeskDlg->SetUpdatePeriod(mp_settings->GetMIDeskUpdatePeriod());
 mp_view->mp_MIDeskDlg->ShowChokePos(true);
 mp_view->mp_MIDeskDlg->ShowSpeedAndDistance(true);
}

//from MainTabController
void CLogPlayerTabController::OnDeactivate(void)
{
 mp_comm->m_pAppAdapter->RemoveEventHandler(EHKEY);
 mp_sbar->SetInformationText(_T(""));
 _ClosePlayer();
}

//hurrah!!! ������� ����� �� SECU-3
void CLogPlayerTabController::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{
 //������ ������: ������ ����� � �������������� �����
 if (i_descriptor == OP_COMP_NC)
 {
  const OPCompNc* p_ndata = (OPCompNc*)ip_packet;
  switch(p_ndata->opcode)
  {
   case OPCODE_EEPROM_PARAM_SAVE: //��������� ���� ���������
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_PARAMS_HAS_BEEN_SAVED));
    return;
   case OPCODE_SAVE_TABLSET:      //������� ���� ���������
    mp_sbar->SetInformationText(MLL::LoadString(IDS_PM_TABLSET_HAS_BEEN_SAVED));    
    return;
  }
 }
}

//��������� ����������� ����������!
void CLogPlayerTabController::OnConnection(const bool i_online)
{
 int state;
 ASSERT(mp_sbar);

 if (i_online) //������� � ������
  state = CStatusBarManager::STATE_ONLINE;
 else
  state = CStatusBarManager::STATE_OFFLINE;

 mp_sbar->SetConnectionState(state);
}

bool CLogPlayerTabController::OnClose(void)
{
 return true;
}

void CLogPlayerTabController::OnCloseNotify(void)
{
 //empty
}

bool CLogPlayerTabController::OnAskFullScreen(void)
{
 return true;
}

void CLogPlayerTabController::OnFullScreen(bool i_what, const CRect& i_rect)
{
 //��� ��������� �������������� ������ �������� ���� ������� ��� ����� ��� ����
 //������ ��� ��������. ��� ���������� �������������� ������ ��� ������� ��� ��������
 //������� � ������� �������.

 if (i_what)
  mp_view->MoveWindow(i_rect.left, i_rect.top, i_rect.Width(), i_rect.Height());

 mp_view->EnlargeMonitor(i_what, mp_settings->GetShowExFixtures());
}

bool CLogPlayerTabController::OnAskChangeTab(void)
{
 return true; //always allows to change tab
}

void CLogPlayerTabController::OnOpenFileButton(void)
{
 if (mp_log_reader->IsOpened())
 {  //������ ������ ��� ������ - ��������� �����
  _ClosePlayer();
  return;
 }
 _TSTRING fileName = _T("");
 _OpenFile(fileName);
}

void CLogPlayerTabController::OnPlayButton(void)
{
 if (mp_view->mp_LPPanelDlg->GetPlayButtonState()) //start
  _Play(true);
 else //stop
  _Play(false);
}

void CLogPlayerTabController::OnNextButton(void)
{
 if (!mp_log_reader->IsNextPossible())
  return;
 _ProcessOneRecord(false, DIR_NEXT);
}

void CLogPlayerTabController::OnPrevButton(void)
{
 if (!mp_log_reader->IsPrevPossible())
  return;
 _ProcessOneRecord(false, DIR_PREV);
}

void CLogPlayerTabController::OnTimeFactorCombo(size_t i_factor_code)
{ //��������� ��� ���������� ������� �������
 m_current_time_factor = i_factor_code;
}

void CLogPlayerTabController::OnSliderMoved(UINT i_nSBCode, unsigned long i_nPos)
{
#define _END_TRACKING() m_now_tracking = true; \
    m_period_before_tracking = m_timer.GetPeriod();\
    m_timer.KillTimer();

 switch(i_nSBCode)
 {
  case TB_ENDTRACK:
   m_now_tracking = false;
   if (m_playing)
    m_timer.SetTimer(m_period_before_tracking);
   break;

  case TB_BOTTOM:
  case TB_TOP:
   {
   _END_TRACKING();
   long count = ((long)mp_view->mp_LPPanelDlg->GetSliderPosition()) - (long)mp_log_reader->GetCurPos();
   for(long i = 0; i < abs(count); ++i)
    _ProcessOneRecord(false, (count > 0) ? DIR_NEXT : DIR_PREV, false);
   }
   break;

  case TB_PAGEDOWN:
  case TB_PAGEUP:
   {
    _END_TRACKING();
    unsigned long count = mp_view->mp_LPPanelDlg->GetSliderPageSize();
    for(unsigned long i = 0; i < count; ++i)
     _ProcessOneRecord(false, i_nSBCode==TB_PAGEDOWN ? DIR_NEXT : DIR_PREV, false);
   }
   break;

  case TB_LINEDOWN:
  case TB_LINEUP:
   {
    _END_TRACKING();
    unsigned long count = mp_view->mp_LPPanelDlg->GetSliderLineSize();
    for(unsigned long i = 0; i < count; ++i)
     _ProcessOneRecord(false, i_nSBCode==TB_LINEDOWN ? DIR_NEXT : DIR_PREV, false);
   }
   break;

  case TB_THUMBPOSITION:
  case TB_THUMBTRACK:
   _END_TRACKING();
   long count = (int)i_nPos - (int)mp_log_reader->GetCurPos();
   for(long i = 0; i < abs(count); ++i)
    _ProcessOneRecord(false, (count > 0) ? DIR_NEXT : DIR_PREV, false);

   break;
 }
#undef _END_TRACKING
}

void CLogPlayerTabController::OnTimer(void)
{
 if (m_now_tracking || !m_playing)
  return;

 if (!mp_log_reader->IsNextPossible())
 {
  _Play(false); //������������� ������������ ���� ����� �� �����
  return;
 }

 _ProcessOneRecord(true, DIR_NEXT);
}

void CLogPlayerTabController::OnDropFile(_TSTRING fileName)
{
 if (mp_log_reader->IsOpened())
  _ClosePlayer(); //��������� ����� ����� ������� ���������� ������������

 _OpenFile(fileName);
}

void CLogPlayerTabController::_OpenFile(const _TSTRING& fileName)
{
 HANDLE   hFile = NULL;
 static TCHAR BASED_CODE szFilter[] = _T("CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||");
 CFileDialog open(TRUE, NULL, NULL, NULL, szFilter, NULL);

 if (fileName.empty() && open.DoModal() != IDOK)
  return; //������������ ���������

 mp_log_reader->SetSeparatingSymbol(mp_settings->GetCSVSepSymbol());

 LogReader::FileError error_id;
 _TSTRING file_path = fileName.empty() ? open.GetPathName().GetBuffer(0) : fileName;
 bool result = mp_log_reader->OpenFile(file_path, error_id);
 if (false==result)
 {
  if (error_id==LogReader::FE_OPEN)
   AfxMessageBox(MLL::LoadString(IDS_LP_CANT_OPEN_FILE));
  else if (error_id==LogReader::FE_FORMAT)
  {
   AfxMessageBox(MLL::LoadString(IDS_LP_INCORRECT_FILE_FORMAT));
  }
  else
   ASSERT(0);

  mp_log_reader->CloseFile();
  return; //�� ����� ����������, ��� ��� ��������� ������
 }

 ////////////////////////////////////////////////////////////////
 mp_view->mp_LPPanelDlg->SetOpenFileButtonText(MLL::GetString(IDS_LP_CLOSE_FILE));

 //obtain file name from full path
 TCHAR stripped_name[MAX_PATH+1] = {0};
 file_path.copy(stripped_name, file_path.size());
 PathStripPath(stripped_name);

 CString string;
 string.Format(MLL::LoadString(IDS_LP_FILE_INFO_FMT_STRING), stripped_name, mp_log_reader->GetCount());
 mp_view->mp_LPPanelDlg->SetFileIndicator(string.GetBuffer(0));

 mp_view->mp_MIDeskDlg->Enable(true);
 mp_view->mp_CEDeskDlg->Enable(true);
 mp_view->mp_LMDeskDlg->Enable(true);
 mp_view->mp_LPPanelDlg->EnableAll(true);
 mp_view->mp_OScopeCtrl->EnableWindow(true);

 //�������������� ������ ������ � �������� ����� �����������
 if (mp_log_reader->GetCount() > 0)
 {
  _InitPlayer();
  _Play(true);
 }
 ////////////////////////////////////////////////////////////////
}

void CLogPlayerTabController::_GoNext(void)
{
 //��������� ������ � ��������� ��� � ������� �������
 unsigned long period = CalcPeriod(m_prev_record.first, m_curr_record.first);
 m_last_perionds.push_front(period);

 //������������ ������ �������
 if (m_last_perionds.size() > QUEUE_SIZE)
  m_last_perionds.pop_back();

 m_prev_record = m_curr_record;
}

void CLogPlayerTabController::_GoBack(void)
{
 //��������� � ������� c����
 unsigned long period = CalcPeriod(m_curr_record.first, m_prev_record.first);
 m_last_perionds.push_back(period);

 //������������ ������ �������
 if (m_last_perionds.size() > QUEUE_SIZE)
  m_last_perionds.pop_front();

 m_prev_record = m_curr_record;
}

void CLogPlayerTabController::_GetRecord(void)
{
 bool status = mp_log_reader->GetRecord(m_curr_record.first, m_curr_record.second, m_curr_marks);
 if (!status)
 {
  AfxMessageBox(MLL::LoadString(IDS_LP_FILE_READ_ERROR), MB_OK);
  _ClosePlayer(); //���������� ������������
 }
}

unsigned long CLogPlayerTabController::_GetAveragedPeriod(void)
{
 unsigned long sum = 0;

 //������� ������� �� ��������� ��������
 size_t count = m_last_perionds.size();
 for(size_t i = 0; i < count; ++i)
  sum+=m_last_perionds[i];

 if (m_last_perionds.size()!=0)
  return sum / m_last_perionds.size();
 else
  return 0; //��� �������
}

void CLogPlayerTabController::_ProcessOneRecord(bool i_set_timer, EDirection i_direction, bool i_set_slider /*= true*/)
{
 if (i_direction == DIR_NEXT)
  mp_log_reader->Next();
 else if (i_direction == DIR_PREV)
  mp_log_reader->Prev();
 else
 {
  ASSERT(0);
  return;
 }

 _GetRecord();

 if (i_direction == DIR_NEXT)
  _GoNext();
 else if (i_direction == DIR_PREV)
  _GoBack();
 else
 {
  ASSERT(0);
  return;
 }

 _UpdateTimerPeriod(i_set_timer);
 _DisplayCurrentRecord(i_direction);
 _UpdateButtons();

 if (i_set_slider)
  mp_view->mp_LPPanelDlg->SetSliderPosition(mp_log_reader->GetCurPos());

 //Stop playing if mark appears and it is enabled by a check box
 if (m_curr_marks && mp_view->mp_LPPanelDlg->GetStopOnMarksCheck())
 {
  if (mp_view->mp_LPPanelDlg->GetPlayButtonState()) //start
  {
   _Play(false);
   mp_view->mp_LPPanelDlg->SetPlayButtonState(false);
  }
 }
}

void CLogPlayerTabController::_UpdateTimerPeriod(bool i_set_timer)
{
 //��������� ������� ������. ��������� ������ �������
 unsigned long period = _GetAveragedPeriod();
 period = MathHelpers::Round(((float)period) * m_time_factors[m_current_time_factor].second);

 //������������� ������ ���� �����. ���� ����� ������ ���������� �� �������
 //������ ��� �� 10%, �� �� ������������� ������
 if ((i_set_timer) && ((unsigned long)abs(m_timer.GetPeriod() - (int)period) > (period / 10)))
 {
  m_timer.KillTimer();
  m_timer.SetTimer(period);
 }
}

void CLogPlayerTabController::_Play(bool i_play)
{
 if (i_play)
  m_timer.SetTimer(0);
 else
  m_timer.KillTimer();

 m_playing = i_play;
 mp_view->mp_LPPanelDlg->SetPlayButtonState(i_play);
}

void CLogPlayerTabController::_InitPlayer(void)
{
 mp_view->mp_LPPanelDlg->EnableSlider(mp_log_reader->GetCount() > 1);
 mp_view->mp_LPPanelDlg->SetSliderRange(0, mp_log_reader->GetCount());
 mp_view->mp_LPPanelDlg->SetSliderPosition(mp_log_reader->GetCurPos());
 _GetRecord();
 _DisplayCurrentRecord(DIR_NEXT);
 _UpdateButtons();
 m_prev_record = m_curr_record;
}

void CLogPlayerTabController::_ClosePlayer(void)
{
 mp_view->mp_LPPanelDlg->SetFileIndicator(_T(""));
 mp_view->mp_LPPanelDlg->SetOpenFileButtonText(MLL::GetString(IDS_LP_OPEN_FILE));
 mp_view->mp_LPPanelDlg->SetSliderPosition(0);
 mp_view->mp_MIDeskDlg->Enable(false);
 mp_view->mp_CEDeskDlg->Enable(false);
 mp_view->mp_LMDeskDlg->Enable(false);
 mp_view->mp_LPPanelDlg->EnableAll(false);
 mp_view->mp_OScopeCtrl->EnableWindow(false);
 m_timer.KillTimer();
 mp_log_reader->CloseFile();
 mp_view->mp_LPPanelDlg->SetPositionIndicator(_T(""));
 mp_view->ResetKnockOscilloscope();
 mp_view->mp_LMDeskDlg->SetValues(false, false, false);
}

void CLogPlayerTabController::_DisplayCurrentRecord(EDirection i_direction)
{
 //��������� �������, � ����� ��������� ������� ��������, ���� �����
 mp_view->mp_MIDeskDlg->SetValues(&m_curr_record.second);

 //��������� ������
 mp_view->mp_CEDeskDlg->SetValues(m_curr_record.second.ce_errors);

 //��������� ����� ����
 mp_view->mp_LMDeskDlg->SetValues(m_curr_marks & 0x1, m_curr_marks & 0x2, m_curr_marks & 0x4);

 //������� ����� ������
 CString string;
 string.Format(_T("%05d    %02d:%02d:%02d.%02d"),
     mp_log_reader->GetCurPos() + 1,
     m_curr_record.first.wHour,
     m_curr_record.first.wMinute,
     m_curr_record.first.wSecond,
     m_curr_record.first.wMilliseconds / 10);
 mp_view->mp_LPPanelDlg->SetPositionIndicator(string.GetBuffer(0));

 //������� ������ ��
 mp_view->AppendKnockValue(m_curr_record.second.knock_k, i_direction == DIR_PREV);
}

void CLogPlayerTabController::_UpdateButtons(void)
{
 if (mp_view->mp_LPPanelDlg->IsNextButtonEnabled()!=mp_log_reader->IsNextPossible())
  mp_view->mp_LPPanelDlg->EnableNextButton(mp_log_reader->IsNextPossible());
 if (mp_view->mp_LPPanelDlg->IsPrevButtonEnabled()!=mp_log_reader->IsPrevPossible())
  mp_view->mp_LPPanelDlg->EnablePrevButton(mp_log_reader->IsPrevPossible());
 if (mp_view->mp_LPPanelDlg->IsPlayButtonEnabled()!=mp_log_reader->IsNextPossible())
  mp_view->mp_LPPanelDlg->EnablePlayButton(mp_log_reader->IsNextPossible());
}
