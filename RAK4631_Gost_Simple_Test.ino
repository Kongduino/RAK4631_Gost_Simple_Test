#include <Arduino.h>
#include <SX126x-RAK4630.h> //http://librarymanager/All#SX126x
#include <SPI.h>
#include "Helper.h"
#include "GOST.h"
#include "Sx1262LoRandom.h"

#ifdef NRF52_SERIES
#define LED_BUILTIN 35
#endif

void GostTest();

// Define LoRa parameters
#define RF_FREQUENCY 868300000 // Hz
#define TX_OUTPUT_POWER 22 // dBm
#define LORA_BANDWIDTH 0 // [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved]
#define LORA_SPREADING_FACTOR 7 // [SF7..SF12]
#define LORA_CODINGRATE 1 // [1: 4/5, 2: 4/6,  3: 4/7,  4: 4/8]
#define LORA_PREAMBLE_LENGTH 8 // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT 0 // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON false
#define RX_TIMEOUT_VALUE 3000
#define TX_TIMEOUT_VALUE 3000

static RadioEvents_t RadioEvents;
static uint8_t TxdBuffer[64];

void setup() {
  // Initialize Serial for debug output
  time_t timeout = micros();
  Serial.begin(115200);
  for (uint8_t i = 5; i > 0; i--) {
    delay(1000);
    Serial.print(i); Serial.print(", ");
  }
  Serial.println("0!");
  Serial.println("=================");
  Serial.println("LoRap2p Tx Test");
  Serial.println("=================");
  // Initialize LoRa chip.
  lora_rak4630_init();
  // Initialize the Radio callbacks
  // We are bot using any here.
  RadioEvents.TxDone = NULL;
  RadioEvents.RxDone = NULL;
  RadioEvents.TxTimeout = NULL;
  RadioEvents.RxTimeout = NULL;
  RadioEvents.RxError = NULL;
  RadioEvents.CadDone = NULL;
  // Initialize the Radio
  Radio.Init(&RadioEvents);
  // Set Radio channel
  Radio.SetChannel(RF_FREQUENCY);
  // Set Radio RX configuration
  Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                    0, true, 0, 0, LORA_IQ_INVERSION_ON, true);
  stockUpRandom();
  Radio.Standby();
}

void loop() {
  GostTest();
  delay(10000);
}

void GostTest() {
  unsigned char plaintext[72];
  unsigned char ciphertext[72];
  unsigned char decrypted[72];
  GOST_CTX ctx;
  unsigned char key[32] = {
    0x4B, 0x9E, 0xA4, 0xD1, 0x1A, 0x32, 0x17, 0xD2,
    0xF3, 0xB1, 0xBD, 0x9C, 0xE9, 0x89, 0xB4, 0x64,
    0x63, 0xAA, 0xE5, 0x9A, 0x26, 0x68, 0xB5, 0x18,
    0x4A, 0xC2, 0xBE, 0x79, 0x71, 0x24, 0x54, 0xAD
  };

  Serial.println("GOST test");
  unsigned char IV[8];
  uint32_t t0, t1;
  getRandomBytes(IV, (uint16_t)8);
  getRandomBytes(plaintext, (uint16_t)72);
  Serial.print("IV ="); for (uint8_t i = 0; i < 8; i++) Serial.printf(" %02x", IV[i]); Serial.print("\n");
  Serial.println("Plaintext:");
  hexDump(plaintext, 72);
  t0 = micros();
  uint16_t finalLength = GOST_Encrypt_Block_ECB(plaintext, 72, ciphertext, key, IV);
  t1 = micros();
  Serial.printf("Encrypted in %d micros\n", (t1 - t0));
  memcpy(IV, ciphertext + finalLength - 8, 8);
  Serial.print("\nIV ="); for (uint8_t i = 0; i < 8; i++) Serial.printf(" %02x", IV[i]); Serial.print("\n");
  t0 = micros();
  finalLength = GOST_Decrypt_Block_ECB(ciphertext, 72, decrypted, key, IV);
  t1 = micros();
  Serial.println("Decrypted:");
  hexDump(decrypted, finalLength);
  Serial.printf("Encrypted in %d micros\n", (t1 - t0));
  if (compareBlocks(decrypted, plaintext, finalLength)) Serial.println("It's a match!");
  else Serial.println("NOT a match!!!");
}
