/* Common utilities - language layer
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
             e-mail: shabelnikov-stc@mail.ru
             Microprocessor systems - design & programming.
*/

/** \file Dll.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

namespace DLL {

static HMODULE GetModuleHandle(void)
{
 MEMORY_BASIC_INFORMATION mbi;
 return ((::VirtualQuery(GetModuleHandle, &mbi, sizeof(mbi)) != 0) ?
 (HMODULE) mbi.AllocationBase : NULL);
}
} //ns
