/*******************************************************************************
*File Name: ModbusTCPClient.h
*
* Version: 1.0
*
* Description:
* In this header file for ModbusTCPClient functions
*
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , 
*******************************************************************************/
#ifndef ModbuTCPClient_h
#define ModbuTCPClient_h

#include <Arduino.h>
#include <AsyncTCP.h>

#define TCP_STATE_NONE			0
#define TCP_STATE_CONNECTING	1
#define TCP_STATE_DISCONNECTING	3
#define TCP_STATE_CONNECTED		4
#define TCP_STATE_DISCONNECTED	5
#define TCP_STATE_DATA_SENT		6
#define TCP_STATE_DATA_RECEIVED	7
#define TCP_STATE_TIMEOUT		8
#define TCP_STATE_ERROR			9

typedef struct{
    uint16_t ui16ReadResult=0;
    uint16_t ui16ProcessResult=0;
    uint16_t ui16WriteResult=0;
	uint16_t ui16PrepareResult=0;
	uint16_t ui16TransactionId=0;
	uint16_t ui16ProtocolId=0;
    uint8_t ui8ModbusRxCounter=0;
	uint16_t ui16TCPRxCounter=0;
    uint8_t  ui8AdditionalBytes=0;  
    uint8_t  aui8TCPBuffer[512];
  	IPAddress IPServerAddr;
  	uint16_t ui16ServerPort=502;
	uint8_t  ui8TCPState=TCP_STATE_NONE;
	bool 	 bCommunicationDone=false;
	uint8_t  ui8LastDeviceId=0;
	uint8_t  ui8LastReqFun=0;
	uint8_t  ui8ExpectedBytes=0;
	uint16_t ui16WaitTimeValue=0;
	bool     bWriteInitiated=0;
	uint16_t ui16TxByteCount=0;
	bool     bCommunicationInit=false;
	uint8_t  ui8ModbusError=0;
	unsigned long ulModbusTxTime=0;
	uint16_t ui16ResponseTime=0;
}TCPC_PARAMETER;

class ModbusTCPClient{
public:
    ModbusTCPClient();
	TCPC_PARAMETER tdftcpc;
	void read(void);
	void process(void);
	void write(void);
	bool is_data_received(void);
	void prepare_packet(uint8_t* aui8Data, uint8_t ui8ArraySize, uint8_t ui8DataWithDeviceID);
	void update(void);
	void connect_to_server(uint8_t ui8DeviceId,IPAddress IP, uint16_t ui16Port);
	void disconnect_from_server(void);
	static void _onConnected(void* mb, AsyncClient* client);
	void get_data_to_buffer(char* acData, size_t Size);
	void set_server_ip_address(IPAddress IP);
	IPAddress get_server_ip_address(void);
	void set_server_port(uint16_t ui16Port);
	uint16_t get_server_port(void);
	void set_device_id(uint8_t ui8DeviceId);
	uint8_t get_device_id(void);

	void read_coil_status(IPAddress IP, uint16_t ui16Port, uint8_t ui8DeviceId, uint16_t ui16CoilStartAddr, uint16_t ui16NumberOfCoils, uint16_t ui16Timeout);
    void read_input_status(IPAddress IP, uint16_t ui16Port,uint8_t ui8DeviceId,uint16_t ui16InputStartAddr, uint16_t ui16NumberOfInputs, uint16_t ui16Timeout);
    void read_holding_register(IPAddress IP, uint16_t ui16Port,uint8_t ui8DeviceId, uint16_t ui16StartAddr, uint16_t ui16NumberOfReg, uint16_t ui16Timeout);
    void read_input_register(IPAddress IP, uint16_t ui16Port,uint8_t ui8DeviceId,uint16_t ui16StartAddr, uint16_t ui16NumberOfReg, uint16_t ui16Timeout);
    void write_coil(IPAddress IP, uint16_t ui16Port,uint8_t ui8DeviceId, uint16_t ui16CoilIndex,bool bValue, uint16_t ui16Timeout);
    void write_register(IPAddress IP, uint16_t ui16Port,uint8_t ui8DeviceId,uint16_t ui16RegIndex, uint16_t ui16RegValue, uint16_t ui16Timeout);
    void write_multiple_coils(IPAddress IP, uint16_t ui16Port,uint8_t ui8DeviceId, uint16_t ui16CoilStartIndex,uint8_t* aui8Data,uint16_t ui16NumberOfCoils, uint16_t ui16Timeout);
    void write_multiple_registers(IPAddress IP, uint16_t ui16Port,uint8_t ui8DeviceId,uint16_t ui16StartRegIndex,uint8_t* aui8Data,uint16_t ui16NumberOfReg, uint16_t ui16Timeout);
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
private:
  	AsyncClient _client;
  	void _connect();
 	void _disconnect(bool now = false);
  	
  	static void _onDisconnected(void* mb, AsyncClient* client);
  	static void _onError(void* mb, AsyncClient* client, int8_t error);
  	static void _onTimeout(void* mb, AsyncClient* client, uint32_t time);
  	static void _onData(void* mb, AsyncClient* client, void* data, size_t length);
  	static void _onPoll(void* mb, AsyncClient* client);
  	uint32_t _lastMillis;
  	const uint8_t _serverID = 1;
  	const IPAddress _addr;
  	const uint16_t _port=502;
	QueueHandle_t _queue;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MODBUSTCPCLIENT)
extern ModbusTCPClient tcpc;
#endif
#endif