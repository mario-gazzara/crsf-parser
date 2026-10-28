#include "crsf_utils.h"

namespace crsf {
uint16_t readChannel(const uint8_t *data, int i) {
  // Il primo bit da cui si parte
  int bitOffset = i * 11;

  // Dividendo per 8 e prendendo la parte intera, si ha il byte di partenza
  int sbIdx = bitOffset / 8;

  // Prendendo il resto, invece si ha il bit da cui si parte all'interno del
  // byte di partenza
  int offset = bitOffset % 8;

  // Un canale di 11 bit cade al massimo in 3 byte consecutivi, quindi
  // prendiamo sempre 3 byte a partire da quello in cui comincia il canale.
  // Il primo della fila va nei bit bassi, perché nel pacchetto il primo
  // bit che arriva è il meno significativo.
  //
  //   bit:  31     24 23     16 15      8 7       0
  //         00000000 CCCCCCCC BBBBBBBB AAAAAAAA
  //                  data[+2] data[+1] data[sbIdx]
  //
  // A = data[sbIdx] nei bit 0-7, B = data[sbIdx + 1] nei bit 8-15,
  // C = data[sbIdx + 2] nei bit 16-23. I bit 24-31 restano a zero.
  uint32_t threeBytesUnion =
      (uint32_t)data[sbIdx] | ((uint32_t)data[sbIdx + 1] << 8);

  // Il terzo byte serve solo se il canale non sta tutto nei primi due,
  // cioè se 16 - offset < 11, ovvero offset > 5.
  if (offset > 5)
    threeBytesUnion |= (uint32_t)data[sbIdx + 2] << 16;

  // Spostiamo a destra di "offset" bit, così il canale parte dal bit 0.
  // I bit più bassi, che appartengono al canale precedente, escono fuori.
  // Poi la maschera 0x7FF (undici 1 di fila) tiene solo gli 11 bit bassi
  // e taglia quelli del canale successivo.
  //
  // Esempio con il canale 1, offset = 3:
  //
  //   prima:        00000000 CCCCCCCC BBBBBBBB AAAAAAAA
  //   >> 3:         00000000 000CCCCC CCCBBBBB BBBAAAAA   (escono 3 bit di A)
  //   & 0x7FF:      00000000 00000000 00000111 11111111
  //   value:        00000000 00000000 00000BBB BBBAAAAA
  //
  // value contiene i 5 bit alti di A e i 6 bit bassi di B, cioè il canale 1.
  uint32_t value = (threeBytesUnion >> offset) & 0x7FF;

  return value;
}

uint8_t crc8(const uint8_t *data, int len) {
  uint8_t crc = 0;
  for (int k = 0; k < len; k++) {
    crc ^= data[k]; // porta dentro il byte successivo
    for (int b = 0; b < 8; b++) {
      if (crc & 0x80)            // il bit in testa è 1, il divisore ci sta
        crc = (crc << 1) ^ 0xD5; // sposta e sottrai il divisore
      else
        crc = crc << 1; // il divisore non ci sta, sposta e basta
    }
  }
  return crc;
}

} // namespace crsf
