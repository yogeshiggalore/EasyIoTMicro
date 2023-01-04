/*******************************************************************************
*File Name: ModbusUtils.cpp
*
* Version: 1.0
*
* Description:
* In this source file for modbus utility application
*
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21)
*******************************************************************************/

#include<Arduino.h>
#include<ModbusUtils.h>
#include <LogLevel.h>
#include <logger.h>

//*************************************************************************************************
//*** CRC look-up table
//*************************************************************************************************
const char TabCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

const char TabCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
    0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
    0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
    0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
    0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
    0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
    0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
    0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
    0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

union
{
    unsigned char Array[2];
    
    struct
    {
        unsigned char LL    :4;
        unsigned char LH    :4;
        unsigned char HL    :4;
        unsigned char HH    :4;
    }NB4;
    
    struct
    {                               // Nibbles x 16 bits
        unsigned char Lo;           // IntL
        unsigned char Hi;           // IntH
    }Byte;
    
    struct
    {                               // BITs
        unsigned b00 :1;
        unsigned b01 :1;
        unsigned b02 :1;
        unsigned b03 :1;
        unsigned b04 :1;
        unsigned b05 :1;
        unsigned b06 :1;
        unsigned b07 :1;
        unsigned b08 :1;
        unsigned b09 :1;
        unsigned b10 :1;
        unsigned b11 :1;
        unsigned b12 :1;
        unsigned b13 :1;
        unsigned b14 :1;
        unsigned b15 :1;
    }Bit;

    signed int INTs;
    unsigned int INTu;
}CRC;

// Constructors ////////////////////////////////////////////////////////////////
ModbusUtils::ModbusUtils(){}

void ModbusUtils::start(uint8_t ui8Mode){
    elog.Write(LogLevel::Info,"ModbusUtils", "setting up modbus utils");
    set_modbus_mode(ui8Mode);
    set_modbus_uart_parameters(MODBUS_UART_2,4,0,8,1);
    
    if(tdfParameter.bPinControl){
        pinMode(MODBUS_UART_CN_PIN,OUTPUT);
        if(tdfParameter.bPinLogic){
            digitalWrite(MODBUS_UART_CN_PIN,HIGH);
        }else{
            digitalWrite(MODBUS_UART_CN_PIN,LOW);
        }
    }

    if(tdfParameter.bLedControl){
        pinMode(MODBUS_UART_LED_PIN,OUTPUT);
        if(tdfParameter.bLedLogic){
            digitalWrite(MODBUS_UART_LED_PIN,HIGH);
        }else{
            digitalWrite(MODBUS_UART_LED_PIN,LOW);
        }
    }
}

