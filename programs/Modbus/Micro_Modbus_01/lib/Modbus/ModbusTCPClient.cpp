/*******************************************************************************
* File Name: ModbusTCPClient.cpp
*
* Version: 1.0
*
* Description:
* This is source file for modbus tcp client functions
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
#include <Arduino.h>
#include <ModbusTCPClient.h>
#include <ModbusUtils.h>
#include<LogLevel.h>
#include<logger.h>

// Constructors ////////////////////////////////////////////////////////////////
ModbusTCPClient::ModbusTCPClient():
    _client(),
    _lastMillis(0),
    //_state(NOTCONNECTED),
   // _serverID(serverID),
   // _addr(addr),
   // _port(port),
    //_onDataHandler(nullptr),
    //_onErrorHandler(nullptr),
    _queue() {
    	_client.onConnect(_onConnected, this);
    	_client.onDisconnect(_onDisconnected, this);
    	_client.onError(_onError, this);
    	_client.onTimeout(_onTimeout, this);
    	_client.onPoll(_onPoll, this);
    	_client.onData(_onData, this);
    	_client.setNoDelay(true);
    	_client.setAckTimeout(1000);
	}

void ModbusTCPClient::read(void){

	if(is_data_received()){
		tdftcpc.ui16ReadResult = 1;
		tdftcpc.ui16ReadResult <<= 8;
		tdftcpc.ui16ReadResult |= tdftcpc.ui8ModbusRxCounter;
	}
	
}

void ModbusTCPClient::process(void){
	uint8_t ui8Result=0;
    ui8Result = tdftcpc.ui16ReadResult >> 8;

    if(ui8Result){
        if(tdftcpc.ui8ModbusError == MODBUS_ERR_NONE){
            process_received_data_error_none();
        }else if(tdftcpc.ui8ModbusError == MODBUS_ERR_CRC){
            process_received_data_error_crc();
        }else if(tdftcpc.ui8ModbusError == MODBUS_ERR_TIMEOUT){
            process_received_data_error_timeout();
        }else if(tdftcpc.ui8ModbusError == MODBUS_INVALID_REG_ADDRESS){
            process_received_data_error_invalid_reg_addr();
        }else if(tdftcpc.ui8ModbusError == MODBUS_INVALID_RESPONSE_LEN){
            process_received_data_error_invalid_resp_length();
        }else if(tdftcpc.ui8ModbusError == MODBUS_INVALID_MODBUS_COMMAND){
            process_received_data_error_invalid_command();
        }else if(tdftcpc.ui8ModbusError == MODBUS_INVALID_FUNC){
            process_received_data_error_invalid_function();
        }else if(tdftcpc.ui8ModbusError == MODBUS_INVALID_DATA_VALUE){
            process_received_data_error_invalid_data_value();
        }else if(tdftcpc.ui8ModbusError == MODBUS_GATEWAY_FAILED){
            process_received_data_error_gateway_failed();
        }else if(tdftcpc.ui8ModbusError == MODBUS_NO_NEW_DATA){
            process_received_data_error_no_new_data();
        }else if(tdftcpc.ui8ModbusError == MODBUS_SLAVEID_ERROR){
            process_received_data_error_slaveid_error();
        }else if(tdftcpc.ui8ModbusError == MODBUS_ERR_UNKNOWN){
            process_received_data_error_unknow();
        }else{
            //devare kapadu
        }

        tdftcpc.ui16ReadResult = 0;
        tdftcpc.ui8ModbusRxCounter = 0;
        tdftcpc.bCommunicationDone = false;
        tdftcpc.bCommunicationInit = false;

		disconnect_from_server();
		tdftcpc.ui8TCPState = TCP_STATE_NONE;
    }
}

void ModbusTCPClient::write(void){
	uint8_t ui8Result=0;

	ui8Result = tdftcpc.ui16PrepareResult >> 8;
	if(ui8Result){
		tdftcpc.bWriteInitiated = true;
		uint8_t ui8ByteCount = tdftcpc.ui16PrepareResult;
		tdftcpc.ui16TransactionId++;
		tdftcpc.ui16TxByteCount = ModUtil.convert_rtu_to_tcp(ModUtil.tdfParameter.aui8ModbusTxBuffer,tdftcpc.aui8TCPBuffer,
		tdftcpc.ui16TransactionId,tdftcpc.ui16ProtocolId,ui8ByteCount,true);
		connect_to_server(ModUtil.tdfParameter.aui8ModbusTxBuffer[0],tdftcpc.IPServerAddr,tdftcpc.ui16ServerPort);
		tdftcpc.ui16PrepareResult = 0;
		tdftcpc.ui8TCPState = TCP_STATE_NONE;
		tdftcpc.ulModbusTxTime = millis();
	}

	if(tdftcpc.bWriteInitiated){
		if(tdftcpc.ui8TCPState == TCP_STATE_CONNECTED){
			tdftcpc.bWriteInitiated = false;
			tdftcpc.bCommunicationDone = false;
            tdftcpc.bCommunicationInit = true;
			elog.Write(LogLevel::Trace,"TCPClient", elog.get_array_to_string("tx",tdftcpc.aui8TCPBuffer,tdftcpc.ui16TxByteCount,PRINT_HEX).c_str());
			_client.write((char*)tdftcpc.aui8TCPBuffer,tdftcpc.ui16TxByteCount);
			tdftcpc.ui8ModbusError = MODBUS_ERR_NONE;
			tdftcpc.ui8ModbusRxCounter = 0;
			tdftcpc.ui16TCPRxCounter = 0;
			tdftcpc.ulModbusTxTime = millis();
		}else{
			if((millis() - tdftcpc.ulModbusTxTime) > tdftcpc.ui16WaitTimeValue){
				tdftcpc.ui8ModbusError = MODBUS_ERR_TIMEOUT;
				tdftcpc.bCommunicationDone = true;
				tdftcpc.bCommunicationInit = false;
				tdftcpc.ui8ModbusRxCounter = 0;
				tdftcpc.ui16ReadResult = 1;
				tdftcpc.ui16ReadResult <<= 8;
				tdftcpc.ui16ReadResult |= tdftcpc.ui8ModbusRxCounter;
				tdftcpc.bWriteInitiated = false;
			}
		}
	}
}

bool ModbusTCPClient::is_data_received(void){
	
	if(tdftcpc.bCommunicationInit == true){
		if(tdftcpc.ui8TCPState == TCP_STATE_DATA_RECEIVED){
			tdftcpc.ui8ModbusRxCounter = ModUtil.convert_tcp_to_rtu(tdftcpc.aui8TCPBuffer,ModUtil.tdfParameter.aui8ModbusRxBuffer,
			&tdftcpc.ui16TransactionId,&tdftcpc.ui16ProtocolId,tdftcpc.ui16TCPRxCounter,true);
			elog.Write(LogLevel::Trace,"TCPClient", elog.get_array_to_string("rx",ModUtil.tdfParameter.aui8ModbusRxBuffer,tdftcpc.ui8ModbusRxCounter,PRINT_HEX).c_str());
			tdftcpc.bCommunicationDone = true;
			tdftcpc.bCommunicationInit = false;
		}
	}
	
	if(tdftcpc.ui8ModbusRxCounter > 0){
        if(ModUtil.tdfParameter.aui8ModbusRxBuffer[0] != tdftcpc.ui8LastDeviceId){
            tdftcpc.ui8ModbusRxCounter = 0;
            tdftcpc.bCommunicationDone = true;
            tdftcpc.ui8ModbusError = MODBUS_SLAVEID_ERROR;
            tdftcpc.ui16ResponseTime = millis() - tdftcpc.ulModbusTxTime;
            String sPrintData = "MODBUS_SLAVEID_ERROR Time: " + String(tdftcpc.ui16ResponseTime) + " ms";
            elog.Write(LogLevel::Error,"TCPClient",sPrintData.c_str());
        }else{
            if(tdftcpc.ui8ModbusRxCounter >= MODBUS_RTU_MANDATORY_RESPONSE_BYTES){
                uint8_t ui8FunCodeResponse=0;
                ui8FunCodeResponse = ModUtil.verify_function_code(ModUtil.tdfParameter.aui8ModbusRxBuffer[1]);
                if(ui8FunCodeResponse == 0){
                    tdftcpc.bCommunicationDone = true;
                    tdftcpc.ui8ModbusError = MODBUS_INVALID_FUNC;
                    tdftcpc.ui16ResponseTime = millis() - tdftcpc.ulModbusTxTime;
                    String sPrintData = "MODBUS_INVALID_FUNC Time: " + String(tdftcpc.ui16ResponseTime) + " ms bytes:" + String(tdftcpc.ui8ModbusRxCounter);
                    elog.Write(LogLevel::Error,"TCPClient",sPrintData.c_str());
                }else if(ui8FunCodeResponse == 2){
                    tdftcpc.bCommunicationDone = true;
                    tdftcpc.ui8ModbusError = ModUtil.get_exception(ModUtil.tdfParameter.aui8ModbusRxBuffer[2]);
                    tdftcpc.ui16ResponseTime = millis() - tdftcpc.ulModbusTxTime;
                    String sPrintData = "MODBUS_EXCEPTION " + String(tdftcpc.ui8ModbusError) + " Time: " + String(tdftcpc.ui16ResponseTime) + " ms bytes:" + String(tdftcpc.ui8ModbusRxCounter);
                    elog.Write(LogLevel::Error,"TCPClient",sPrintData.c_str());
                }else{
                    if(tdftcpc.ui8ModbusRxCounter >= tdftcpc.ui8ExpectedBytes){
                        uint16_t ui16CRC;
                        uint8_t  ui8CRCLow;
                        uint8_t  ui8CRCHigh;
                        uint8_t  ui8CRCCompLow;
                        uint8_t  ui8CRCCompHigh;

                        ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusRxBuffer,tdftcpc.ui8ModbusRxCounter-2);
                        ui8CRCLow = ui16CRC >> 8;
                        ui8CRCHigh = ui16CRC;
                        ui8CRCCompLow = ModUtil.tdfParameter.aui8ModbusRxBuffer[tdftcpc.ui8ModbusRxCounter-2];
                        ui8CRCCompHigh = ModUtil.tdfParameter.aui8ModbusRxBuffer[tdftcpc.ui8ModbusRxCounter-1];

                        if((ui8CRCLow == ui8CRCCompLow) && (ui8CRCHigh == ui8CRCCompHigh)){
                            tdftcpc.bCommunicationDone = true;
                            tdftcpc.ui8ModbusError = MODBUS_ERR_NONE;
                            tdftcpc.ui16ResponseTime = millis() - tdftcpc.ulModbusTxTime;
                            String sPrintData =" Time: " + String(tdftcpc.ui16ResponseTime) + " ms bytes:" + String(tdftcpc.ui8ModbusRxCounter) + " rtu";
                            elog.Write(LogLevel::Trace,"TCPClient", elog.get_array_to_string(sPrintData,ModUtil.tdfParameter.aui8ModbusRxBuffer,tdftcpc.ui8ModbusRxCounter,PRINT_HEX).c_str());
                        }else{
                            tdftcpc.bCommunicationDone = true;
                            tdftcpc.ui8ModbusError = MODBUS_INVALID_RESPONSE_LEN;
                            tdftcpc.ui16ResponseTime = millis() - tdftcpc.ulModbusTxTime;
                            String sPrintData ="MODBUS_INVALID_RESPONSE_LEN  Time: " + String(tdftcpc.ui16ResponseTime) + " ms bytes:" + String(tdftcpc.ui8ModbusRxCounter) + " rtu";
                            elog.Write(LogLevel::Error,"TCPClient",sPrintData.c_str());
                        }
                    }
                }
            }
        }
    }

	if(tdftcpc.bCommunicationInit == true){
		if(tdftcpc.bCommunicationDone == false){
			if((millis() - tdftcpc.ulModbusTxTime) > tdftcpc.ui16WaitTimeValue){
				tdftcpc.ui8ModbusError = MODBUS_ERR_TIMEOUT;
				tdftcpc.bCommunicationDone = true;
				tdftcpc.bCommunicationInit = false;
				tdftcpc.ui8ModbusRxCounter = 0;
			}
		}
	}
	

	if(tdftcpc.ui8TCPState == TCP_STATE_ERROR){
		tdftcpc.ui8ModbusError = MODBUS_ERR_TIMEOUT;
		tdftcpc.bCommunicationDone = true;
		tdftcpc.bCommunicationInit = false;
		tdftcpc.ui8ModbusRxCounter = 0;
	}

	return tdftcpc.bCommunicationDone;
}

void ModbusTCPClient::prepare_packet(uint8_t* aui8Data, uint8_t ui8ArraySize, uint8_t ui8DataWithDeviceID){
	
}

void ModbusTCPClient::update(void){
	read();
	process();
	write();
}

void ModbusTCPClient::read_coil_status(IPAddress IP, uint16_t ui16Port,uint8_t ui8DeviceId, uint16_t ui16CoilStartAddr, uint16_t ui16NumberOfCoils, uint16_t ui16Timeout){
    uint16_t ui16CRC;

    tdftcpc.IPServerAddr = IP;
	tdftcpc.ui16ServerPort = ui16Port;
	tdftcpc.ui8LastDeviceId = ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[0] = ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[1] = MODBUS_FUNC_READ_COIL_REG;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[2] = ui16CoilStartAddr >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[3] = ui16CoilStartAddr;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[4] = ui16NumberOfCoils >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[5] = ui16NumberOfCoils;
    
    ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,6);
    ModUtil.tdfParameter.aui8ModbusTxBuffer[6] = ui16CRC >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[7] = ui16CRC;

    tdftcpc.ui8LastDeviceId = ui8DeviceId;
    tdftcpc.ui8LastReqFun = MODBUS_FUNC_READ_COIL_REG;
    tdftcpc.ui8ExpectedBytes = ModUtil.expected_bytes_rtu(ModUtil.tdfParameter.aui8ModbusTxBuffer);
    tdftcpc.ui16WaitTimeValue = ui16Timeout;
    tdftcpc.ui16PrepareResult = 1;
    tdftcpc.ui16PrepareResult <<= 8;
    tdftcpc.ui16PrepareResult = tdftcpc.ui16PrepareResult | 8;

    elog.Write(LogLevel::Trace,"TCPClient","read coil status ");
}

void ModbusTCPClient::read_input_status(IPAddress IP, uint16_t ui16Port,uint8_t ui8DeviceId,uint16_t ui16InputStartAddr, uint16_t ui16NumberOfInputs, uint16_t ui16Timeout){
    uint16_t ui16CRC;

	tdftcpc.IPServerAddr = IP;
	tdftcpc.ui16ServerPort = ui16Port;
	tdftcpc.ui8LastDeviceId = ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[0] = ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[1] = MODBUS_FUNC_READ_INPUT_STATUS_REG;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[2] = ui16InputStartAddr >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[3] = ui16InputStartAddr;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[4] = ui16NumberOfInputs >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[5] = ui16NumberOfInputs;
    
    ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,6);
    ModUtil.tdfParameter.aui8ModbusTxBuffer[6] = ui16CRC >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[7] = ui16CRC;

    tdftcpc.ui8LastDeviceId = ui8DeviceId;
    tdftcpc.ui8LastReqFun = MODBUS_FUNC_READ_INPUT_STATUS_REG;
    tdftcpc.ui8ExpectedBytes = ModUtil.expected_bytes_rtu(ModUtil.tdfParameter.aui8ModbusTxBuffer);
    tdftcpc.ui16WaitTimeValue = ui16Timeout;
    tdftcpc.ui16PrepareResult = 1;
    tdftcpc.ui16PrepareResult <<= 8;
    tdftcpc.ui16PrepareResult = tdftcpc.ui16PrepareResult | 8;

    elog.Write(LogLevel::Trace,"TCPClient","read input status ");
}

void ModbusTCPClient::read_holding_register(IPAddress IP, uint16_t ui16Port,uint8_t ui8DeviceId, uint16_t ui16StartAddr, uint16_t ui16NumberOfReg, uint16_t ui16Timeout){
    uint16_t ui16CRC;

	tdftcpc.IPServerAddr = IP;
	tdftcpc.ui16ServerPort = ui16Port;
	tdftcpc.ui8LastDeviceId = ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[0] = ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[1] = MODBUS_FUNC_READ_HOLDING_REG;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[2] = ui16StartAddr >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[3] = ui16StartAddr;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[4] = ui16NumberOfReg >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[5] = ui16NumberOfReg;
    
    ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,6);
    ModUtil.tdfParameter.aui8ModbusTxBuffer[6] = ui16CRC >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[7] = ui16CRC;

    tdftcpc.ui8LastDeviceId = ui8DeviceId;
    tdftcpc.ui8LastReqFun = MODBUS_FUNC_READ_HOLDING_REG;
    tdftcpc.ui8ExpectedBytes = ModUtil.expected_bytes_rtu(ModUtil.tdfParameter.aui8ModbusTxBuffer);
    tdftcpc.ui16WaitTimeValue = ui16Timeout;
    tdftcpc.ui16PrepareResult = 1;
    tdftcpc.ui16PrepareResult <<= 8;
    tdftcpc.ui16PrepareResult = tdftcpc.ui16PrepareResult | 8;

    elog.Write(LogLevel::Trace,"TCPClient","read holding register");
}

void ModbusTCPClient::read_input_register(IPAddress IP, uint16_t ui16Port,uint8_t ui8DeviceId,uint16_t ui16StartAddr, uint16_t ui16NumberOfReg, uint16_t ui16Timeout){
    uint16_t ui16CRC;

	tdftcpc.IPServerAddr = IP;
	tdftcpc.ui16ServerPort = ui16Port;
	tdftcpc.ui8LastDeviceId = ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[0] = ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[1] = MODBUS_FUNC_READ_INPUT_REG;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[2] = ui16StartAddr >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[3] = ui16StartAddr;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[4] = ui16NumberOfReg >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[5] = ui16NumberOfReg;
    
    ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,6);
    ModUtil.tdfParameter.aui8ModbusTxBuffer[6] = ui16CRC >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[7] = ui16CRC;

    tdftcpc.ui8LastDeviceId = ui8DeviceId;
    tdftcpc.ui8LastReqFun = MODBUS_FUNC_READ_INPUT_REG;
    tdftcpc.ui8ExpectedBytes = ModUtil.expected_bytes_rtu(ModUtil.tdfParameter.aui8ModbusTxBuffer);
    tdftcpc.ui16WaitTimeValue = ui16Timeout;
    tdftcpc.ui16PrepareResult = 1;
    tdftcpc.ui16PrepareResult <<= 8;
    tdftcpc.ui16PrepareResult = tdftcpc.ui16PrepareResult | 8;

    elog.Write(LogLevel::Trace,"TCPClient","read input register");
}

void ModbusTCPClient::write_coil(IPAddress IP, uint16_t ui16Port,uint8_t ui8DeviceId, uint16_t ui16CoilIndex,bool bValue, uint16_t ui16Timeout){
    uint16_t ui16CRC;

	tdftcpc.IPServerAddr = IP;
	tdftcpc.ui16ServerPort = ui16Port;
	tdftcpc.ui8LastDeviceId = ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[0] = ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[1] = MODBUS_FUNC_WRITE_SINGLE_COIL;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[2] = ui16CoilIndex >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[3] = ui16CoilIndex;
    
    if(bValue){
        ModUtil.tdfParameter.aui8ModbusTxBuffer[4] = 0xFF;
        ModUtil.tdfParameter.aui8ModbusTxBuffer[5] = 0x00;
    }else{
        ModUtil.tdfParameter.aui8ModbusTxBuffer[4] = 0x00;
        ModUtil.tdfParameter.aui8ModbusTxBuffer[5] = 0x00;
    }
    
    
    ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,6);
    ModUtil.tdfParameter.aui8ModbusTxBuffer[6] = ui16CRC >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[7] = ui16CRC;

    tdftcpc.ui8LastDeviceId = ui8DeviceId;
    tdftcpc.ui8LastReqFun = MODBUS_FUNC_WRITE_SINGLE_COIL;
    tdftcpc.ui8ExpectedBytes = 8;
    tdftcpc.ui16WaitTimeValue = ui16Timeout;
    tdftcpc.ui16PrepareResult = 1;
    tdftcpc.ui16PrepareResult <<= 8;
    tdftcpc.ui16PrepareResult = tdftcpc.ui16PrepareResult | 8;

    elog.Write(LogLevel::Trace,"TCPClient","write coil");
}

void ModbusTCPClient::write_register(IPAddress IP, uint16_t ui16Port,uint8_t ui8DeviceId,uint16_t ui16RegIndex, uint16_t ui16RegValue, uint16_t ui16Timeout){
    uint16_t ui16CRC;

	tdftcpc.IPServerAddr = IP;
	tdftcpc.ui16ServerPort = ui16Port;
	tdftcpc.ui8LastDeviceId = ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[0] = ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[1] = MODBUS_FUNC_WRITE_SINGLE_REG;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[2] = ui16RegIndex >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[3] = ui16RegIndex;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[4] = ui16RegValue >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[5] = ui16RegValue;
    

    ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,6);
    ModUtil.tdfParameter.aui8ModbusTxBuffer[6] = ui16CRC >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[7] = ui16CRC;

    tdftcpc.ui8LastDeviceId = ui8DeviceId;
    tdftcpc.ui8LastReqFun = MODBUS_FUNC_WRITE_SINGLE_REG;
    tdftcpc.ui8ExpectedBytes = 8;
    tdftcpc.ui16WaitTimeValue = ui16Timeout;
    tdftcpc.ui16PrepareResult = 1;
    tdftcpc.ui16PrepareResult <<= 8;
    tdftcpc.ui16PrepareResult = tdftcpc.ui16PrepareResult | 8;

    elog.Write(LogLevel::Trace,"TCPClient","write register");
}

void ModbusTCPClient::write_multiple_coils(IPAddress IP, uint16_t ui16Port,uint8_t ui8DeviceId, uint16_t ui16CoilStartIndex,uint8_t* aui8Data,uint16_t ui16NumberOfCoils, uint16_t ui16Timeout){
    uint16_t ui16CRC;
    uint8_t ui8ArrayCounter=0;
    uint8_t ui8ByteCount=0;
    uint8_t ui8LoopCounter=0;

	tdftcpc.IPServerAddr = IP;
	tdftcpc.ui16ServerPort = ui16Port;
	tdftcpc.ui8LastDeviceId = ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = MODBUS_FUNC_WRITE_MULTI_COIL;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CoilStartIndex >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CoilStartIndex;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16NumberOfCoils >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16NumberOfCoils;

    if((ui16NumberOfCoils % 8) > 0){
        ui8ByteCount = (ui16NumberOfCoils / 8) + 1;
    }else{
        ui8ByteCount = (ui16NumberOfCoils / 8);
    }
    
    ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui8ByteCount;
    for(ui8LoopCounter=0;ui8LoopCounter<ui8ByteCount;ui8LoopCounter++){
        ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = aui8Data[ui8LoopCounter];
    }

    ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,ui8ArrayCounter);
    ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CRC >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CRC;

    tdftcpc.ui8LastDeviceId = ui8DeviceId;
    tdftcpc.ui8LastReqFun = MODBUS_FUNC_WRITE_MULTI_COIL;
    tdftcpc.ui8ExpectedBytes = ModUtil.expected_bytes_rtu(ModUtil.tdfParameter.aui8ModbusTxBuffer);
    tdftcpc.ui16WaitTimeValue = ui16Timeout;
    tdftcpc.ui16PrepareResult = 1;
    tdftcpc.ui16PrepareResult <<= 8;
    tdftcpc.ui16PrepareResult = tdftcpc.ui16PrepareResult | ui8ArrayCounter;

    elog.Write(LogLevel::Trace,"TCPClient","write multiple coils");
}

void ModbusTCPClient::write_multiple_registers(IPAddress IP, uint16_t ui16Port,uint8_t ui8DeviceId,uint16_t ui16StartRegIndex,uint8_t* aui8Data,uint16_t ui16NumberOfReg, uint16_t ui16Timeout){
    uint16_t ui16CRC;
    uint8_t ui8ArrayCounter=0;
    uint8_t ui8ByteCount=0;
    uint8_t ui8LoopCounter=0;

	tdftcpc.IPServerAddr = IP;
	tdftcpc.ui16ServerPort = ui16Port;
	tdftcpc.ui8LastDeviceId = ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = MODBUS_FUNC_WRITE_MULTI_REG;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16StartRegIndex >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16StartRegIndex;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16NumberOfReg >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16NumberOfReg;

    ui8ByteCount = ui16NumberOfReg * 2;

    ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui8ByteCount;
    for(ui8LoopCounter=0;ui8LoopCounter<ui8ByteCount;ui8LoopCounter++){
        ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = aui8Data[ui8LoopCounter];
    }

    ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,ui8ArrayCounter);
    ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CRC >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CRC;

    tdftcpc.ui8LastDeviceId = ui8DeviceId;
    tdftcpc.ui8LastReqFun = MODBUS_FUNC_WRITE_MULTI_REG;
    tdftcpc.ui8ExpectedBytes = ModUtil.expected_bytes_rtu(ModUtil.tdfParameter.aui8ModbusTxBuffer);
    tdftcpc.ui16WaitTimeValue = ui16Timeout;
    tdftcpc.ui16PrepareResult = 1;
    tdftcpc.ui16PrepareResult <<= 8;
    tdftcpc.ui16PrepareResult = tdftcpc.ui16PrepareResult | ui8ArrayCounter;

    elog.Write(LogLevel::Trace,"TCPClient","write multiple registers");
}

void ModbusTCPClient::process_coil_status(void){
    uint8_t ui8LoopCounter=0;
    uint8_t ui8BitCounter=0;
    uint8_t ui8BitMask=0;
    uint16_t ui16CoilIndex=0;
    String sData = "Read Coil status: ";

    ui16CoilIndex = ModUtil.tdfParameter.aui8ModbusTxBuffer[2];
    ui16CoilIndex <<= 8;
    ui16CoilIndex |=  ModUtil.tdfParameter.aui8ModbusTxBuffer[3];

    for(ui8LoopCounter=0;ui8LoopCounter<ModUtil.tdfParameter.aui8ModbusRxBuffer[2];ui8LoopCounter++){
        ui8BitMask = 1;
        for(ui8BitCounter=0;ui8BitCounter<8;ui8BitCounter++){
            if(ModUtil.tdfParameter.aui8ModbusRxBuffer[3+ui8LoopCounter] & ui8BitMask){
                sData += String(ui16CoilIndex) + ":ON "; 
            }else{
                sData += String(ui16CoilIndex) + ":OFF "; 
            }
            ui8BitMask <<=1;
            ui16CoilIndex++;
        }
    }

    elog.Write(LogLevel::Trace,"TCPClient",sData.c_str());
}

void ModbusTCPClient::process_input_status(void){
    uint8_t ui8LoopCounter=0;
    uint8_t ui8BitCounter=0;
    uint8_t ui8BitMask=0;
    uint16_t ui16InputStatusIndex=0;
    String sData = "Read Input status: ";

    ui16InputStatusIndex = ModUtil.tdfParameter.aui8ModbusTxBuffer[2];
    ui16InputStatusIndex <<= 8;
    ui16InputStatusIndex |=  ModUtil.tdfParameter.aui8ModbusTxBuffer[3];

    for(ui8LoopCounter=0;ui8LoopCounter<ModUtil.tdfParameter.aui8ModbusRxBuffer[2];ui8LoopCounter++){
        ui8BitMask = 1;
        for(ui8BitCounter=0;ui8BitCounter<8;ui8BitCounter++){
            if(ModUtil.tdfParameter.aui8ModbusRxBuffer[3+ui8LoopCounter] & ui8BitMask){
                sData += String(ui16InputStatusIndex) + ":ON "; 
            }else{
                sData += String(ui16InputStatusIndex) + ":OFF "; 
            }
            ui8BitMask <<=1;
            ui16InputStatusIndex++;
        }
    }

    elog.Write(LogLevel::Trace,"TCPClient",sData.c_str());
}

void ModbusTCPClient::process_holding_register(void){
    uint8_t ui8LoopCounter=0;
    uint16_t ui16RegIndex=0;
    uint16_t ui16Value=0;
    String sData = "Read Holding register: ";

    ui16RegIndex = ModUtil.tdfParameter.aui8ModbusTxBuffer[2];
    ui16RegIndex <<= 8;
    ui16RegIndex |=  ModUtil.tdfParameter.aui8ModbusTxBuffer[3];

    for(ui8LoopCounter=0;ui8LoopCounter<ModUtil.tdfParameter.aui8ModbusRxBuffer[2];ui8LoopCounter++){
        ui16Value = ModUtil.tdfParameter.aui8ModbusRxBuffer[3 + ui8LoopCounter];
        ui16Value <<= 8;
        ui8LoopCounter++;
        ui16Value |= ModUtil.tdfParameter.aui8ModbusRxBuffer[3 + ui8LoopCounter]; 

        sData += String(ui16RegIndex) + ": " + String(ui16Value) + " ";
    }

    elog.Write(LogLevel::Trace,"TCPClient",sData.c_str());
}

void ModbusTCPClient::process_input_register(void){
    uint8_t ui8LoopCounter=0;
    uint16_t ui16RegIndex=0;
    uint16_t ui16Value=0;
    String sData = "Read Input register: ";

    ui16RegIndex = ModUtil.tdfParameter.aui8ModbusTxBuffer[2];
    ui16RegIndex <<= 8;
    ui16RegIndex |=  ModUtil.tdfParameter.aui8ModbusTxBuffer[3];

    for(ui8LoopCounter=0;ui8LoopCounter<ModUtil.tdfParameter.aui8ModbusRxBuffer[2];ui8LoopCounter++){
        ui16Value = ModUtil.tdfParameter.aui8ModbusRxBuffer[3 + ui8LoopCounter];
        ui16Value <<= 8;
        ui8LoopCounter++;
        ui16Value |= ModUtil.tdfParameter.aui8ModbusRxBuffer[3 + ui8LoopCounter]; 

        sData += String(ui16RegIndex) + ": " + String(ui16Value) + " ";
    }

    elog.Write(LogLevel::Trace,"TCPClient",sData.c_str());
}

void ModbusTCPClient::process_preset_coil(void){
    uint16_t ui16CoilTxIndex=0;
    uint16_t ui16CoilTxValue=0;
    uint16_t ui16CoilRxIndex=0;
    uint16_t ui16CoilRxValue=0;
    String sData;

    ui16CoilTxIndex = ModUtil.tdfParameter.aui8ModbusTxBuffer[2];
    ui16CoilTxIndex <<= 8;
    ui16CoilTxIndex |= ModUtil.tdfParameter.aui8ModbusTxBuffer[3];

    ui16CoilRxIndex = ModUtil.tdfParameter.aui8ModbusRxBuffer[2];
    ui16CoilRxIndex <<= 8;
    ui16CoilRxIndex |= ModUtil.tdfParameter.aui8ModbusRxBuffer[3]; 

    ui16CoilTxValue = ModUtil.tdfParameter.aui8ModbusTxBuffer[4];
    ui16CoilTxValue <<= 8;
    ui16CoilTxValue |= ModUtil.tdfParameter.aui8ModbusTxBuffer[5];

    ui16CoilRxValue = ModUtil.tdfParameter.aui8ModbusRxBuffer[4];
    ui16CoilRxValue <<= 8;
    ui16CoilRxValue |= ModUtil.tdfParameter.aui8ModbusRxBuffer[5];

    if(ui16CoilTxIndex == ui16CoilRxIndex){
        if(ui16CoilTxValue == ui16CoilRxValue){
            if(ui16CoilTxValue == 0xFF00){
                sData = "Coil " + String(ui16CoilTxIndex) + " write 1 sucess"; 
            }else{
                sData = "Coil " + String(ui16CoilTxIndex) + " write 0 sucess"; 
            }
        }else{
            sData = "Coil " + String(ui16CoilTxIndex) + " write failure"; 
        }
    }else{
        sData = "Coil " + String(ui16CoilTxIndex) + " write failure"; 
    }

    elog.Write(LogLevel::Trace,"TCPClient",sData.c_str());
}

void ModbusTCPClient::process_write_register(void){
    uint16_t ui16RegTxIndex=0;
    uint16_t ui16RegTxValue=0;
    uint16_t ui16RegRxIndex=0;
    uint16_t ui16RegRxValue=0;
    String sData;

    ui16RegTxIndex = ModUtil.tdfParameter.aui8ModbusTxBuffer[2];
    ui16RegTxIndex <<= 8;
    ui16RegTxIndex |= ModUtil.tdfParameter.aui8ModbusTxBuffer[3];

    ui16RegRxIndex = ModUtil.tdfParameter.aui8ModbusRxBuffer[2];
    ui16RegRxIndex <<= 8;
    ui16RegRxIndex |= ModUtil.tdfParameter.aui8ModbusRxBuffer[3]; 

    ui16RegTxValue = ModUtil.tdfParameter.aui8ModbusTxBuffer[4];
    ui16RegTxValue <<= 8;
    ui16RegTxValue |= ModUtil.tdfParameter.aui8ModbusTxBuffer[5];

    ui16RegRxValue = ModUtil.tdfParameter.aui8ModbusRxBuffer[4];
    ui16RegRxValue <<= 8;
    ui16RegRxValue |= ModUtil.tdfParameter.aui8ModbusRxBuffer[5];

    if(ui16RegTxIndex == ui16RegRxIndex){
        if(ui16RegTxValue == ui16RegRxValue){
            sData = "Reg " + String(ui16RegTxIndex) + " write value " + String(ui16RegTxValue) + " sucess"; 
        }else{
            sData = "Reg " + String(ui16RegTxIndex) + " write value " + String(ui16RegTxValue) + " failure"; 
        }
    }else{
        sData = "Reg " + String(ui16RegTxIndex) + " write value " + String(ui16RegTxValue) + " failure"; 
    }

    elog.Write(LogLevel::Trace,"TCPClient",sData.c_str());
}

void ModbusTCPClient::process_multiple_coils(void){
    uint16_t ui16CoilTxIndex=0;
    uint16_t ui16CoilTxNumber=0;
    uint16_t ui16CoilRxIndex=0;
    uint16_t ui16CoilRxNumber=0;
    String sData;

    ui16CoilTxIndex = ModUtil.tdfParameter.aui8ModbusTxBuffer[2];
    ui16CoilTxIndex <<= 8;
    ui16CoilTxIndex |= ModUtil.tdfParameter.aui8ModbusTxBuffer[3];

    ui16CoilRxIndex = ModUtil.tdfParameter.aui8ModbusRxBuffer[2];
    ui16CoilRxIndex <<= 8;
    ui16CoilRxIndex |= ModUtil.tdfParameter.aui8ModbusRxBuffer[3]; 

    ui16CoilTxNumber = ModUtil.tdfParameter.aui8ModbusTxBuffer[4];
    ui16CoilTxNumber <<= 8;
    ui16CoilTxNumber |= ModUtil.tdfParameter.aui8ModbusTxBuffer[5];

    ui16CoilRxNumber = ModUtil.tdfParameter.aui8ModbusRxBuffer[4];
    ui16CoilRxNumber <<= 8;
    ui16CoilRxNumber |= ModUtil.tdfParameter.aui8ModbusRxBuffer[5];

    if(ui16CoilTxIndex == ui16CoilRxIndex){
        if(ui16CoilTxNumber == ui16CoilRxNumber){
            sData = String(ui16CoilTxIndex) + " Coils from " + String(ui16CoilTxIndex) + " write sucess"; 
        }else{
            sData = String(ui16CoilTxIndex) + " Coils from " + String(ui16CoilTxIndex) + " write failure"; 
        }
    }else{
        sData = String(ui16CoilTxIndex) + " Coils from " + String(ui16CoilTxIndex) + " write sucess";
    }

    elog.Write(LogLevel::Trace,"TCPClient",sData.c_str());
}

void ModbusTCPClient::process_multiple_register(void){
    uint16_t ui16RegTxIndex=0;
    uint16_t ui16RegTxNumber=0;
    uint16_t ui16RegRxIndex=0;
    uint16_t ui16RegRxNumber=0;
    String sData;

    ui16RegTxIndex = ModUtil.tdfParameter.aui8ModbusTxBuffer[2];
    ui16RegTxIndex <<= 8;
    ui16RegTxIndex |= ModUtil.tdfParameter.aui8ModbusTxBuffer[3];

    ui16RegRxIndex = ModUtil.tdfParameter.aui8ModbusRxBuffer[2];
    ui16RegRxIndex <<= 8;
    ui16RegRxIndex |= ModUtil.tdfParameter.aui8ModbusRxBuffer[3]; 

    ui16RegTxNumber = ModUtil.tdfParameter.aui8ModbusTxBuffer[4];
    ui16RegTxNumber <<= 8;
    ui16RegTxNumber |= ModUtil.tdfParameter.aui8ModbusTxBuffer[5];

    ui16RegRxNumber = ModUtil.tdfParameter.aui8ModbusRxBuffer[4];
    ui16RegRxNumber <<= 8;
    ui16RegRxNumber |= ModUtil.tdfParameter.aui8ModbusRxBuffer[5];

    if(ui16RegTxIndex == ui16RegRxIndex){
        if(ui16RegTxNumber == ui16RegRxNumber){
            sData = String(ui16RegTxIndex) + " registers from " + String(ui16RegTxIndex) + " write sucess"; 
        }else{
            sData = String(ui16RegTxIndex) + " registers from " + String(ui16RegTxIndex) + " write failure"; 
        }
    }else{
        sData = String(ui16RegTxIndex) + " registers from " + String(ui16RegTxIndex) + " write sucess";
    }

    elog.Write(LogLevel::Trace,"TCPClient",sData.c_str());
}

void ModbusTCPClient::process_received_data_error_none(void){
    if(tdftcpc.ui8LastReqFun == MODBUS_FUNC_READ_COIL_REG){
        process_coil_status();
    }else if(tdftcpc.ui8LastReqFun == MODBUS_FUNC_READ_INPUT_STATUS_REG){
        process_input_status();
    }else if(tdftcpc.ui8LastReqFun == MODBUS_FUNC_READ_HOLDING_REG){
        process_holding_register();
    }else if(tdftcpc.ui8LastReqFun == MODBUS_FUNC_READ_INPUT_REG){
        process_input_register();
    }else if(tdftcpc.ui8LastReqFun == MODBUS_FUNC_WRITE_SINGLE_COIL){
        process_preset_coil();
    }else if(tdftcpc.ui8LastReqFun == MODBUS_FUNC_WRITE_SINGLE_REG){
        process_write_register();
    }else if(tdftcpc.ui8LastReqFun == MODBUS_FUNC_WRITE_MULTI_COIL){
        process_multiple_coils();
    }else if(tdftcpc.ui8LastReqFun == MODBUS_FUNC_WRITE_MULTI_REG){
        process_multiple_register();
    }else{
        //devare kapadu
    }
}   

void ModbusTCPClient::process_received_data_error_crc(void){
    elog.Write(LogLevel::Trace,"TCPClient","recieved crc error");
}

void ModbusTCPClient::process_received_data_error_timeout(void){
    elog.Write(LogLevel::Trace,"TCPClient","no rx data timeout error");
}

void ModbusTCPClient::process_received_data_error_invalid_reg_addr(void){
    elog.Write(LogLevel::Trace,"TCPClient","invalid register address");
}

void ModbusTCPClient::process_received_data_error_invalid_resp_length(void){
    elog.Write(LogLevel::Trace,"TCPClient","invalid register length");
}

void ModbusTCPClient::process_received_data_error_invalid_command(void){
    elog.Write(LogLevel::Trace,"TCPClient","modbus invalid command");
}

void ModbusTCPClient::process_received_data_error_invalid_function(void){
    elog.Write(LogLevel::Trace,"TCPClient","invalid function");
}

void ModbusTCPClient::process_received_data_error_invalid_data_value(void){
    elog.Write(LogLevel::Trace,"TCPClient","invalid data value");
}

void ModbusTCPClient::process_received_data_error_gateway_failed(void){
    elog.Write(LogLevel::Trace,"TCPClient","gateway failure");
}

void ModbusTCPClient::process_received_data_error_no_new_data(void){
    elog.Write(LogLevel::Trace,"TCPClient","no new data ");
}

void ModbusTCPClient::process_received_data_error_slaveid_error(void){
    elog.Write(LogLevel::Trace,"TCPClient","wrong slave id");
}

void ModbusTCPClient::process_received_data_error_unknow(void){
    elog.Write(LogLevel::Trace,"TCPClient","modbus error unknow ");
}   

void ModbusTCPClient::set_server_ip_address(IPAddress IP){
	tdftcpc.IPServerAddr = IP;
}

IPAddress ModbusTCPClient::get_server_ip_address(void){
	return tdftcpc.IPServerAddr; 
}

void ModbusTCPClient::set_server_port(uint16_t ui16Port){
	tdftcpc.ui16ServerPort = ui16Port;
}
uint16_t ModbusTCPClient::get_server_port(void){
	return tdftcpc.ui16ServerPort;
}

void ModbusTCPClient::set_device_id(uint8_t ui8DeviceId){
	tdftcpc.ui8LastDeviceId = ui8DeviceId;
}

uint8_t ModbusTCPClient::get_device_id(void){
	return tdftcpc.ui8LastDeviceId;
}

//void ModbusTCPClient::onData(esp32Modbus::MBTCPOnData handler) {
//  _onDataHandler = handler;
//}

//void esp32ModbusTCP::onError(esp32Modbus::MBTCPOnError handler) {
//  _onErrorHandler = handler;
//}

//uint16_t ModbusTCPClient::readDiscreteInputs(uint16_t address, uint16_t numberInputs) {
  //esp32ModbusTCPInternals::ModbusRequest* request =
  //  new esp32ModbusTCPInternals::ModbusRequest02(_serverID, address, numberInputs);
  //return _addToQueue(request);
//}

//uint16_t esp32ModbusTCP::readHoldingRegisters(uint16_t address, uint16_t numberRegisters) {
//  esp32ModbusTCPInternals::ModbusRequest* request =
//    new esp32ModbusTCPInternals::ModbusRequest03(_serverID, address, numberRegisters);
//  return _addToQueue(request);
//}

/*uint16_t esp32ModbusTCP::readInputRegisters(uint16_t address, uint16_t numberRegisters) {
  esp32ModbusTCPInternals::ModbusRequest* request =
    new esp32ModbusTCPInternals::ModbusRequest04(_serverID, address, numberRegisters);
  return _addToQueue(request);
}*/

