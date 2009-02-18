/****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "LogWriter.h"
#include "ufcodes.h"
#include "SECU3IO.h"

using namespace SECU3IO;

LogWriter::LogWriter()
: m_is_busy(false)
, m_out_handle(NULL)
{
}

LogWriter::~LogWriter()
{
 //��������� ������� ������ ���� ���� �� ��� �� ��������
 if (IsLoggingInProcess())
  EndLogging();
}

void LogWriter::OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet)
{
 if (i_descriptor==SENSOR_DAT)
 {
 ASSERT(m_out_handle);
 SensorDat *p_sensors = reinterpret_cast<SensorDat*>(ip_packet);

 SYSTEMTIME time;
 ::GetLocalTime(&time);

 //���������� ASCII ������, ���� �� ������ ���� ���������
 //"hh:mm:ss.ms", ms - ����� ���� �������  
 fprintf(m_out_handle,"%02d:%02d:%02d.%02d,",time.wHour,time.wMinute,time.wSecond,time.wMilliseconds/10);

 fprintf(m_out_handle,"%04d,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%02d,%01d,%01d,%01d\r\n",
	                   p_sensors->frequen,
					   p_sensors->adv_angle,
					   p_sensors->pressure,
					   p_sensors->voltage,
					   p_sensors->temperat,
					   p_sensors->knock_k,
					   (int)p_sensors->air_flow,
					   (int)p_sensors->carb,
					   (int)p_sensors->gas,
					   (int)p_sensors->ephh_valve);
 }
}

void LogWriter::OnConnection(const bool i_online)
{
 //na
}


bool LogWriter::BeginLogging(const _TSTRING& i_folder, _TSTRING* o_full_file_name /* = NULL*/)
{
 //���������� ��� ����� � ��������� ��� 
 SYSTEMTIME time;
 ::GetLocalTime(&time);

 _TSTRING full_file_name = i_folder;
 CString string;
 //yyyy.mm.dd_hh.mm.ss.csv
 string.Format(_T("%04d.%02d.%02d_%02d.%02d.%02d.csv"),time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond);
 
 full_file_name += _T("\\");
 full_file_name += string;
 *o_full_file_name = full_file_name;

 m_out_handle = _tfopen(full_file_name.c_str(),_T("wb+"));
 if (NULL == m_out_handle)
  return false;

 m_is_busy = true;
 return true;
}

void LogWriter::EndLogging(void)
{
 //��������� ����
 fclose(m_out_handle); 
 m_is_busy = false;
}


bool LogWriter::IsLoggingInProcess(void)
{
 return m_is_busy;
}