void ModbusUtils::set_modbus_uart_parameters(uint8_t ui8UART, uint8_t ui8Baudrate, uint8_t ui8Parity, uint8_t ui8Databits, uint8_t ui8Stopbits){
    uint32_t ui32SerialConfig;
    String sPrintString;

    set_baudrate(ui8Baudrate);
    set_parity(ui8Parity);
    set_databits(ui8Databits);
    set_stopbits(ui8Databits);
    set_time_per_byte(ui8Baudrate);
    tdfParameter.ui8ModbusUART = ui8UART;

    if((ui8Databits == MODBUS_DATA_BITS_5) & (ui8Parity == MODBUS_PARITY_NONE) & (ui8Stopbits == MODBUS_STOP_BIT_1)){
        ui32SerialConfig =   SERIAL_5N1;
    }else if((ui8Databits == MODBUS_DATA_BITS_6) & (ui8Parity == MODBUS_PARITY_NONE) & (ui8Stopbits == MODBUS_STOP_BIT_1)){
        ui32SerialConfig =   SERIAL_6N1;
    }else if((ui8Databits == MODBUS_DATA_BITS_7) & (ui8Parity == MODBUS_PARITY_NONE) & (ui8Stopbits == MODBUS_STOP_BIT_1)){
        ui32SerialConfig =   SERIAL_7N1;
    }else if((ui8Databits == MODBUS_DATA_BITS_8) & (ui8Parity == MODBUS_PARITY_NONE) & (ui8Stopbits == MODBUS_STOP_BIT_1)){
        ui32SerialConfig =   SERIAL_8N1;
    }else if((ui8Databits == MODBUS_DATA_BITS_5) & (ui8Parity == MODBUS_PARITY_NONE) & (ui8Stopbits == MODBUS_STOP_BIT_2)){
        ui32SerialConfig =   SERIAL_5N2;
    }else if((ui8Databits == MODBUS_DATA_BITS_6) & (ui8Parity == MODBUS_PARITY_NONE) & (ui8Stopbits == MODBUS_STOP_BIT_2)){
        ui32SerialConfig =   SERIAL_6N2;
    }else if((ui8Databits == MODBUS_DATA_BITS_7) & (ui8Parity == MODBUS_PARITY_NONE) & (ui8Stopbits == MODBUS_STOP_BIT_2)){
        ui32SerialConfig =   SERIAL_7N2;
    }else if((ui8Databits == MODBUS_DATA_BITS_8) & (ui8Parity == MODBUS_PARITY_NONE) & (ui8Stopbits == MODBUS_STOP_BIT_2)){
        ui32SerialConfig =   SERIAL_8N2;
    }else if((ui8Databits == MODBUS_DATA_BITS_5) & (ui8Parity == MODBUS_PARITY_EVEN) & (ui8Stopbits == MODBUS_STOP_BIT_1)){
        ui32SerialConfig =   SERIAL_5E1;
    }else if((ui8Databits == MODBUS_DATA_BITS_6) & (ui8Parity == MODBUS_PARITY_EVEN) & (ui8Stopbits == MODBUS_STOP_BIT_1)){
        ui32SerialConfig =   SERIAL_6E1;
    }else if((ui8Databits == MODBUS_DATA_BITS_7) & (ui8Parity == MODBUS_PARITY_EVEN) & (ui8Stopbits == MODBUS_STOP_BIT_1)){
        ui32SerialConfig =   SERIAL_7E1;
    }else if((ui8Databits == MODBUS_DATA_BITS_8) & (ui8Parity == MODBUS_PARITY_EVEN) & (ui8Stopbits == MODBUS_STOP_BIT_1)){
        ui32SerialConfig =   SERIAL_8E1;
    }else if((ui8Databits == MODBUS_DATA_BITS_5) & (ui8Parity == MODBUS_PARITY_EVEN) & (ui8Stopbits == MODBUS_STOP_BIT_2)){
        ui32SerialConfig =   SERIAL_5E2;
    }else if((ui8Databits == MODBUS_DATA_BITS_6) & (ui8Parity == MODBUS_PARITY_EVEN) & (ui8Stopbits == MODBUS_STOP_BIT_2)){
        ui32SerialConfig =   SERIAL_6E2;
    }else if((ui8Databits == MODBUS_DATA_BITS_7) & (ui8Parity == MODBUS_PARITY_EVEN) & (ui8Stopbits == MODBUS_STOP_BIT_2)){
        ui32SerialConfig =   SERIAL_7E2;
    }else if((ui8Databits == MODBUS_DATA_BITS_8) & (ui8Parity == MODBUS_PARITY_EVEN) & (ui8Stopbits == MODBUS_STOP_BIT_2)){
        ui32SerialConfig =   SERIAL_8E2;
    }else if((ui8Databits == MODBUS_DATA_BITS_5) & (ui8Parity == MODBUS_PARITY_ODD) & (ui8Stopbits == MODBUS_STOP_BIT_1)){
        ui32SerialConfig =   SERIAL_5O1;
    }else if((ui8Databits == MODBUS_DATA_BITS_6) & (ui8Parity == MODBUS_PARITY_ODD) & (ui8Stopbits == MODBUS_STOP_BIT_1)){
        ui32SerialConfig =   SERIAL_6O1;
    }else if((ui8Databits == MODBUS_DATA_BITS_7) & (ui8Parity == MODBUS_PARITY_ODD) & (ui8Stopbits == MODBUS_STOP_BIT_1)){
        ui32SerialConfig =   SERIAL_7O1;
    }else if((ui8Databits == MODBUS_DATA_BITS_8) & (ui8Parity == MODBUS_PARITY_ODD) & (ui8Stopbits == MODBUS_STOP_BIT_1)){
        ui32SerialConfig =   SERIAL_8O1;
    }else if((ui8Databits == MODBUS_DATA_BITS_5) & (ui8Parity == MODBUS_PARITY_ODD) & (ui8Stopbits == MODBUS_STOP_BIT_2)){
        ui32SerialConfig =   SERIAL_5O2;
    }else if((ui8Databits == MODBUS_DATA_BITS_6) & (ui8Parity == MODBUS_PARITY_ODD) & (ui8Stopbits == MODBUS_STOP_BIT_2)){
        ui32SerialConfig =   SERIAL_6O2;
    }else if((ui8Databits == MODBUS_DATA_BITS_7) & (ui8Parity == MODBUS_PARITY_ODD) & (ui8Stopbits == MODBUS_STOP_BIT_2)){
        ui32SerialConfig =   SERIAL_7O2;
    }else if((ui8Databits == MODBUS_DATA_BITS_8) & (ui8Parity == MODBUS_PARITY_ODD) & (ui8Stopbits == MODBUS_STOP_BIT_2)){
        ui32SerialConfig =   SERIAL_8O2;
    }else{
        ui32SerialConfig =   SERIAL_8N1;
    }

    ModbusUART.begin(tdfParameter.ui32Baudrate,ui32SerialConfig,MODBUS_UART_RX_PIN,MODBUS_UART_TX_PIN);

    sPrintString = "UART: " + String(tdfParameter.ui8ModbusUART) + " baudrate: " + get_baudrate() + " parity: "+ get_parity() + " databit: " + get_databits() + " stopbit: " + get_stopbits();
    elog.Write(LogLevel::Info,"ModbusUtils", sPrintString.c_str());

    #ifdef MODBUS_PIN_CONTROL
        pinMode(MODBUS_UART_CN_PIN,OUTPUT);
        digitalWrite(MODBUS_UART_CN_PIN,HIGH);
    #endif
}

