# CRSF Parser

Implementazione in C++ di un parser per il protocollo **CRSF** (Crossfire Serial Protocol).

Il parser serve a ricevere e interpretare i valori dei comandi che la radio
**RadioMaster** invia alla ricevente **LRS** (Long Range System). La ricevente
inoltra i pacchetti CRSF via seriale e questo codice li decodifica, estraendo i
16 canali RC (stick, interruttori, potenziometri) come valori a 11 bit.

## Come funziona

Il parser lavora byte per byte con una macchina a stati:

1. **WAIT_SYNC**: attende il byte di sincronizzazione `0xC8`.
2. **READ_LEN**: legge la lunghezza del pacchetto e la valida (tra 2 e 64 byte).
3. **COLLECT**: accumula i byte del payload. A pacchetto completo verifica il
   CRC8, controlla che il tipo sia `0x16` (pacchetto canali RC) e decodifica
   i 16 canali impacchettati a 11 bit ciascuno.

## Struttura

| File | Contenuto |
|------|-----------|
| `src/crsf_parser.h` / `.cpp` | Classe `CrsfParser` con la macchina a stati e le costanti del protocollo |
| `src/crsf_utils.h` / `.cpp` | Funzioni di supporto: `crc8` e `readChannel` (estrazione dei canali a 11 bit) |

## Uso

```cpp
#include "crsf_parser.h"

CrsfParser parser;

// Per ogni byte ricevuto dalla seriale della ricevente:
if (parser.feed(byte)) {
  // Pacchetto RC valido: i canali sono aggiornati
  uint16_t throttle = parser.channel(2);
}
```

`feed()` restituisce `true` solo quando un pacchetto canali RC completo e con
CRC corretto è stato decodificato. I valori dei canali vanno da 172 a 1811
(con 992 come centro), secondo lo standard CRSF.

## Compilazione

```sh
g++ -std=c++17 -Wall -Wextra -c src/crsf_parser.cpp src/crsf_utils.cpp
```
