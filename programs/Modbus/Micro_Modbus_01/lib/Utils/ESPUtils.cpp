/*******************************************************************************
*File Name: ESPUtils.h
*
* Version: 1.0
*
* Description:
* In this header for utility functions
*
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , Lattech
*******************************************************************************/
#include <ESPUtils.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <WiFi.h>

ESPUtils::ESPUtils(void):ntpUDP(),timeClient(ntpUDP, "in.pool.ntp.org", 19800, 60000){
}

void ESPUtils::Start(void){
    timeClient.begin();
}

time_t ESPUtils::get_epoch_time(void){
    timeClient.update();
    return timeClient.getEpochTime();
}

String ESPUtils::GetTime(void){
    timeClient.update();

    unsigned long rawTime = timeClient.getEpochTime();

    unsigned long hours = (rawTime % 86400L) / 3600;
    String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

    unsigned long minutes = (rawTime % 3600) / 60;
    String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

    unsigned long seconds = rawTime % 60;
    String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);

    String dateStr = String(Get_Date());

    String monthStr = String(Get_Month());

    String yearStr = String(Get_Year());

    return hoursStr + ":" + minuteStr + ":" + secondStr + " " + dateStr + "-" + monthStr + "-" + yearStr;
}

int ESPUtils::Get_Day(void) {
  return (((timeClient.getEpochTime()  / 86400L) + 4 ) % 7); //0 is Sunday
}

int ESPUtils::Get_Hours(void) {
  return ((timeClient.getEpochTime()  % 86400L) / 3600);
}
int ESPUtils::Get_Minutes(void) {
  return ((timeClient.getEpochTime() % 3600) / 60);
}
int ESPUtils::Get_Seconds(void) {
  return (timeClient.getEpochTime() % 60);
}

int ESPUtils::Get_Year(void) {
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);
  int year = ti->tm_year + 1900;

  return year;
}

int ESPUtils::Get_Month(void) {
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);
  int month = (ti->tm_mon + 1) < 10 ? 0 + (ti->tm_mon + 1) : (ti->tm_mon + 1);

  return month;
}

int ESPUtils::Get_Date(void) {
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);
  int month = (ti->tm_mday) < 10 ? 0 + (ti->tm_mday) : (ti->tm_mday);

  return month;
}

String ESPUtils::Get_MAC(void){
  uint8_t ui8LoopCounter=0;
  uint8_t ui8Length=0;

  sMACAddress = WiFi.macAddress();
  ui8Length = sMACAddress.length();
  for(ui8LoopCounter=0;ui8LoopCounter<ui8Length;ui8LoopCounter++){
    if(sMACAddress[ui8LoopCounter] == ':'){
      sMACAddress.remove(ui8LoopCounter,1);
    }
  }
  return sMACAddress;
}

byte ESPUtils::DecToBcd(byte bValue){
  return( (bValue/10*16) + (bValue%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte ESPUtils::BcdToDec(byte val){
  return( (val/16*10) + (val%16) );
}

String ESPUtils::Get_Device_Name(void){
  sDeviceName = "DW01" + Get_MAC();
  return sDeviceName;
}

void ESPUtils::Set_Host_Name(void){
  sHostNameWifi = Get_Device_Name();
  sHostNameWifi.concat(".local");
  WiFi.setHostname(sHostNameWifi.c_str());
}

/******************************************************************************
* Function Name: Convert_ASCIIHex_To_Hex
*******************************************************************************
*
* Summary:
*  This function convert the asciihex to hex
*
*
* Parameters:
*  ascii hex string
*
* Return:
* returns hex
*
*******************************************************************************/
uint8_t ESPUtils::Convert_ASCIIHex_To_Hex(char sASCIIHex[]){
    char cCharLowerNibble=0;
    char cCharHigherNibble=0;
    uint8_t ui8HexByte=0;

    cCharHigherNibble = sASCIIHex[0];
    cCharLowerNibble = sASCIIHex[1];

    if(cCharHigherNibble > '9'){
        cCharHigherNibble = cCharHigherNibble - 7;
    }
    cCharHigherNibble = cCharHigherNibble - '0';

    ui8HexByte = cCharHigherNibble;
    ui8HexByte = ui8HexByte << 4;

    if(cCharLowerNibble > '9'){
        cCharLowerNibble = cCharLowerNibble - 7;
    }
    cCharLowerNibble = cCharLowerNibble - '0';

    ui8HexByte = ui8HexByte | cCharLowerNibble;

    return ui8HexByte;
}

void ESPUtils::Parse_String_To_Bytes(const char* str, char sep, uint8_t* bytes, int maxBytes, int base) {
    for (int i = 0; i < maxBytes; i++) {
        bytes[i] = strtoul(str, NULL, base);  // Convert byte
        str = strchr(str, sep);               // Find next separator
        if (str == NULL || *str == '\0') {
            break;                            // No more separators, exit
        }
        str++;                                // Point to next character after separator
    }
}


void ESPUtils::Enable_Serial_Debug(void){
    ui8DebugEnable = 1;
}

void ESPUtils::Disable_Serial_Debug(void){
    ui8DebugEnable = 0;
}

uint8_t ESPUtils::Get_Serial_Debug(void){
    return ui8DebugEnable;
}
// Preinstantiate Objects //////////////////////////////////////////////////////

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
ESPUtils EUtils;
#endif
