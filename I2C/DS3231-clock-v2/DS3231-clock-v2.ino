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

#define SLEEP_TIME 5        // Sleep time [sec]

void printTime() {
  
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
  
  delay(100); // But you could also deepsleep 5 seconds or whatever since the RTC IC holds the time for you
  obd.drawLine(0, 30, 128, 30, OBD_WHITE);
  obd.drawLine(0, 31, 128, 31, OBD_WHITE);
  obd.displayLines(30,2);
}
void setup()
{
  Serial.begin(115200);  // start serial for output
  digitalWrite(LED_RED, LOW);
  rtc.init(); // initialize the RTC that's found (amongst 3 supported) on the default I2C pins for this target board
  // Internal RTC initialization to wake up
  initRTC(32768 * SLEEP_TIME);    // SLEEP_MTIME [sec]  

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
  // Contrast - set to medium to consume less:
  obd.writeCommand(0x81);
  obd.writeCommand(100); // 10 -> 255 max
  
  obd.fillScreen(OBD_WHITE); // The colors are WHITE for all 0's and BLACK for all 1's. This references mono LCDs and e-paper. OLEDs display inverted colors. 
  // Lower brightness?
  obd.setFont(FONT_12x16);   // There are 5 font sizes built in to the library: 6x8, 8x8, 12x16, 16x16 and 16x32
  obd.println("Clock demo");   // On most Arduino targets, println and printf are supported
  obd.setRotation(180);
  delay(400);
  obd.fillScreen(OBD_WHITE); // Clean display
  Serial.end();
  printTime();

  sd_power_mode_set(NRF_POWER_MODE_LOWPWR);
  sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);    // This saves power
}

void loop()
{
  
  digitalWrite(LED_RED, LOW);
  // Enter System ON sleep mode
  __WFI();
  // Make sure any pending events are cleared
  __SEV();
  __WFI();
		// For more information on the WFE - SEV - WFE sequence, please refer to the following Devzone article:
		// https://devzone.nordicsemi.com/index.php/how-do-you-put-the-nrf51822-chip-to-sleep#reply-1589
}


//**************************************************************************************************************
// RTC initialization
void initRTC(unsigned long count30) 
{
  // See "6.22 RTC - Real-time counter 6.22.10 Registers"
  NRF_CLOCK->TASKS_LFCLKSTOP = 1;
  NRF_CLOCK->LFCLKSRC = 1;      // select LFXO
  NRF_CLOCK->TASKS_LFCLKSTART = 1;
  while(NRF_CLOCK->LFCLKSTAT != 0x10001)
  
  NRF_RTC2->TASKS_STOP = 1;      // stop counter 
  NRF_RTC2->TASKS_CLEAR = 1;     // clear counter
  NRF_RTC2->PRESCALER = 0;       // set counter prescaler, fout=32768/(1+PRESCALER)　32768Hz
  NRF_RTC2->CC[0] = count30;     // set value for TRC compare register 0
  NRF_RTC2->INTENSET = 0x10000;  // enable interrupt CC[0] compare match event
  NRF_RTC2->EVTENCLR = 0x10000;  // clear counter when CC[0] event
  NVIC_SetPriority(RTC2_IRQn,3); // Higher priority than SoftDeviceAPI
  NVIC_EnableIRQ(RTC2_IRQn);     // enable interrupt  
  NRF_RTC2->TASKS_START = 1;     // start Timer
}

//**************************************************************************************************************
// RTC interrupt handler
extern "C" void RTC2_IRQHandler(void)
{
  if ((NRF_RTC2->EVENTS_COMPARE[0] != 0) && ((NRF_RTC2->INTENSET & 0x10000) != 0)) {
    printTime();
    NRF_RTC2->EVENTS_COMPARE[0] = 0;  // clear compare register 0 event
    NRF_RTC2->TASKS_CLEAR = 1;        // clear counter
    digitalWrite(LED_RED, HIGH);
    delayMicroseconds(1000);      // for LED
  }
}