/*uint16_t esp32ModbusTCP::_addToQueue(esp32ModbusTCPInternals::ModbusRequest* request) {
  if (uxQueueSpacesAvailable(_queue) > 0) {
    if (xQueueSend(_queue, &request, (TickType_t) 10) == pdPASS) {
      _processQueue();
      return request->getId();
    }
  }
  delete request;
  return 0;
}*/

/************************ TCP Handling *******************************/

void ModbusTCPClient::connect_to_server(uint8_t ui8DeviceId, IPAddress IP, uint16_t ui16Port){
    _client.connect(IP, ui16Port);
    tdftcpc.ui8TCPState = TCP_STATE_CONNECTING;
    tdftcpc.ui8LastDeviceId = ui8DeviceId;
    tdftcpc.IPServerAddr = IP;
    tdftcpc.ui16ServerPort = ui16Port;
}

void ModbusTCPClient::disconnect_from_server(void){
	tdftcpc.ui8TCPState = TCP_STATE_DISCONNECTING;
	_client.close(false);
}

void ModbusTCPClient::_onConnected(void* mb, AsyncClient* client) {
    elog.Write(LogLevel::Trace,"TCPClient","connected to server");
	ModbusTCPClient* obj = static_cast<ModbusTCPClient*>(mb);  
	obj->tdftcpc.ui8TCPState = TCP_STATE_CONNECTED; 
  //o->_state = IDLE;
  //o->_lastMillis = millis();
  	//obj->_processQueue();
}

