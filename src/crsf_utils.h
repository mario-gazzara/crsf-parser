#pragma once
#include <cstdint>

namespace crsf {
uint16_t readChannel(const uint8_t *data, int i);
uint8_t crc8(const uint8_t *data, int len);
} // namespace crsf
