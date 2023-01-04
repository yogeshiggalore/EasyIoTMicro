/*******************************************************************************
*File Name: ESPUtils.c
*
* Version: 1.0
*
* Description:
* In this source for utility functions
*
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , Lattech
*******************************************************************************/

#ifndef ESPUtils_h
#define ESPUtils_h

#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

class ESPUtils{
public:
    ESPUtils();
    String sMACAddress;
    String sDeviceName;
    String sHostNameWifi;
    long lMACAddress;
    uint8_t ui8DebugEnable;
    void  Start(void);
    String Get_MAC(void);
    String Get_Device_Name(void);
    void Set_Host_Name(void);
    uint8_t Get_Serial_Debug(void);
    String GetTime(void);
    time_t get_epoch_time(void);
    int Get_Day(void);
    int Get_Hours(void);
    int Get_Minutes(void);
    int Get_Seconds(void);
    int Get_Date(void);
    int Get_Month(void);
    int Get_Year(void);
    byte DecToBcd(byte bValue);
    byte BcdToDec(byte val);
    uint8_t Convert_ASCIIHex_To_Hex(char sASCIIHex[]);
    void Disable_Serial_Debug(void);
    void Enable_Serial_Debug(void);
    void Parse_String_To_Bytes(const char* str, char sep, uint8_t* bytes, int maxBytes, int base);

private:
    WiFiUDP ntpUDP;
    NTPClient timeClient;
};


#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
extern ESPUtils EUtils;
#endif
#endif