void ModbusTCPClient::_onDisconnected(void* mb, AsyncClient* client) {
	elog.Write(LogLevel::Trace,"TCPClient","disconnected from server");
  	ModbusTCPClient* obj = static_cast<ModbusTCPClient*>(mb);
	obj->tdftcpc.ui8TCPState = TCP_STATE_DISCONNECTED;
  //esp32ModbusTCP* o = reinterpret_cast<esp32ModbusTCP*>(mb);
  //o->_state = NOTCONNECTED;
 // o->_lastMillis = millis();
 // o->_processQueue();
}

void ModbusTCPClient::_onError(void* mb, AsyncClient* client, int8_t error) {
	elog.Write(LogLevel::Trace,"TCPClient","error");
	ModbusTCPClient* obj =static_cast<ModbusTCPClient*>(mb);
	obj->tdftcpc.ui8TCPState = TCP_STATE_ERROR;
	obj->tdftcpc.bCommunicationDone = true;
  /*esp32ModbusTCP* o = reinterpret_cast<esp32ModbusTCP*>(mb);
  if (o->_state == IDLE || o->_state == DISCONNECTING) {
    log_w("unexpected tcp error");
    return;
  }
  o->_tryError(esp32Modbus::COMM_ERROR);*/
}

void ModbusTCPClient::_onTimeout(void* mb, AsyncClient* client, uint32_t time) {
  	elog.Write(LogLevel::Trace,"TCPClient","timeout");
	ModbusTCPClient* obj = static_cast<ModbusTCPClient*>(mb);
	obj->tdftcpc.ui8TCPState = TCP_STATE_TIMEOUT;
	obj->tdftcpc.bCommunicationDone = true;

	  /*esp32ModbusTCP* o = reinterpret_cast<esp32ModbusTCP*>(mb);
  if (o->_state < WAITING) {
    log_w("unexpected tcp timeout");
    return;
  }
  o->_tryError(esp32Modbus::TIMEOUT);*/
}

