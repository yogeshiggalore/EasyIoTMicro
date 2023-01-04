/*******************************************************************************
*File Name: ESPRTC.h
*
* Version: 1.0
*
* Description:
* In this header used for managing RTC
*
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoT
*******************************************************************************/
#ifndef ESPRTC_h
#define ESPRTC_h
#include <Arduino.h>

#define DS3231_I2C_ADDRESS 0x68

#define RTC_NONE        0
#define RTC_START_ERROR 1
#define RTC_START_OK    2
#define RTC_RUN_OK      3
#define RTC_RUN_ERROR   4

class ESPRTC{
public:
    ESPRTC();
    uint8_t ui8RTCStatus;
    uint8_t ui8Year;
    uint8_t ui8Month;
    uint8_t ui8DayOfMonth;
    uint8_t ui8Hour;
    uint8_t ui8Minute;
    uint8_t ui8Second;
    uint8_t ui8DayOfWeek;

    char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    
    void Start(void);
    String Read(void);
    void Write(void);
    void handleTiming(void);
    byte DecToBcd(byte val);
    byte BcdToDec(byte val);
    uint8_t isrunning(void);

    void set_time(uint8_t h, uint8_t m, uint8_t s, uint8_t d, uint8_t M, uint8_t y);
    String get_rtc_in_datetime_format(void);
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
extern ESPRTC ERTC;
#endif
#endif