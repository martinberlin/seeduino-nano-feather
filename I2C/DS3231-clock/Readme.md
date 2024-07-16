### I2C Clock demo

![Seeed I2C scan](/assets/DS3231-clock.jpg)

The objective of this example is to show an external RTC at work. Simply setting time in setup() and defining a variable that will be used to set it only once:

#define SET_TIME false --> Need to be set to true in the initial flash along with the right time. 

A more advanced version will save that flag to know the time is set in the Non Volatile Storage or NVS then it could be flashed only one time. 
Provided you use an MCU with WiFi such as any Espressif MCU then you could test a more advanced example like you can see in the BB RTC repository.

https://github.com/martinberlin/seeduino-nano-feather/blob/xx/I2C/DS3231-clock/DS3231-clock.ino#L1-L75

## Requirements

Any compatible board with an I2C display.   

For this example we used:   Seeed XIAO nRF52840

**Libraries:**  

- OneBitDisplay ([Repository link](https://github.com/bitbank2/OneBitDisplay))
- bb_rtc Latest version to handle RTC
