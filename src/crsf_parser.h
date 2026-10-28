#pragma once
#include <cstdint>

class CrsfParser {
public:
  static constexpr int MAX_PACKET_SIZE = 64;
  static constexpr int MAX_CHANNELS = 16;

  bool feed(uint8_t currentByte);
  uint16_t channel(int i) const { return channels[i]; }

private:
  static constexpr uint8_t SYNC_BYTE = 0xC8;
  static constexpr uint8_t RC_PACKET_TYPE = 0x16;
  static constexpr int MIN_PACKET_SIZE = 2;

  enum State { WAIT_SYNC, READ_LEN, COLLECT };

  State state = WAIT_SYNC;
  uint8_t buf[MAX_PACKET_SIZE];
  int pos = 0;
  int expected = 0;
  uint16_t channels[MAX_CHANNELS] = {0};
};
