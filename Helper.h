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

void hexDump(uint8_t* buf, uint16_t len) {
  // Something similar to the Unix/Linux hexdump -C command
  // Pretty-prints the contents of a buffer, 16 bytes a row
  char alphabet[17] = "0123456789abcdef";
  uint16_t i, index;
  Serial.print(F("   +------------------------------------------------+ +----------------+\n"));
  Serial.print(F("   |.0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .a .b .c .d .e .f | |      ASCII     |\n"));
  for (i = 0; i < len; i += 16) {
    if (i % 128 == 0) Serial.print(F("   +------------------------------------------------+ +----------------+\n"));
    char s[] = "|                                                | |                |\n";
    // pre-formated line. We will replace the spaces with text when appropriate.
    uint8_t ix = 1, iy = 52, j;
    for (j = 0; j < 16; j++) {
      if (i + j < len) {
        uint8_t c = buf[i + j];
        // fastest way to convert a byte to its 2-digit hex equivalent
        s[ix++] = alphabet[(c >> 4) & 0x0F];
        s[ix] = alphabet[c & 0x0F];
        ix += 2;
        if (c > 31 && c < 128) s[iy++] = c;
        else s[iy++] = '.'; // display ASCII code 0x20-0x7F or a dot.
      }
    }
    index = i / 16;
    // display line number then the text
    if (i < 256) Serial.write(' ');
    Serial.print(index, HEX); Serial.write('.');
    Serial.print(s);
  }
  Serial.print(F("   +------------------------------------------------+ +----------------+\n"));
}

void getRandomBytes(uint8_t *buff, uint16_t count) {
  // This should be faster than the method commented out below
  uint16_t r = count, index = 0, available = 256 - randomIndex;
  while (r > available) {
    memcpy(buff + index, randomStock + randomIndex, available);
    stockUpRandom(); // randomIndex is reset to 0
    r -= available;
    index += available;
    available = 256;
  }
  if (r == 0) return;
  memcpy(buff + index, randomStock + randomIndex, r);
  // check whether we have used up the stock of random bytes
  if (randomIndex + r == 256) stockUpRandom();
  else randomIndex += r;
  
//   for (uint8_t i = 0; i < count; i++) {
//     buff[i] = randomStock[randomIndex++];
//     // reset random stock automatically if needed
//     if (randomIndex > 254) stockUpRandom();
//   }
}

bool compareBlocks(uint8_t *A, uint8_t *B, uint16_t ln) {
  for (uint8_t i = 0; i < ln; i++) if (A[i] != B[i]) return false;
  return true;
}