void ModbusTCPClient::_onData(void* mb, AsyncClient* client, void* data, size_t length) {
	
	elog.Write(LogLevel::Trace,"TCPClient","data received");
  	String sData = elog.get_array_to_string("Data",reinterpret_cast<char*>(data),length,1);
  	elog.Write(LogLevel::Trace,"TCPClient",sData.c_str());

	ModbusTCPClient* obj = static_cast<ModbusTCPClient*>(mb);
	obj->tdftcpc.ui8TCPState = TCP_STATE_DATA_RECEIVED;
	obj->get_data_to_buffer(reinterpret_cast<char*>(data),length);

	obj->tdftcpc.ui16TCPRxCounter = length; 
	
}

void ModbusTCPClient::_onPoll(void* mb, AsyncClient* client) {
	/*ModbusTCPClient* obj = static_cast<ModbusTCPClient*>(mb);
  	if (millis() - o->_lastMillis > MB_IDLE_DICONNECT_TIME) {
    	log_v("idle time disconnecting");
    	obj->_disconnect();
  	}*/
}

/*void esp32ModbusTCP::_processQueue() {
  if (_state == NOTCONNECTED && uxQueueMessagesWaiting(_queue) > 0) {
    _connect();
    return;
  }
  if (_state == CONNECTING ||
      _state == WAITING ||
      _state == DISCONNECTING ||
      uxQueueSpacesAvailable(_queue) == 0 ||
      !_client.canSend()) {
    return;
  }
  esp32ModbusTCPInternals::ModbusRequest* req = nullptr;
  if (xQueuePeek(_queue, &req, (TickType_t)20)) {
    _state = WAITING;
    log_v("send");
    _client.add(reinterpret_cast<char*>(req->getMessage()), req->getSize());
    _client.send();
    _lastMillis = millis();
  }
}

void esp32ModbusTCP::_tryError(esp32Modbus::Error error) {
  esp32ModbusTCPInternals::ModbusRequest* req = nullptr;
  if (xQueuePeek(_queue, &req, (TickType_t)10)) {
    if (_onErrorHandler) _onErrorHandler(req->getId(), error);
  }
  _next();
}

void esp32ModbusTCP::_tryData(esp32ModbusTCPInternals::ModbusResponse* response) {
  if (_onDataHandler) _onDataHandler(
      response->getId(),
      response->getSlaveAddress(),
      response->getFunctionCode(),
      response->getData(),
      response->getByteCount());
  _next();
}

void esp32ModbusTCP::_next() {
  esp32ModbusTCPInternals::ModbusRequest* req = nullptr;
  if (xQueueReceive(_queue, &req, (TickType_t)20)) {
    delete req;
  }
  _lastMillis = millis();
  _state = IDLE;
  _processQueue();
}*/