uint8_t ModbusUtils::get_ascii_lrc(uint8_t *MsgSourceAddr, uint16_t MsgLength){
	uint8_t cLRC = 0;

    //it should be rtu bytes not ascii bytes
    while (MsgLength != 0){
		cLRC += *MsgSourceAddr++;
		MsgLength--;
	}
    cLRC = ((cLRC ^ 0xFF) + 1) & 0xFF;

	return cLRC;
}

uint16_t ModbusUtils::get_crc16(uint8_t *MsgStartAddr, uint8_t MsgLength){
    uint16_t ui16Response=0;
    uint8_t Index;

    uint8_t CRCtmpHi = 0xFF;                      // Initialize CRC register to 0xFFFF
    uint8_t CRCtmpLo = 0xFF;

    while(MsgLength--)                                  // Find CRC for each byte of the string
    {
        Index = CRCtmpLo ^ *MsgStartAddr++;
        CRCtmpLo = CRCtmpHi ^ TabCRCHi[Index];
        CRCtmpHi = TabCRCLo[Index];
    }

    ui16Response = CRCtmpLo;
    ui16Response = ui16Response << 8;
    ui16Response = ui16Response | CRCtmpHi;
    CRC.Byte.Hi = CRCtmpHi;                             // Update the CRC registers
    CRC.Byte.Lo = CRCtmpLo;

    return ui16Response;
}

uint8_t ModbusUtils::verify_function_code(uint8_t ui8ReceivedByte){
    uint8_t ui8Response=0;
    uint8_t ui8FunCode=0;

    ui8FunCode = ui8ReceivedByte;
    ui8ReceivedByte = ui8ReceivedByte & 0x1F;

    switch(ui8ReceivedByte){
        case MODBUS_FUNC_READ_COIL_REG:
            ui8Response = 1;
            break;
        case MODBUS_FUNC_READ_INPUT_STATUS_REG:
            ui8Response = 1;
            break;
        case MODBUS_FUNC_READ_HOLDING_REG:
            ui8Response = 1;
            break;
        case MODBUS_FUNC_READ_INPUT_REG:
            ui8Response = 1;
            break;
        case MODBUS_FUNC_WRITE_SINGLE_COIL:
            ui8Response = 1;
            break;
        case MODBUS_FUNC_WRITE_SINGLE_REG:
            ui8Response = 1;
            break;
        case MODBUS_FUNC_WRITE_MULTI_COIL:
            ui8Response = 1;
            break;
        case MODBUS_FUNC_WRITE_MULTI_REG:
            ui8Response = 1;
            break;
        default:
            ui8Response = 0;
            break;
    }

    /* proper funcode error */
    if(ui8Response == 1){
        if((ui8FunCode >> 4) == 0x8){
            ui8Response = 2;
        }else if((ui8FunCode >> 4) == 0x9){
            ui8Response = 2;
        }else if((ui8FunCode >> 4) == 0x1){
            ui8Response = 1;
        }else{
            if((ui8FunCode >> 4) != 0x0){
                ui8Response = 0;
            }
        }
    }

    return ui8Response;
}

