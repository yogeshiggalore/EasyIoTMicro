/*******************************************************************************
* File Name: ModbusTCPServer.h
*
* Version: 1.0
*
* Description:
* This is header file for modbus tcp server application. All the functions related to
* modbus tcp server  are here
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
#ifndef ModbusTCPServer_h
#define ModbusTCPServer_h
#include <Arduino.h>
#include <AsyncTCP.h>
#include <vector>

#define MODBUS_TCP_COIL_STATUS_SIZE     24
#define MODBUS_TCP_INPUT_STATUS_SIZE    10
#define MODBUS_TCP_HOLDING_REG_SIZE     10
#define MODBUS_TCP_INPUT_REG_SIZE       10

#define MODBUS_TCP_COIL_STATUS_START_REG_INDEX      0
#define MODBUS_TCP_INPUT_STATUS_START_REG_INDEX     0
#define MODBUS_TCP_HOLDING_START_REG_INDEX          0
#define MODBUS_TCP_INPUT_START_REG_INDEX            0

typedef struct{
    uint16_t ui16ReadResult=0;
    uint16_t ui16ProcessResult=0;
    uint16_t ui16WriteResult=0;
    uint8_t ui8ModbusRxCounter=0;
    uint8_t  ui8DeviceId=0x0A;
    IPAddress IPServerAddr;
    uint16_t  ui16ServerPort;
    uint8_t  ui8AdditionalBytes=0;  
    bool     abCoilStatusbits[MODBUS_TCP_COIL_STATUS_SIZE];
    bool     abInputStatusbits[MODBUS_TCP_INPUT_STATUS_SIZE];
    uint16_t aui16HoldingReg[MODBUS_TCP_HOLDING_REG_SIZE]= {1,2,3,4,5,6,7,8,9,10};
    uint16_t aui16InputReg[MODBUS_TCP_INPUT_REG_SIZE];
    uint8_t  aui8TCPBuffer[512];
    uint16_t ui16TCPRxCounter;
    uint16_t ui16TxByteCount=0;
    uint16_t ui16TransactionId=0;
	uint16_t ui16ProtocolId=0;
}TCPS_PARAMETER;

class ModbusTCPServer{
    public:
        TCPS_PARAMETER tdftcps;
        ModbusTCPServer();

        void begin(IPAddress IP, uint16_t ui16Port);
        
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

        void set_server_ip_address(IPAddress IP);
	    IPAddress get_server_ip_address(void);
	    void set_server_port(uint16_t ui16Port);
	    uint16_t get_server_port(void);
	    void set_device_id(uint8_t ui8DeviceId);
	    uint8_t get_device_id(void);
        void get_data_to_buffer(char* acData, size_t Size);
        
        void test(void);
    private:

        AsyncServer _server;
        AsyncServer* server = new AsyncServer(502);
        static void _handle_new_client(void* mb, AsyncClient* client);
        
        static void _handle_error(void* mb, AsyncClient* client, int8_t error);
        static void _handle_data(void* mb, AsyncClient* client, void* data, size_t len);
        static void _handle_disconnect(void* mb, AsyncClient* client);
        static void _handle_timeout(void* mb, AsyncClient* client, uint32_t time);

        QueueHandle_t _queue;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MODBUSTCPSERVER)
extern ModbusTCPServer tcps;
#endif
#endif