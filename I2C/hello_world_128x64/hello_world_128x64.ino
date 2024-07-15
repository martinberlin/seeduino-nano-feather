#include <OneBitDisplay.h>
ONE_BIT_DISPLAY obd;
void setup() {
  obd.I2Cbegin(OLED_128x64);
  obd.fillScreen(OBD_WHITE);
  obd.setFont(FONT_12x16);
  obd.println("Hello");
  obd.println("WORLD!");
}
void loop() {
}