uint8_t ModbusUtils::get_exception(uint8_t ui8Byte){

    if(ui8Byte == MODBUS_ILLEGAL_FUN_CODE)
    {
        return MODBUS_INVALID_FUNC;    
    }
    else if(ui8Byte == MODBUS_ILLEGAL_ADDRESS_CODE)
    {
        return MODBUS_INVALID_REG_ADDRESS;
    }
    else if(ui8Byte == MODBUS_ILLEGAL_VALUE_CODE)
    {
        return MODBUS_INVALID_RESPONSE_LEN;
    }
    else if(ui8Byte == MODBUS_SLAVE_FAILURE)
    {
        return MODBUS_ERR_UNKNOWN;
    }
    else if(ui8Byte == MODBUS_ACKNOWLEDGE)
    {
        return MODBUS_ERR_UNKNOWN;
    }
    else if(ui8Byte == MODBUS_NAK_ACKNOWLEDGE)
    {
        return MODBUS_ERR_UNKNOWN;
    }
    else if(ui8Byte == MODBUS_ILLEGAL_VALUE_CODE)
    {
        return MODBUS_INVALID_DATA_VALUE;
    }
    else if(ui8Byte == MODBUS_MEMORY_ERROR)
    {
        return MODBUS_GATEWAY_FAILED;
    }
    else if(ui8Byte == MODBUS_GATEWAY_ERROR)
    {
        return MODBUS_GATEWAY_FAILED;
    }
    else if(ui8Byte == MODBUS_GATEWAY_FAILURE)
    {
        return MODBUS_GATEWAY_FAILED;
    }
    else
    {
        return MODBUS_ERR_UNKNOWN;
    }
}

uint8_t ModbusUtils::expected_bytes_rtu(uint8_t aui8Buffer[]){
    uint8_t ui8Response=0;
    uint16_t ui16NumberOfRegister=0;
    uint8_t  ui8FunctionCode=0;
    
    ui16NumberOfRegister = aui8Buffer[4];
    ui16NumberOfRegister = ui16NumberOfRegister << 8;
    ui16NumberOfRegister = ui16NumberOfRegister | aui8Buffer[5];
    
    ui8FunctionCode = aui8Buffer[1];
    
    
    if((ui8FunctionCode == MODBUS_FUNC_READ_COIL_REG) || (ui8FunctionCode == MODBUS_FUNC_READ_INPUT_STATUS_REG)){
        ui8Response = ui16NumberOfRegister/8;
        ui8Response = MODBUS_RTU_MANDATORY_RESPONSE_BYTES + ui8Response;
    }
    
    else if((ui8FunctionCode == MODBUS_FUNC_READ_HOLDING_REG) || (ui8FunctionCode == MODBUS_FUNC_READ_INPUT_REG)){
        ui8Response = MODBUS_RTU_MANDATORY_RESPONSE_BYTES + (ui16NumberOfRegister * 2);
    }
    else{
        ui8Response = 8;
    }
    
    return ui8Response;
}

void ModbusUtils::set_modbus_mode(uint8_t ui8Mode){
    String sPrintString;
    if(ui8Mode <= MODBUS_MODE_TCPIP_SLAVE){
        tdfParameter.ui8ModbusMode = ui8Mode;
        sPrintString = "Modbus mode " + get_modbus_mode_string();
        elog.Write(LogLevel::Info,"ModbusUtils", sPrintString.c_str());
    }else{
        tdfParameter.ui8ModbusMode = MODBUS_MODE_RTU_MASTER;
        sPrintString = "Modbus mode error setting back to " + get_modbus_mode_string();
        elog.Write(LogLevel::Info,"ModbusUtils", sPrintString.c_str());
    }
}

