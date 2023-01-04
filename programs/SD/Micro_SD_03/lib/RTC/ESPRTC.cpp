/*******************************************************************************
*File Name: ESPRTC.cpp
*
* Version: 1.0
*
* Description:
* In this source code for managing rtc
*
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoT
*******************************************************************************/
#include <Arduino.h>
#include <ESPRTC.h>
#include <Wire.h>

// Constructors ////////////////////////////////////////////////////////////////
ESPRTC::ESPRTC(){}

void ESPRTC::Start(void){

    Wire.begin();

    if (! isrunning()) {
        Serial.println("RTC is NOT running!");
        ui8RTCStatus = RTC_RUN_ERROR;
    }else{
        ui8RTCStatus = RTC_RUN_OK;
        Serial.println("RTC is running!");
    }

    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}


uint8_t ESPRTC::isrunning(void) {
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();

  Wire.requestFrom(DS3231_I2C_ADDRESS, 1);
  uint8_t ss = Wire.read();
  return !(ss >> 7);
}

String ESPRTC::Read(void){
    String sDateTime;

    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    /* set DS3231 register pointer to 00h */
    Wire.write(0); 
    Wire.endTransmission();
    Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
    
    /* request seven bytes of data from DS3231 starting from register 00h */
    ui8Second     = BcdToDec(Wire.read() & 0x7f);
    ui8Minute     = BcdToDec(Wire.read());
    ui8Hour       = BcdToDec(Wire.read() & 0x3f);
    ui8DayOfWeek  = BcdToDec(Wire.read()) -1;
    ui8DayOfMonth = BcdToDec(Wire.read());
    ui8Month      = BcdToDec(Wire.read());
    ui8Year       = BcdToDec(Wire.read());

    sDateTime = String(ui8Year);
    if(ui8Month < 10){
        sDateTime = sDateTime + "0" + String(ui8Month);
    }else{
        sDateTime = sDateTime + String(ui8Month);
    }

    if(ui8DayOfMonth < 10){
        sDateTime = sDateTime + "0" + String(ui8DayOfMonth);
    }else{
        sDateTime = sDateTime + String(ui8DayOfMonth);
    }

    if(ui8Hour < 10){
        sDateTime = sDateTime + "0" + String(ui8Hour);
    }else{
        sDateTime = sDateTime + String(ui8Hour);
    }

    if(ui8Minute < 10){
        sDateTime = sDateTime + "0" + String(ui8Minute);
    }else{
        sDateTime = sDateTime + String(ui8Minute);
    }

    if(ui8Second < 10){
        sDateTime = sDateTime + "0" + String(ui8Second);
    }else{
        sDateTime = sDateTime + String(ui8Second);
    }

    return sDateTime;
}

String ESPRTC::get_rtc_in_datetime_format(void){
    String sDate;
    uint16_t ui16Year=0;

    Read();
    ui16Year = 2000 + ui8Year;
    sDate = String(ui8DayOfMonth) + "/" + String(ui8Month) + "/" + String(ui16Year) + " " + String(ui8Hour) + ":" + String(ui8Minute) + ":" + String(ui8Second);

    return sDate;
}

void ESPRTC::Write(void){
    /* sets time and date data to DS3231 */
    Wire.beginTransmission(DS3231_I2C_ADDRESS);

    /* set next input to start at the seconds register */
    Wire.write(0);
    
    Wire.write(DecToBcd(ui8Second)); 
    Wire.write(DecToBcd(ui8Minute));
    Wire.write(DecToBcd(ui8Hour));
    Wire.write(DecToBcd(ui8DayOfWeek));
    Wire.write(DecToBcd(ui8DayOfMonth));
    Wire.write(DecToBcd(ui8Month));
    Wire.write(DecToBcd(ui8Year));

    Wire.endTransmission();
}

byte ESPRTC::DecToBcd(byte val){
    return( (val/10*16) + (val%10) );
}

byte ESPRTC::BcdToDec(byte val){
    return( (val/16*10) + (val%16) );
}

void ESPRTC::handleTiming(void){
    Read();    
}

void ESPRTC::set_time(uint8_t h, uint8_t m, uint8_t s, uint8_t d, uint8_t M, uint8_t y){
    ui8Hour = h;
    ui8Minute = m;
    ui8Second = s;

    ui8Month = M;
    ui8DayOfMonth = d;
    ui8Year = y;

    Write();
}

// Preinstantiate Objects //////////////////////////////////////////////////////
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
ESPRTC ERTC;
#endif