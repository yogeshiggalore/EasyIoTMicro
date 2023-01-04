/*******************************************************************************
* File Name: ModbusASCIISlave.h
*
* Version: 1.0
*
* Description:
* This is header file for modbus ascii slave application. All the functions related to
* modbus ascii slave are here
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
#ifndef ModbusASCIISlave_h
#define ModbusASCIISlave_h
#include <Arduino.h>

#define MODBUS_ASCII_COIL_STATUS_SIZE     24
#define MODBUS_ASCII_INPUT_STATUS_SIZE    10
#define MODBUS_ASCII_HOLDING_REG_SIZE     10
#define MODBUS_ASCII_INPUT_REG_SIZE       10

#define MODBUS_ASCII_COIL_STATUS_START_REG_INDEX      0
#define MODBUS_ASCII_INPUT_STATUS_START_REG_INDEX     0
#define MODBUS_ASCII_HOLDING_START_REG_INDEX          0
#define MODBUS_ASCII_INPUT_START_REG_INDEX            0

typedef struct{
    uint16_t ui16ReadResult=0;
    uint16_t ui16ProcessResult=0;
    uint16_t ui16WriteResult=0;
    uint16_t ui16ModbusRxCounter=0;
    uint8_t  ui8DeviceId=10;
    uint8_t  ui8AdditionalBytes=0;  
    bool     abCoilStatusbits[MODBUS_ASCII_COIL_STATUS_SIZE];
    bool     abInputStatusbits[MODBUS_ASCII_INPUT_STATUS_SIZE];
    uint16_t aui16HoldingReg[MODBUS_ASCII_HOLDING_REG_SIZE];
    uint16_t aui16InputReg[MODBUS_ASCII_INPUT_REG_SIZE];
    uint16_t ui16ModbusASCIIRxCounter=0;
    uint8_t  aui8ASCIIBuffer[512];
}ASCIIS_PARAMETER;

class ModbusASCIISlave{
    public:
        ASCIIS_PARAMETER tdfascis;
        ModbusASCIISlave();
        void update(void);
        void read(void);
        void process(void);
        void write(void);
        bool is_data_received(void);
        
        void process_coil_status(void);
        void process_input_status(void);
        void process_holding_register(void);
        void process_input_register(void);
        void process_preset_coil(void);
        void process_write_register(void);
        void process_multiple_coils(void);
        void process_multiple_register(void);

        void exception_handling(uint8_t ui8ErrorCode, uint8_t ui8ErrorValue);
        void process_errorCode(uint8_t ui8FunctionError, uint8_t ui8ErrorCode);

        bool read_ascii_bytes(void);
        
        void test(void);
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MODBUSASCIISLAVE)
extern ModbusASCIISlave ascis;
#endif
#endif