String ModbusUtils::get_modbus_mode_string(void){
    String sData;
    if(tdfParameter.ui8ModbusMode == MODBUS_MODE_NONE){
        sData = "MODBUS_MODE_NONE";
    }else if(tdfParameter.ui8ModbusMode == MODBUS_MODE_RTU_MASTER){
        sData = "MODBUS_MODE_RTU_MASTER";
    }else if(tdfParameter.ui8ModbusMode == MODBUS_MODE_RTU_SLAVE){
        sData = "MODBUS_MODE_RTU_SLAVE";
    }else if(tdfParameter.ui8ModbusMode == MODBUS_MODE_ASCII_MASTER){
        sData = "MODBUS_MODE_ASCII_MASTER";
    }else if(tdfParameter.ui8ModbusMode == MODBUS_MODE_ASCII_SLAVE){
        sData = "MODBUS_MODE_ASCII_SLAVE";
    }else if(tdfParameter.ui8ModbusMode == MODBUS_MODE_TCPIP_MASTER){
        sData = "MODBUS_MODE_TCPIP_MASTER";
    }else if(tdfParameter.ui8ModbusMode == MODBUS_MODE_TCPIP_SLAVE){
        sData = "MODBUS_MODE_TCPIP_SLAVE";
    }else{
        sData = "MODBUS_MODE_ERROR";
    }

    return sData;
}


uint32_t ModbusUtils::get_baudrate(void){
    return tdfParameter.ui32Baudrate;
}

uint8_t  ModbusUtils::get_parity(void){
    return tdfParameter.ui8Parity;
}

uint8_t  ModbusUtils::get_time_per_byte(void){
    return tdfParameter.ui8Timeperbyte;
}

uint8_t  ModbusUtils::get_stopbits(void){
    return tdfParameter.ui8Stopbits;
}

uint8_t  ModbusUtils::get_databits(void){
    return tdfParameter.ui8Databits;
}

void ModbusUtils::set_baudrate(uint8_t ui8Index){
    if(ui8Index < MODBUS_SPEEDS_SUPPORTED_MAX){
        tdfParameter.ui32Baudrate = tdfParameter.aui32ModbusBaudrateValue[ui8Index];
    }else{
        tdfParameter.ui32Baudrate = tdfParameter.aui32ModbusBaudrateValue[4];
    }
}

void ModbusUtils::set_parity(uint8_t ui8Index){
    if(ui8Index < MODBUS_PARITY_SUPPORT_MAX){
        tdfParameter.ui8Parity = tdfParameter.aui8ModbusParity[ui8Index];
    }else{
        tdfParameter.ui8Parity = tdfParameter.aui8ModbusParity[0];
    }
}

void ModbusUtils::set_time_per_byte(uint8_t ui8Index){
    if(ui8Index < MODBUS_SPEEDS_SUPPORTED_MAX){
        tdfParameter.ui8Timeperbyte = tdfParameter.aui8TimePerByte[ui8Index];
    }else{
        tdfParameter.ui8Timeperbyte = tdfParameter.aui8TimePerByte[4];
    }
}

void ModbusUtils::set_stopbits(uint8_t ui8Value){
    if(ui8Value == MODBUS_STOP_BIT_1){
        tdfParameter.ui8Stopbits = MODBUS_STOP_BIT_1;
    }else if(ui8Value == MODBUS_STOP_BIT_2){
        tdfParameter.ui8Stopbits = MODBUS_STOP_BIT_2;
    }else{
        tdfParameter.ui8Stopbits = MODBUS_STOP_BIT_1;
    }
}

void ModbusUtils::set_databits(uint8_t ui8Value){
    if(ui8Value >= MODBUS_DATA_BITS_5 && ui8Value <= MODBUS_DATA_BITS_8){
        tdfParameter.ui8Databits = ui8Value;
    }else{
        tdfParameter.ui8Databits = MODBUS_DATA_BITS_8;
    }
}

void ModbusUtils::pin_control(uint8_t ui8Value){
    
    if(ui8Value){
        if(tdfParameter.bPinControl){
            if(tdfParameter.bPinLogic){
                digitalWrite(MODBUS_UART_CN_PIN,LOW);
            }else{
                digitalWrite(MODBUS_UART_CN_PIN,HIGH);
            }
        }

        if(tdfParameter.bLedControl){
            if(tdfParameter.bLedLogic){
                digitalWrite(MODBUS_UART_LED_PIN,LOW);
            }else{
                digitalWrite(MODBUS_UART_LED_PIN,HIGH);
            }
        }

    }else{
        if(tdfParameter.bPinControl){
            if(tdfParameter.bPinLogic){
                digitalWrite(MODBUS_UART_CN_PIN,HIGH);
            }else{
                digitalWrite(MODBUS_UART_CN_PIN,LOW);
            }
        }

        if(tdfParameter.bLedControl){
            if(tdfParameter.bLedLogic){
                digitalWrite(MODBUS_UART_LED_PIN,HIGH);
            }else{
                digitalWrite(MODBUS_UART_LED_PIN,LOW);
            }
        }
    }
}

