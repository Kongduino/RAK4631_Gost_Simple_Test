void stockUpRandom();
void fillnRFrandom();
void hexDump(unsigned char *, uint16_t);
void getRandomBytes(uint8_t *buff, uint8_t count);

uint8_t randomStock[256];
uint8_t randomIndex = 0;

void stockUpRandom() {
  // fillRandom();
  fillnRFrandom();
  randomIndex = 0;
#ifdef NEED_DEBUG
  if (NEED_DEBUG > 0) {
    Serial.println("* Random Stock");
    hexDump(randomStock, 256);
  }
#endif // NEED_DEBUG
}

void hexDump(unsigned char *buf, uint16_t len) {
  String s = "|", t = "| |";
  Serial.println(F("  |.0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .a .b .c .d .e .f |"));
  Serial.println(F("  +------------------------------------------------+ +----------------+"));
  for (uint16_t i = 0; i < len; i += 16) {
    for (uint8_t j = 0; j < 16; j++) {
      if (i + j >= len) {
        s = s + "   "; t = t + " ";
      } else {
        char c = buf[i + j];
        if (c < 16) s = s + "0";
        s = s + String(c, HEX) + " ";
        if (c < 32 || c > 127) t = t + ".";
        else t = t + (String(c));
      }
    }
    uint8_t index = i / 16;
    Serial.print(index, HEX); Serial.write('.');
    Serial.println(s + t + "|");
    s = "|"; t = "| |";
  }
  Serial.println(F("  +------------------------------------------------+ +----------------+"));
}

void getRandomBytes(uint8_t *buff, uint8_t count) {
  uint8_t r;
  for (uint8_t i = 0; i < count; i++) {
    buff[i] = randomStock[randomIndex++];
    // reset random stock automatically if needed
    if (randomIndex > 254) stockUpRandom();
  }
}

bool compareBlocks(uint8_t *A, uint8_t *B, uint16_t ln) {
  for (uint8_t i = 0; i < ln; i++) if (A[i] != B[i]) return false;
  return true;
}
