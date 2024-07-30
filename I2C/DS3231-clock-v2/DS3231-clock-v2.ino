#include <Wire.h>
#include <OneBitDisplay.h>
#include <bb_rtc.h>  // RTC library supporting various RTC including DS3231 & RV
BBRTC rtc;
ONE_BIT_DISPLAY obd; // static instance of the class; this can also be allocated dynamically

TwoWire *wi = &Wire;
// RTC time struct
struct tm myTime;
// In order to avoid getting time from outside (BLE or WiFi) we set it up initially here:
#define SET_TIME false
// IMPORTANT: Flash it again to false when time is already set. Otherwise it will setup time every time power is cut!

int tm_hour = 16;
int tm_min  = 27;
int tm_year = 2024;
int tm_mon  = 7;  // July
int tm_mday = 16; // Day of the month

char * month[13] = {(char *)"", (char *)"Jan", (char *)"Feb", (char *)"March", (char *)"April", (char *)"May", (char *)"June",
                    (char *)"July", (char *)"Aug", (char *)"Sept", (char *)"Oct", (char *)"Nov", (char *)"Dec"};

void setup()
{
  Serial.begin(115200);  // start serial for output

  rtc.init(); // initialize the RTC that's found (amongst 3 supported) on the default I2C pins for this target board

  myTime.tm_sec = 0;
  myTime.tm_hour = tm_hour;
  myTime.tm_min = tm_min;
  myTime.tm_year = tm_year;
  myTime.tm_mon = tm_mon;
  myTime.tm_mday = tm_mday;
  #if SET_TIME == true
  rtc.setTime(&myTime);
  #endif
  obd.I2Cbegin(OLED_128x64); // initialize the Wire library on the default I2C pins for a SSD1306 128x64 OLED
  obd.fillScreen(OBD_WHITE); // The colors are WHITE for all 0's and BLACK for all 1's. This references mono LCDs and e-paper. OLEDs display inverted colors. 
  obd.setFont(FONT_12x16);   // There are 5 font sizes built in to the library: 6x8, 8x8, 12x16, 16x16 and 16x32
  obd.println("Clock demo");   // On most Arduino targets, println and printf are supported
  obd.setRotation(180);
  wi->begin();        // join i2c bus (address optional for main)

    Serial.println("Scanning address from 0 to 127");
  for (int addr = 1; addr < 128; addr++)
  {
    wi->beginTransmission(addr);
    if ( 0 == wi->endTransmission() )
    {
      obd.printf("Found %x\n", addr); // Print address in Hexa, check C printf documentation
      if (addr = 0x68) {
          obd.println("DS3231 OK");
          }
      Serial.print("Found: 0x");      
      Serial.print(addr, HEX);
      Serial.println();
    }
  }
  delay(1000);
  obd.fillScreen(OBD_WHITE); // Clean display
}

void loop()
{
  char szDate[14];
  char szTemp[32];
  int color;
  rtc.getTime(&myTime); // Read the current time from the RTC into our time structure
  sprintf(szTemp, "%02d:%02d", myTime.tm_hour, myTime.tm_min);
  sprintf(szDate, "%02d %s", myTime.tm_mday, month[myTime.tm_mon]); //myTime.tm_year-100
  obd.setCursor(1, 15);
  obd.setFont(FONT_12x16);
  obd.print(szDate);
  obd.setCursor(1, 47);
  obd.setFont(FONT_16x32);
  obd.print(szTemp);
  if (myTime.tm_min % 2 ==0) {
    color = OBD_WHITE;
  } else {
    color = OBD_BLACK;
  }
  obd.drawLine(0, 30, myTime.tm_sec*2, 30, color);
  obd.drawLine(0, 31, myTime.tm_sec*2, 31, color);
  delay(2000); // But you could also deepsleep 5 seconds or whatever since the RTC IC holds the time for you
  obd.drawLine(0, 30, 128, 30, OBD_WHITE);
  obd.drawLine(0, 31, 128, 31, OBD_WHITE);
  obd.displayLines(30,2);
  //obd.fillScreen(OBD_WHITE); // No need to clean display here, otherwise it will flicker!
}
