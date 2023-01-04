/*******************************************************************************
* File Name: ModbusASCIIMaster.h
*
* Version: 1.0
*
* Description:
* This is header file for modbus ascii application. All the functions related to
* modbus ascii master are here
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
#ifndef ModbusASCIIMaster_h
#define ModbusASCIIMaster_h
#include <Arduino.h>

#define MODBUS_ASCII_WAIT_TIME    1000

typedef struct{
    uint16_t ui16ReadResult=0;
    uint16_t ui16ProcessResult=0;
    uint16_t ui16WriteResult=0;
    uint16_t ui16PrepareResult=0;
    uint16_t ui16ModbusASCIIRxCounter=0;
    uint16_t ui16ModbusRxCounter=0;
    uint16_t ui16WaitTimeValue=MODBUS_ASCII_WAIT_TIME;
    unsigned long ulModbusTxTime=0;     
    uint8_t  ui8LastDeviceId=0;
    uint8_t  ui8ExpectedBytes=0;
    bool     bCommunicationDone=0;
    uint8_t  ui8ModbusError=0;
    uint16_t ui16ResponseTime=0;
    uint8_t  aui8ASCIIBuffer[512];
    bool     bCommunicationInit=false;
    uint8_t  ui8LastReqFun=0;
}ASCIIM_PARAMETER;

class ModbusASCIIMaster{
    public:
        ASCIIM_PARAMETER tdfascim;
        ModbusASCIIMaster();
        void update(void);
        void read(void);
        void process(void);
        void write(void);
        void prepare_packet(uint8_t* aui8Data, uint8_t ui8ArraySize, uint8_t ui8DataWithCRC);
        bool is_data_received(void);
        bool read_ascii_bytes(void);
        void read_coil_status(uint8_t ui8DeviceId, uint16_t ui16CoilStartAddr, uint16_t ui16NumberOfCoils, uint16_t ui16Timeout);
        void read_input_status(uint8_t ui8DeviceId,uint16_t ui16InputStartAddr, uint16_t ui16NumberOfInputs, uint16_t ui16Timeout);
        void read_holding_register(uint8_t ui8DeviceId, uint16_t ui16StartAddr, uint16_t ui16NumberOfReg, uint16_t ui16Timeout);
        void read_input_register(uint8_t ui8DeviceId,uint16_t ui16StartAddr, uint16_t ui16NumberOfReg, uint16_t ui16Timeout);
        void write_coil(uint8_t ui8DeviceId, uint16_t ui16CoilIndex,bool bValue, uint16_t ui16Timeout);
        void write_register(uint8_t ui8DeviceId,uint16_t ui16RegIndex, uint16_t ui16RegValue, uint16_t ui16Timeout);
        void write_multiple_coils(uint8_t ui8DeviceId, uint16_t ui16CoilStartIndex,uint8_t* aui8Data,uint16_t ui16NumberOfCoils, uint16_t ui16Timeout);
        void write_multiple_registers(uint8_t ui8DeviceId,uint16_t ui16StartRegIndex,uint8_t* aui8Data,uint16_t ui16NumberOfReg, uint16_t ui16Timeout);
        void process_coil_status(void);
        void process_input_status(void);
        void process_holding_register(void);
        void process_input_register(void);
        void process_preset_coil(void);
        void process_write_register(void);
        void process_multiple_coils(void);
        void process_multiple_register(void);
        void process_received_data_error_none(void);
        void process_received_data_error_crc(void);
        void process_received_data_error_timeout(void);
        void process_received_data_error_invalid_reg_addr(void);
        void process_received_data_error_invalid_resp_length(void);
        void process_received_data_error_invalid_command(void);
        void process_received_data_error_invalid_function(void);
        void process_received_data_error_invalid_data_value(void);
        void process_received_data_error_gateway_failed(void);
        void process_received_data_error_no_new_data(void);
        void process_received_data_error_slaveid_error(void);
        void process_received_data_error_unknow(void);
        void test(void);
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MODBUSASCIIMASTER)
extern ModbusASCIIMaster ascim;
#endif
#endif