void ModbusTCPClient::get_data_to_buffer(char* acData, size_t Size){
	uint16_t ui16LoopCounter=0;

	for(ui16LoopCounter=0;ui16LoopCounter<Size;ui16LoopCounter++){
		tdftcpc.aui8TCPBuffer[ui16LoopCounter] = acData[ui16LoopCounter];
	}
}

void ModbusTCPClient::test(void){
	uint8_t aui8Data[8] = {10,3,0,0,0,124,0,0};
    static unsigned long ulLastTime=0;
    static bool binit=0;
    static uint8_t  ui8Type=0;

    if(binit == 0){
        binit = 1;
        ulLastTime = millis();
    }

    if((millis() - ulLastTime) > 5000){
        ulLastTime = millis();
        //prepare_packet(aui8Data,6,0);
        if(ui8Type == 0){
            read_coil_status({192,168,43,77},502,10,0,10,2000);
        }else if(ui8Type == 1){
            read_input_status({192,168,43,77},502,10,0,10,2000);
        }else if(ui8Type == 2){
            read_holding_register({192,168,43,77},502,10,0,10,2000);
        }else if(ui8Type == 3){
            read_input_register({192,168,43,77},502,10,0,10,2000);
        }else if(ui8Type == 4){
            write_coil({192,168,43,77},502,10,0,1,2000);
        }else if(ui8Type == 5){
            write_register({192,168,43,77},502,10,0,23,2000);
        }else if(ui8Type == 6){
            uint8_t aui8Data1[2] = {0x00,0xFF};
            write_multiple_coils({192,168,43,77},502,10,0,aui8Data1,10,2000);
        }else if(ui8Type == 7){
            uint8_t aui8Data1[16] = {0x00,0x00,0x00,0x01,0x00,0x02,0x00,0x03,0x00,0x04,0x00,0x05,0x00,0x06,0x00,0x07};
            write_multiple_registers({192,168,43,77},502,10,0,aui8Data1,8,2000);
        }else{
            //devare kapadu
        }

        ui8Type++;
        if(ui8Type > 7){
            ui8Type = 0;
        }
    }
}


// Preinstantiate Objects //////////////////////////////////////////////////////
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MODBUSTCPCLIENT)
ModbusTCPClient tcpc;
#endif