uint8_t  ModbusUtils::convert_ascii_to_rtu(uint8_t* aui8ASCIIArray,  uint8_t* aui8RTUArray, uint16_t ui16ASCIIByteSize,uint8_t ui8IsFullPacket,uint8_t ui8NeedFullPacket,uint8_t ui8Colon){
    uint8_t  ui8RTUCounter=0;
    uint16_t ui16ASCIICounter=0;
    uint16_t ui16ASCIISize=0;
    uint8_t  ui8ByteValue=0;
    uint8_t  ui8ASCIIValue=0;
    uint16_t ui16CRC=0;

    if(ui8IsFullPacket){
        ui16ASCIISize = ui16ASCIIByteSize - 4;
    }else{
        ui16ASCIISize = ui16ASCIIByteSize;
    }

    if(ui8Colon){
        ui16ASCIICounter = 1;
    }else{
        ui16ASCIICounter = 0;
    }

    for(;ui16ASCIICounter < ui16ASCIISize;ui16ASCIICounter++){
        ui8ASCIIValue = aui8ASCIIArray[ui16ASCIICounter];
        if(ui8ASCIIValue > '9'){
            ui8ASCIIValue -= 7; 
        }
        ui8ASCIIValue -= '0';

        ui8ByteValue = ui8ASCIIValue;
        ui8ByteValue <<= 4;

        ui16ASCIICounter++;
        ui8ASCIIValue = aui8ASCIIArray[ui16ASCIICounter];
        if(ui8ASCIIValue > '9'){
            ui8ASCIIValue -= 7; 
        }
        ui8ASCIIValue -= '0';

        ui8ByteValue |= ui8ASCIIValue;

        aui8RTUArray[ui8RTUCounter++] = ui8ByteValue; 
    }

    if(ui8NeedFullPacket){
        ui16CRC = ModUtil.get_crc16(aui8RTUArray,ui8RTUCounter);
        aui8RTUArray[ui8RTUCounter++] = ui16CRC >> 8;
        aui8RTUArray[ui8RTUCounter++] = ui16CRC;
    }

    return ui8RTUCounter;
}

uint16_t ModbusUtils::convert_rtu_to_ascii(uint8_t* aui8RTUArray, uint8_t* aui8ASCIIArray,  uint8_t ui8RTUByteSize,uint8_t ui8IsFullPacket,uint8_t ui8NeedFullPacket){
    uint16_t ui16ASCIICounter=0;
    uint8_t  ui8Nibble=0;
    uint8_t  ui8RTUCounter=0;
    uint8_t  ui8Size=0;

    
    if(ui8IsFullPacket){
        ui8Size = ui8RTUByteSize - 2;
    }else{
        ui8Size = ui8RTUByteSize;
    }

    if(ui8NeedFullPacket){
        aui8ASCIIArray[ui16ASCIICounter++] = ':';
    }
    
    for(ui8RTUCounter=0;ui8RTUCounter<ui8Size;ui8RTUCounter++){
        ui8Nibble = aui8RTUArray[ui8RTUCounter] >> 4;
        if(ui8Nibble > 9){
            ui8Nibble += 7;     
        }
        ui8Nibble += 0x30;
        aui8ASCIIArray[ui16ASCIICounter++] = ui8Nibble;

        ui8Nibble = aui8RTUArray[ui8RTUCounter] & 0x0F;
        if(ui8Nibble > 9){
            ui8Nibble += 7;     
        }
        ui8Nibble += 0x30;
        aui8ASCIIArray[ui16ASCIICounter++] = ui8Nibble; 
    }

    if(ui8NeedFullPacket){
        uint8_t ui8Value=0;
        ui8Value = ModUtil.get_ascii_lrc(aui8RTUArray,ui8Size);
        ui8Nibble = ui8Value >> 4;
        if(ui8Nibble > 9){
            ui8Nibble += 7;     
        }
        ui8Nibble += 0x30;
        aui8ASCIIArray[ui16ASCIICounter++] = ui8Nibble;

        ui8Nibble = ui8Value & 0x0F;
        if(ui8Nibble > 9){
            ui8Nibble += 7;     
        }
        ui8Nibble += 0x30;
        aui8ASCIIArray[ui16ASCIICounter++] = ui8Nibble; 
        
        aui8ASCIIArray[ui16ASCIICounter++] = 0x0D;
        aui8ASCIIArray[ui16ASCIICounter++] = 0x0A;
    }
    

    return ui16ASCIICounter;
}

