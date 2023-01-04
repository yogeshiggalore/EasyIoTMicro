/*******************************************************************************
* File Name: ModbusUtils.h
*
* Version: 1.0
*
* Description:
* This is header file for modbus utility. All the functions related to
* modbus utility are here.
*
* Owner:
* Yogesh M Iggalore
*
* Code Tested With:
* 1. platformio and esp32
*
********************************************************************************
* Copyright (2020-21) ,
********************************************************************************/
#ifndef ModbusUtils_h
#define ModbusUtils_h
#include <Arduino.h>

#define MODBUS_MODE_NONE             0
#define MODBUS_MODE_RTU_MASTER       1
#define MODBUS_MODE_RTU_SLAVE        2
#define MODBUS_MODE_ASCII_MASTER     3
#define MODBUS_MODE_ASCII_SLAVE      4
#define MODBUS_MODE_TCPIP_MASTER     5
#define MODBUS_MODE_TCPIP_SLAVE      6

#define MODBUS_RX_BUFFER_SIZE   512
#define MODBUS_TX_BUFFER_SIZE   512

#define MODBUS_PARITY_NONE  0
#define MODBUS_PARITY_EVEN  1
#define MODBUS_PARITY_ODD   2
#define MODBUS_PARITY_MASK  3

#define MODBUS_STOP_BIT_0  0
#define MODBUS_STOP_BIT_1  1
#define MODBUS_STOP_BIT_2  2

#define MODBUS_DATA_BITS_5 5
#define MODBUS_DATA_BITS_6 6
#define MODBUS_DATA_BITS_7 7
#define MODBUS_DATA_BITS_8 8

#define MODBUS_SPEEDS_SUPPORTED_MAX 8
#define MODBUS_PARITY_SUPPORT_MAX   4

#define MODBUS_TCP_PORT 502

#define MODBUS_TCP_TID              0
#define MODBUS_TCP_PID              2
#define MODBUS_TCP_LEN              4
#define MODBUS_TCP_UID              6
#define MODBUS_TCP_FUNC             7
#define MODBUS_TCP_REGISTER_START   8
#define MODBUS_TCP_REGISTER_NUMBER  10

#define MODBUS_ASCII_START_BYTE   0x3A
#define MODBUS_ASCII_CR_BYTE      0x0D
#define MODBUS_ASCII_LF_BYTE      0x0A

// ModBus Function Codes (Read)
#define MODBUS_FUNC_READ_COIL_REG		            0x01
#define MODBUS_FUNC_READ_INPUT_STATUS_REG		    0x02
#define MODBUS_FUNC_READ_HOLDING_REG		        0x03
#define MODBUS_FUNC_READ_INPUT_REG			        0x04

#define MODBUS_FUNC_WRITE_SINGLE_COIL			    0x05
#define MODBUS_FUNC_WRITE_SINGLE_REG			    0x06
#define MODBUS_FUNC_WRITE_MULTI_COIL			    0x0F
#define MODBUS_FUNC_WRITE_MULTI_REG			        0x10

#define MODBUS_RTU_COMMAND_STANDARD_LENGTH          0x08

#define MODBUS_ERR_NONE						0x00
#define MODBUS_ERR_CRC						0xFF
#define MODBUS_ERR_TIMEOUT					0xFE
#define MODBUS_INVALID_REG_ADDRESS	        0xFD
#define MODBUS_INVALID_RESPONSE_LEN		    0xFC
#define MODBUS_INVALID_MODBUS_COMMAND		0xFB
#define MODBUS_INVALID_FUNC		            0xF9
#define MODBUS_INVALID_DATA_VALUE		    0xF8
#define MODBUS_GATEWAY_FAILED		        0xF7
#define MODBUS_NO_NEW_DATA		            0xF6
#define MODBUS_SLAVEID_ERROR                0xF5
#define MODBUS_ERR_UNKNOWN		            0xF1

#define MODBUS_ILLEGAL_FUN_CODE     0x01
#define MODBUS_ILLEGAL_ADDRESS_CODE 0x02
#define MODBUS_ILLEGAL_VALUE_CODE   0x03
#define MODBUS_SLAVE_FAILURE        0x04
#define MODBUS_ACKNOWLEDGE          0x05
#define MODBUS_SLAVE_BUSY           0x06
#define MODBUS_NAK_ACKNOWLEDGE      0x07
#define MODBUS_MEMORY_ERROR         0x08 
#define MODBUS_GATEWAY_ERROR        0x0A
#define MODBUS_GATEWAY_FAILURE      0x0B
    
#define MODBUS_RTU_DEFAULT_BYTE_SIZE   0x08
#define MODBUS_RTU_MIN_BYTES           0x05

#define MODBUS_RTU_MANDATORY_RESPONSE_BYTES     5

#define MODBUS_MAX_DEVICE_ID_ALLOWED   245

#define MODBUS_UART_0   0
#define MODBUS_UART_1   1
#define MODBUS_UART_2   2

