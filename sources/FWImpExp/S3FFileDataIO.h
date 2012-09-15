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

#include <vector>
#include "common/unicodesupport.h"
#include "io-core/FirmwareMapsDataHolder.h"

class S3FFileDataIO
{
 public:
  S3FFileDataIO();
 ~S3FFileDataIO();

  bool Load(const _TSTRING i_file_name);
  bool Save(const _TSTRING i_file_name);

  const FWMapsDataHolder& GetData() const;
  FWMapsDataHolder& GetDataLeft();

  size_t GetMapSetsNumber(void) const;
  bool IsFileIntegrityOk(void) const;

 private:
  FWMapsDataHolder m_data;
  bool m_file_crc_ok;
};
