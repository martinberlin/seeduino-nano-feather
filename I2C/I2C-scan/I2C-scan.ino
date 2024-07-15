#include <Wire.h>
#include <OneBitDisplay.h>
ONE_BIT_DISPLAY obd; // static instance of the class; this can also be allocated dynamically

TwoWire *wi = &Wire;

void setup()
{
  Serial.begin(115200);  // start serial for output
  while ( !Serial ) delay(10);   // for nrf52840 with native usb
  obd.I2Cbegin(OLED_128x64); // initialize the Wire library on the default I2C pins for a SSD1306 128x64 OLED
  obd.fillScreen(OBD_WHITE); // The colors are WHITE for all 0's and BLACK for all 1's. This references mono LCDs and e-paper. OLEDs display inverted colors. 
  obd.setFont(FONT_12x16);   // There are 5 font sizes built in to the library: 6x8, 8x8, 12x16, 16x16 and 16x32
  obd.println("I2C Scan");   // On most Arduino targets, println and printf are supported

  wi->begin();        // join i2c bus (address optional for main)
}

void loop()
{
  Serial.println("Scanning address from 0 to 127");
  for (int addr = 1; addr < 128; addr++)
  {
    wi->beginTransmission(addr);
    if ( 0 == wi->endTransmission() )
    {
      obd.printf("Found %x\n", addr); // Print address in Hexa, check C printf documentation
      Serial.print("Found: 0x");
      Serial.print(addr, HEX);
      Serial.println();
    }
  }
  delay(5000);

  obd.fillScreen(OBD_WHITE); // Clean display
}
