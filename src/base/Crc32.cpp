#include "../base/Crc32.h"


namespace Game {
Crc32::Crc32(uint32_t polynomial, uint32_t seed) :
  _uiPolynomial(polynomial)
  , _uiSeed(seed) {
  initializeTable(_uiPolynomial, _uiTable, UiTableLen);
}
Crc32::~Crc32() {
}
uint32_t Crc32::compute(const t_string& buffer) {
  uint32_t hash = ~calculateHash(_uiTable, _uiSeed, buffer.c_str(), 0, buffer.length());

  return hash;
}
void Crc32::initializeTable(uint32_t polynomial, uint32_t* createTable, uint32_t tableLen) {
  for (uint32_t i = 0; i < tableLen; i++) {
    uint32_t entry = i;

    for (uint32_t j = 0; j < 8; j++) {
      if ((entry & 1) == 1) {
        entry = (entry >> 1) ^ polynomial;
      }
      else {
        entry = entry >> 1;
      }
    }
    createTable[i] = entry;
  }
}
uint32_t Crc32::calculateHash(uint32_t* table, uint32_t seed, const char* buffer, int start, int size) {
  uint32_t crc = seed;
  for (int i = start; i < size - start; i++) {
    unsigned char idx = (unsigned char)buffer[i]; //Make sure this lnes up with C#

    crc = (crc >> 8) ^ table[idx ^ crc & 0xff];
  }
  return crc;
}







}//ns Game
