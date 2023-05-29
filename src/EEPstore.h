/*
This file is part of EEPstore.
Copyright (C) 2023 Florian Rau.

EEPstore is free software: you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free Software
Foundation, either version 3 of the License, or any later version.

EEPstore is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with EEPstore. If not, see <https://www.gnu.org/licenses/>.

_______________________________________________________________________________


REVISION HISTORY

version 1.0.0   initial release (thank you: Florian Uekermann)
version 1.0.1   default address: 0
version 1.0.2   reorganize, relicense & publish as library
version 1.0.3   fix compatibility with Arduino registry
version 1.0.4   separate methods: getIfValid & getOrSet

_______________________________________________________________________________
*/

#pragma once

#include <Arduino.h>
#include <EEPROM.h>
#include <util/crc16.h>

template <class T> class EEPstore {

  EEPstore(const T &dataRef) : data(dataRef), crc(calcCRC()) {}

  uint16_t calcCRC() {
    uint16_t out         = 0;
    const uint8_t *bytes = (uint8_t *)&data;
    for (size_t i = 0; i < sizeof(T); i++) {
      out = _crc16_update(out, bytes[i]);
    }
    return out;
  }

  const T data;
  const uint16_t crc;

public:
  static inline bool getIfValid(T &dataRef, const uint16_t address = 0) {
    EEPstore<T> storage(dataRef);
    EEPROM.get(address, storage);
    bool valid = storage.crc == storage.calcCRC();
    if (valid) {
      dataRef = storage.data;
    }
    return valid;
  }

  static inline bool getOrSet(T &dataRef, const uint16_t address = 0) {
    bool valid = getIfValid(dataRef, address);
    if (!valid) {
      set(dataRef, address);
    }
    return valid;
  }

  static inline void set(const T &dataRef, const uint16_t address = 0) {
    EEPstore<T> storage(dataRef);
    EEPROM.put(address, storage);
  }
};
