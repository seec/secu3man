/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

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

#pragma once

// ����� ��������� ������������ (�� �������� ����)
class AFX_EXT_CLASS CDialogWithAccelerators : public CDialog
{
 typedef CDialog Super;

 public:
  CDialogWithAccelerators(); //modeless
  CDialogWithAccelerators(UINT nIDTemplate, CWnd* pParentWnd = NULL );

 protected:
  //����� ����� ������� ���!
  virtual BOOL PreTranslateMessage(MSG* pMsg);
};

//������ � �������������� ������� ������ ������� �� IDOK ��� IDCANCEL
class AFX_EXT_CLASS CModelessDialog : public CDialogWithAccelerators
{
 typedef CDialogWithAccelerators Super;

 public:
  CModelessDialog(); //modeless
  CModelessDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL );

 protected:
  virtual void OnOK();
  virtual void OnCancel();
};
