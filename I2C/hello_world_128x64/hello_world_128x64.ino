//
// Minimal OLED demo
//
// Requires SSD1306 OLED display connected via I2C
//
// The I2CBegin method, when used with only the display type, assumes
// that your Arduino target board definition correctly defines the default
// I2C (SDA and SCL) pins to match your target board. For example,
// if using the Seeed Xiao ESP32-S3, you must choose "XIAO_ESP32S3" as the target
// and not "ESP32S3 Dev Module". The named target configures the correct default
// pin definitions.
//
#include <OneBitDisplay.h>
ONE_BIT_DISPLAY obd; // static instance of the class; this can also be allocated dynamically

void setup() {
  obd.I2Cbegin(OLED_128x64); // initialize the Wire library on the default I2C pins for a SSD1306 128x64 OLED
  obd.fillScreen(OBD_WHITE); // The colors are WHITE for all 0's and BLACK for all 1's. This references mono LCDs and e-paper. OLEDs display inverted colors. 
  obd.setFont(FONT_12x16); // There are 5 font sizes built in to the library: 6x8, 8x8, 12x16, 16x16 and 16x32
  obd.println("Hello"); // On most Arduino targets, println and printf are supported
  obd.println("WORLD!"); // some targets (e.g. Mbed) are missing printf
}

void loop() {
// nothing to do
}
