/*
   Copyright (C) AC SOFTWARE SP. Z O.O.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
   */

#ifndef _supla_fram_spi_h
#define _supla_fram_spi_h

#include <SPI.h>

#include "Adafruit_FRAM_SPI.h"
#include "storage.h"

#define SUPLA_FRAM_WRITING_PERIOD 1000

namespace Supla {

class FramSpi : public Storage {
 public:
  FramSpi(int8_t clk,
          int8_t miso,
          int8_t mosi,
          int8_t framCs,
          unsigned int storageStartingOffset = 0)
      : Storage(storageStartingOffset),
        fram(clk, miso, mosi, framCs),
        lastWriteTimestamp(0) {
  }

  FramSpi(int8_t framCs, unsigned int storageStartingOffset = 0)
      : Storage(storageStartingOffset), fram(framCs), lastWriteTimestamp(0) {
  }

  bool init() {
    if (fram.begin()) {
      Serial.println("Storage: FRAM found");
    } else {
      Serial.println("Storage: FRAM not found");
    }

    return Storage::init();
  }

  bool saveStateAllowed(unsigned long ms) {
    if (ms - lastWriteTimestamp > SUPLA_FRAM_WRITING_PERIOD) {
      lastWriteTimestamp = ms;
      return true;
    }
    return false;
  }

  void commit(){};

 protected:
  int readStorage(int offset, unsigned char *buf, int size, bool logs) {
    if (logs) {
      Serial.print("readStorage: ");
      Serial.print(size);
      Serial.print("; Read: [");
    }
    for (int i = 0; i < size; i++) {
      buf[i] = fram.read8(offset + i);
      if (logs) {
        Serial.print(static_cast<unsigned char *>(buf)[i], HEX);
        Serial.print(" ");
      }
    }
    if (logs) {
      Serial.println("]");
    }
    return size;
  }

  int writeStorage(int offset, const unsigned char *buf, int size) {
    fram.writeEnable(true);
    fram.write(offset, buf, size);
    fram.writeEnable(false);
    Serial.print("Wrote ");
    Serial.print(size);
    Serial.println(" bytes to storage");
    return size;
  }

  unsigned long lastWriteTimestamp;

  Adafruit_FRAM_SPI fram;
};

};  // namespace Supla

#endif