uint8_t  ModbusUtils::convert_tcp_to_rtu(uint8_t* aui8TCPArray,  uint8_t* aui8RTUArray,uint16_t* ui16TransactionId, uint16_t* ui16ProtocolId, uint16_t ui16TCPByteSize,uint8_t ui8NeedFullPacket){
    uint8_t  ui8RTUCounter=0;
    uint16_t ui16TCPCounter=0;
    uint8_t  ui8ByteSize=0;
    uint16_t ui16CRC;
    uint16_t ui16Value=0;

    ui8ByteSize = aui8TCPArray[5];

    ui16Value = aui8TCPArray[0];
    ui16Value <<= 8;
    ui16Value |= aui8TCPArray[1];
    *ui16TransactionId = ui16Value;

    ui16Value = aui8TCPArray[2];
    ui16Value <<= 8;
    ui16Value |= aui8TCPArray[3];
    *ui16ProtocolId = ui16Value;

    for(ui16TCPCounter=0;ui16TCPCounter<ui8ByteSize;ui16TCPCounter++){
        aui8RTUArray[ui8RTUCounter++] = aui8TCPArray[6+ui16TCPCounter];
    }
    
    if(ui8NeedFullPacket){
        ui16CRC = ModUtil.get_crc16(aui8RTUArray,ui8RTUCounter);
        aui8RTUArray[ui8RTUCounter++] = ui16CRC >> 8;
        aui8RTUArray[ui8RTUCounter++] = ui16CRC;
    }

    return ui8RTUCounter;
}

uint16_t ModbusUtils::convert_rtu_to_tcp(uint8_t* aui8RTUArray, uint8_t* aui8TCPArray,  uint16_t ui16TransactionId, uint16_t ui16ProtocolId, uint16_t ui16RTUByteSize,uint8_t ui8FullPacket){
    uint16_t ui16TCPCounter=0;
    uint8_t  ui16ByteCounter=0;

    aui8TCPArray[ui16TCPCounter++] = ui16TransactionId >> 8;
    aui8TCPArray[ui16TCPCounter++] = ui16TransactionId;
    aui8TCPArray[ui16TCPCounter++] = ui16ProtocolId >> 8;
    aui8TCPArray[ui16TCPCounter++] = ui16ProtocolId;

    if(ui8FullPacket){
        ui16RTUByteSize = ui16RTUByteSize - 2;
    }

    aui8TCPArray[ui16TCPCounter++] = ui16RTUByteSize >> 8;
    aui8TCPArray[ui16TCPCounter++] = ui16RTUByteSize;

    for(ui16ByteCounter=0;ui16ByteCounter<ui16RTUByteSize;ui16ByteCounter++){
        aui8TCPArray[ui16TCPCounter++] = aui8RTUArray[ui16ByteCounter];
    }

    return ui16TCPCounter;
}

void ModbusUtils::test(void){
    //uint8_t aui8RTU[8] = {0x11, 0x03, 0x00, 0x6B, 0x00, 0x03, 0xF9, 0x76};
    //uint8_t aui8RTU[8] = {0x0A, 0x02, 0x00, 0x00, 0x00, 0x0A, 0xF9, 0x76};
    //uint8_t aui8ASCII[32];
    //uint16_t ui16Value=0;

    /*ui16Value = convert_rtu_to_ascii(aui8RTU,aui8ASCII,8,1,1);
    elog.Write(LogLevel::Trace,"ModUtils", elog.get_array_to_string("converted ascii",aui8ASCII,ui16Value,PRINT_HEX).c_str());

    ui16Value = convert_ascii_to_rtu(aui8ASCII,aui8RTU,ui16Value,1,1,1);
    elog.Write(LogLevel::Trace,"ModUtils", elog.get_array_to_string("converted rtu",aui8RTU,ui16Value,PRINT_HEX).c_str());*/

}

// Preinstantiate Objects //////////////////////////////////////////////////////
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MODBUSUTILS)
ModbusUtils ModUtil;
#endif