/*
  uint32_t SX126xGetRandom(void) {
  uint8_t buf[] = {0, 0, 0, 0};
  SX126xReadRegisters(RANDOM_NUMBER_GENERATORBASEADDR, buf, 4);
  return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
  }
*/
#define RANDOM_NUMBER_GENERATORBASEADDR 0x0819
#define REG_ANA_LNA 0x08E2
#define REG_ANA_MIXER 0x08E5

void fillRandom() {
  // randomStock
  uint32_t number = 0;
  uint8_t regAnaLna = 0, regAnaMixer = 0, cnt = 0;
  regAnaLna = Radio.Read(REG_ANA_LNA);
  Radio.Write(REG_ANA_LNA, regAnaLna & ~(1 << 0));
  regAnaMixer = Radio.Read(REG_ANA_MIXER);
  Radio.Write(REG_ANA_MIXER, regAnaMixer & ~(1 << 7));
  // Set radio in continuous reception
  Radio.Rx(0xFFFFFF);
  // SX126xSetRx(0xFFFFFF); // Rx Continuous
  for (uint8_t i = 0; i < 32; i++) {
    Radio.ReadBuffer(RANDOM_NUMBER_GENERATORBASEADDR, (uint8_t*)&number, 4);
    Serial.println("number 0x" + String(number, HEX));
    memcpy(randomStock + cnt, (uint8_t*)&number, 4);
    cnt += 4;
  }
  Radio.Standby();
  Radio.Write(REG_ANA_LNA, regAnaLna);
  Radio.Write(REG_ANA_MIXER, regAnaMixer);
}

#include "Adafruit_nRFCrypto.h"
#include <Adafruit_TinyUSB.h> // for Serial

void fillnRFrandom() {
  nRFCrypto.begin();
  nRFCrypto.Random.generate(randomStock, 256);
  nRFCrypto.end();
}