#define ModbusUART          Serial2
#define MODBUS_UART_RX_PIN  16
#define MODBUS_UART_TX_PIN  17
#define MODBUS_UART_CN_PIN  19
#define MODBUS_UART_LED_PIN 25

#define MODBUS_PIN_CONTROL   1
#define MODBUS_LED_CONTROL   1

#define MODBUS_PIN_LOGIC    1
#define MODBUS_LED_LOGIC    1

typedef struct{
    uint8_t ui8ModbusMode=MODBUS_MODE_RTU_MASTER;
    uint8_t aui8ModbusRxBuffer[MODBUS_RX_BUFFER_SIZE];
    uint8_t aui8ModbusTxBuffer[MODBUS_TX_BUFFER_SIZE];
    uint32_t aui32ModbusBaudrateValue[MODBUS_SPEEDS_SUPPORTED_MAX]={115200,57600,38400,19200,9600,4800,2400,1200};
    //Baudrate : 0 - 115200, 1 - 57600, 2 - 38400 , 3 - 19200 , 4 - 9600, 5 - 4800 , 6 - 2400, 7-1200
    uint8_t aui8TimePerByte[MODBUS_SPEEDS_SUPPORTED_MAX] = {1,1,1,1,2,2,4,8};
    // 0 - None parity, 1 - Even Parity , 2 - Odd Parity , 3 - Mark Space
    uint8_t aui8ModbusParity[MODBUS_PARITY_SUPPORT_MAX] = {0, 1, 2, 3};
    uint8_t ui8BaudrateIndex;
    uint8_t ui8ParityIndex;
    uint32_t ui32Baudrate;
    uint8_t  ui8Parity;
    uint8_t ui8Databits;
    uint8_t ui8Stopbits;
    uint8_t ui8Timeperbyte;
    uint8_t ui8ModbusUART;
    bool    bPinControl = MODBUS_PIN_CONTROL;
    bool    bPinLogic = MODBUS_PIN_LOGIC;
    bool    bLedControl = MODBUS_LED_CONTROL;
    bool    bLedLogic = MODBUS_LED_LOGIC;
}PARAMETER;

class ModbusUtils{
    public:
        PARAMETER tdfParameter;
        ModbusUtils();
        void     set_modbus_mode(uint8_t ui8Mode);
        String   get_modbus_mode_string(void);
        uint32_t get_baudrate(void);
        uint8_t  get_parity(void);
        uint8_t  get_time_per_byte(void);
        uint8_t  get_stopbits(void);
        uint8_t  get_databits(void);
        void     set_baudrate(uint8_t ui8Index);
        void     set_parity(uint8_t ui8Index);
        void     set_time_per_byte(uint8_t ui8Index);
        void     set_stopbits(uint8_t ui8Value);
        void     set_databits(uint8_t ui8Value);
        void     start(uint8_t ui8Mode);
        void     set_modbus_uart_parameters(uint8_t ui8UART, uint8_t ui8Baudrate, uint8_t ui8Parity, uint8_t ui8Databits, uint8_t ui8Stopbits);
        uint8_t  get_ascii_lrc(uint8_t *MsgSourceAddr, uint16_t MsgLength);
        //char     get_ascii_lrc(uint8_t[] aui8Array, uint16_t MsgLength);
        uint16_t get_crc16(uint8_t *MsgStartAddr, uint8_t MsgLength);
        uint8_t  verify_function_code(uint8_t ui8ReceivedByte);
        uint8_t  expected_bytes_rtu(uint8_t aui8Buffer[]);
        uint8_t  get_exception(uint8_t ui8Byte);
        void     pin_control(uint8_t ui8Value);
        uint8_t  convert_ascii_to_rtu(uint8_t* aui8ASCIIArray,  uint8_t* aui8RTUArray, uint16_t ui16ASCIIByteSize,uint8_t ui8FullPacket,uint8_t ui8NeedFullPacket,uint8_t ui8Colon);
        uint16_t convert_rtu_to_ascii(uint8_t* aui8RTUArray, uint8_t* aui8ASCIIArray,  uint8_t ui8RTUByteSize,uint8_t ui8FullPacket,uint8_t ui8NeedFullPacket);
        uint8_t  convert_tcp_to_rtu(uint8_t* aui8TCPArray,  uint8_t* aui8RTUArray, uint16_t* ui16TransactionId, uint16_t* ui16ProtocolId,uint16_t ui16TCPByteSize,uint8_t ui8NeedFullPacket);
        uint16_t convert_rtu_to_tcp(uint8_t* aui8RTUArray, uint8_t* aui8TCPArray, uint16_t ui16TransactionId, uint16_t ui16ProtocolId, uint16_t ui16RTUByteSize,uint8_t ui8FullPacket);
        void     test(void);
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MODBUSUTILS)
extern ModbusUtils ModUtil;
#endif
#endif