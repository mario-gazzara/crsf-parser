#include "crsf_parser.h"
#include "crsf_utils.h"

using namespace crsf;

bool CrsfParser::feed(uint8_t currentByte) {
  switch (state) {
  case WAIT_SYNC:
    if (currentByte == SYNC_BYTE) {
      state = READ_LEN;
      pos = 0;
    }
    break;
  case READ_LEN:
    expected = currentByte;
    if (expected < MIN_PACKET_SIZE || expected > MAX_PACKET_SIZE)
      state = WAIT_SYNC;
    else
      state = COLLECT;
    break;
  case COLLECT:
    buf[pos++] = currentByte;

    if (pos == expected) {
      state = WAIT_SYNC;

      if (crc8(buf, expected - 1) != buf[expected - 1]) // CRC non valido
        return false;

      if (buf[0] != RC_PACKET_TYPE) // Tipo di pacchetto non valido (non è
                                         // un pacchetto RC)
        return false;

      for (int i = 0; i < MAX_CHANNELS; i++)
        channels[i] = readChannel(buf + 1, i);

      return true;
    }
  }

  return false;
}