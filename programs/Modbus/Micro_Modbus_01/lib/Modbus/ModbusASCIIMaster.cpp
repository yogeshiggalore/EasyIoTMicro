/*******************************************************************************
*File Name: ModbusASCIIMaster.cpp
*
* Version: 1.0
*
* Description:
* In this source file for modbus ascii master application
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
#include<ModbusASCIIMaster.h>
#include<LogLevel.h>
#include<logger.h>

ModbusASCIIMaster::ModbusASCIIMaster(){}

void ModbusASCIIMaster::update(void){
    read();
    process();
    write();
}

void ModbusASCIIMaster::read(void){
    if(is_data_received()){
        tdfascim.ui16ReadResult = 1;
        tdfascim.ui16ReadResult <<= 8;
        tdfascim.ui16ReadResult |= tdfascim.ui16ModbusRxCounter;
    }
}

void ModbusASCIIMaster::process(void){
    uint8_t ui8Result=0;
    ui8Result = tdfascim.ui16ReadResult >> 8;

    if(ui8Result){
        if(tdfascim.ui8ModbusError == MODBUS_ERR_NONE){
            process_received_data_error_none();
        }else if(tdfascim.ui8ModbusError == MODBUS_ERR_CRC){
            process_received_data_error_crc();
        }else if(tdfascim.ui8ModbusError == MODBUS_ERR_TIMEOUT){
            process_received_data_error_timeout();
        }else if(tdfascim.ui8ModbusError == MODBUS_INVALID_REG_ADDRESS){
            process_received_data_error_invalid_reg_addr();
        }else if(tdfascim.ui8ModbusError == MODBUS_INVALID_RESPONSE_LEN){
            process_received_data_error_invalid_resp_length();
        }else if(tdfascim.ui8ModbusError == MODBUS_INVALID_MODBUS_COMMAND){
            process_received_data_error_invalid_command();
        }else if(tdfascim.ui8ModbusError == MODBUS_INVALID_FUNC){
            process_received_data_error_invalid_function();
        }else if(tdfascim.ui8ModbusError == MODBUS_INVALID_DATA_VALUE){
            process_received_data_error_invalid_data_value();
        }else if(tdfascim.ui8ModbusError == MODBUS_GATEWAY_FAILED){
            process_received_data_error_gateway_failed();
        }else if(tdfascim.ui8ModbusError == MODBUS_NO_NEW_DATA){
            process_received_data_error_no_new_data();
        }else if(tdfascim.ui8ModbusError == MODBUS_SLAVEID_ERROR){
            process_received_data_error_slaveid_error();
        }else if(tdfascim.ui8ModbusError == MODBUS_ERR_UNKNOWN){
            process_received_data_error_unknow();
        }else{
            //devare kapadu
        }

        tdfascim.ui16ReadResult = 0;
        tdfascim.ui16ModbusRxCounter = 0;
        tdfascim.bCommunicationDone = false;
        tdfascim.bCommunicationInit = false;
    }
}

void ModbusASCIIMaster::read_coil_status(uint8_t ui8DeviceId, uint16_t ui16CoilStartAddr, uint16_t ui16NumberOfCoils, uint16_t ui16Timeout){
    uint16_t ui16CRC;

    ModUtil.tdfParameter.aui8ModbusTxBuffer[0] = ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[1] = MODBUS_FUNC_READ_COIL_REG;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[2] = ui16CoilStartAddr >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[3] = ui16CoilStartAddr;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[4] = ui16NumberOfCoils >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[5] = ui16NumberOfCoils;
    
    ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,6);
    ModUtil.tdfParameter.aui8ModbusTxBuffer[6] = ui16CRC >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[7] = ui16CRC;

    tdfascim.ui8LastDeviceId = ui8DeviceId;
    tdfascim.ui8LastReqFun = MODBUS_FUNC_READ_COIL_REG;
    tdfascim.ui8ExpectedBytes = ModUtil.expected_bytes_rtu(ModUtil.tdfParameter.aui8ModbusTxBuffer);
    tdfascim.ui16WaitTimeValue = ui16Timeout;
    tdfascim.ui16PrepareResult = 1;
    tdfascim.ui16PrepareResult <<= 8;
    tdfascim.ui16PrepareResult = tdfascim.ui16PrepareResult | 8;

    elog.Write(LogLevel::Trace,"ASCIIMaster","read coil status ");
}

void ModbusASCIIMaster::read_input_status(uint8_t ui8DeviceId,uint16_t ui16InputStartAddr, uint16_t ui16NumberOfInputs, uint16_t ui16Timeout){
    uint16_t ui16CRC;

    ModUtil.tdfParameter.aui8ModbusTxBuffer[0] = ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[1] = MODBUS_FUNC_READ_INPUT_STATUS_REG;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[2] = ui16InputStartAddr >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[3] = ui16InputStartAddr;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[4] = ui16NumberOfInputs >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[5] = ui16NumberOfInputs;
    
    ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,6);
    ModUtil.tdfParameter.aui8ModbusTxBuffer[6] = ui16CRC >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[7] = ui16CRC;

    tdfascim.ui8LastDeviceId = ui8DeviceId;
    tdfascim.ui8LastReqFun = MODBUS_FUNC_READ_INPUT_STATUS_REG;
    tdfascim.ui8ExpectedBytes = ModUtil.expected_bytes_rtu(ModUtil.tdfParameter.aui8ModbusTxBuffer);
    tdfascim.ui16WaitTimeValue = ui16Timeout;
    tdfascim.ui16PrepareResult = 1;
    tdfascim.ui16PrepareResult <<= 8;
    tdfascim.ui16PrepareResult = tdfascim.ui16PrepareResult | 8;

    elog.Write(LogLevel::Trace,"ASCIIMaster","read input status ");
}

void ModbusASCIIMaster::read_holding_register(uint8_t ui8DeviceId, uint16_t ui16StartAddr, uint16_t ui16NumberOfReg, uint16_t ui16Timeout){
    uint16_t ui16CRC;

    ModUtil.tdfParameter.aui8ModbusTxBuffer[0] = ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[1] = MODBUS_FUNC_READ_HOLDING_REG;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[2] = ui16StartAddr >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[3] = ui16StartAddr;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[4] = ui16NumberOfReg >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[5] = ui16NumberOfReg;
    
    ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,6);
    ModUtil.tdfParameter.aui8ModbusTxBuffer[6] = ui16CRC >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[7] = ui16CRC;

    tdfascim.ui8LastDeviceId = ui8DeviceId;
    tdfascim.ui8LastReqFun = MODBUS_FUNC_READ_HOLDING_REG;
    tdfascim.ui8ExpectedBytes = ModUtil.expected_bytes_rtu(ModUtil.tdfParameter.aui8ModbusTxBuffer);
    tdfascim.ui16WaitTimeValue = ui16Timeout;
    tdfascim.ui16PrepareResult = 1;
    tdfascim.ui16PrepareResult <<= 8;
    tdfascim.ui16PrepareResult = tdfascim.ui16PrepareResult | 8;

    elog.Write(LogLevel::Trace,"ASCIIMaster","read holding register");
}

void ModbusASCIIMaster::read_input_register(uint8_t ui8DeviceId,uint16_t ui16StartAddr, uint16_t ui16NumberOfReg, uint16_t ui16Timeout){
    uint16_t ui16CRC;

    ModUtil.tdfParameter.aui8ModbusTxBuffer[0] = ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[1] = MODBUS_FUNC_READ_INPUT_REG;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[2] = ui16StartAddr >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[3] = ui16StartAddr;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[4] = ui16NumberOfReg >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[5] = ui16NumberOfReg;
    
    ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,6);
    ModUtil.tdfParameter.aui8ModbusTxBuffer[6] = ui16CRC >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[7] = ui16CRC;

    tdfascim.ui8LastDeviceId = ui8DeviceId;
    tdfascim.ui8LastReqFun = MODBUS_FUNC_READ_INPUT_REG;
    tdfascim.ui8ExpectedBytes = ModUtil.expected_bytes_rtu(ModUtil.tdfParameter.aui8ModbusTxBuffer);
    tdfascim.ui16WaitTimeValue = ui16Timeout;
    tdfascim.ui16PrepareResult = 1;
    tdfascim.ui16PrepareResult <<= 8;
    tdfascim.ui16PrepareResult = tdfascim.ui16PrepareResult | 8;

    elog.Write(LogLevel::Trace,"ASCIIMaster","read input register");
}

void ModbusASCIIMaster::write_coil(uint8_t ui8DeviceId, uint16_t ui16CoilIndex,bool bValue, uint16_t ui16Timeout){
    uint16_t ui16CRC;

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

    tdfascim.ui8LastDeviceId = ui8DeviceId;
    tdfascim.ui8LastReqFun = MODBUS_FUNC_WRITE_SINGLE_COIL;
    tdfascim.ui8ExpectedBytes = 8;
    tdfascim.ui16WaitTimeValue = ui16Timeout;
    tdfascim.ui16PrepareResult = 1;
    tdfascim.ui16PrepareResult <<= 8;
    tdfascim.ui16PrepareResult = tdfascim.ui16PrepareResult | 8;

    elog.Write(LogLevel::Trace,"ASCIIMaster","write coil");
}

void ModbusASCIIMaster::write_register(uint8_t ui8DeviceId,uint16_t ui16RegIndex, uint16_t ui16RegValue, uint16_t ui16Timeout){
    uint16_t ui16CRC;

    ModUtil.tdfParameter.aui8ModbusTxBuffer[0] = ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[1] = MODBUS_FUNC_WRITE_SINGLE_REG;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[2] = ui16RegIndex >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[3] = ui16RegIndex;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[4] = ui16RegValue >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[5] = ui16RegValue;
    

    ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,6);
    ModUtil.tdfParameter.aui8ModbusTxBuffer[6] = ui16CRC >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[7] = ui16CRC;

    tdfascim.ui8LastDeviceId = ui8DeviceId;
    tdfascim.ui8LastReqFun = MODBUS_FUNC_WRITE_SINGLE_REG;
    tdfascim.ui8ExpectedBytes = 8;
    tdfascim.ui16WaitTimeValue = ui16Timeout;
    tdfascim.ui16PrepareResult = 1;
    tdfascim.ui16PrepareResult <<= 8;
    tdfascim.ui16PrepareResult = tdfascim.ui16PrepareResult | 8;

    elog.Write(LogLevel::Trace,"ASCIIMaster","write register");
}

void ModbusASCIIMaster::write_multiple_coils(uint8_t ui8DeviceId, uint16_t ui16CoilStartIndex,uint8_t* aui8Data,uint16_t ui16NumberOfCoils, uint16_t ui16Timeout){
    uint16_t ui16CRC;
    uint8_t ui8ArrayCounter=0;
    uint8_t ui8ByteCount=0;
    uint8_t ui8LoopCounter=0;

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

    tdfascim.ui8LastDeviceId = ui8DeviceId;
    tdfascim.ui8LastReqFun = MODBUS_FUNC_WRITE_MULTI_COIL;
    tdfascim.ui8ExpectedBytes = ModUtil.expected_bytes_rtu(ModUtil.tdfParameter.aui8ModbusTxBuffer);
    tdfascim.ui16WaitTimeValue = ui16Timeout;
    tdfascim.ui16PrepareResult = 1;
    tdfascim.ui16PrepareResult <<= 8;
    tdfascim.ui16PrepareResult = tdfascim.ui16PrepareResult | ui8ArrayCounter;

    elog.Write(LogLevel::Trace,"ASCIIMaster","write multiple coils");
}

void ModbusASCIIMaster::write_multiple_registers(uint8_t ui8DeviceId,uint16_t ui16StartRegIndex,uint8_t* aui8Data,uint16_t ui16NumberOfReg, uint16_t ui16Timeout){
    uint16_t ui16CRC;
    uint8_t ui8ArrayCounter=0;
    uint8_t ui8ByteCount=0;
    uint8_t ui8LoopCounter=0;

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

    tdfascim.ui8LastDeviceId = ui8DeviceId;
    tdfascim.ui8LastReqFun = MODBUS_FUNC_WRITE_MULTI_REG;
    tdfascim.ui8ExpectedBytes = ModUtil.expected_bytes_rtu(ModUtil.tdfParameter.aui8ModbusTxBuffer);
    tdfascim.ui16WaitTimeValue = ui16Timeout;
    tdfascim.ui16PrepareResult = 1;
    tdfascim.ui16PrepareResult <<= 8;
    tdfascim.ui16PrepareResult = tdfascim.ui16PrepareResult | ui8ArrayCounter;

    elog.Write(LogLevel::Trace,"ASCIIMaster","write multiple registers");
}

void ModbusASCIIMaster::process_coil_status(void){
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

    elog.Write(LogLevel::Trace,"ASCIIMaster",sData.c_str());
}

void ModbusASCIIMaster::process_input_status(void){
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

    elog.Write(LogLevel::Trace,"ASCIIMaster",sData.c_str());
}

void ModbusASCIIMaster::process_holding_register(void){
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

    elog.Write(LogLevel::Trace,"ASCIIMaster",sData.c_str());
}

void ModbusASCIIMaster::process_input_register(void){
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

    elog.Write(LogLevel::Trace,"ASCIIMaster",sData.c_str());
}

void ModbusASCIIMaster::process_preset_coil(void){
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

    elog.Write(LogLevel::Trace,"ASCIIMaster",sData.c_str());
}

void ModbusASCIIMaster::process_write_register(void){
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

    elog.Write(LogLevel::Trace,"ASCIIMaster",sData.c_str());
}

void ModbusASCIIMaster::process_multiple_coils(void){
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

    elog.Write(LogLevel::Trace,"ASCIIMaster",sData.c_str());
}

void ModbusASCIIMaster::process_multiple_register(void){
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

    elog.Write(LogLevel::Trace,"ASCIIMaster",sData.c_str());
}

void ModbusASCIIMaster::process_received_data_error_none(void){
    if(tdfascim.ui8LastReqFun == MODBUS_FUNC_READ_COIL_REG){
        process_coil_status();
    }else if(tdfascim.ui8LastReqFun == MODBUS_FUNC_READ_INPUT_STATUS_REG){
        process_input_status();
    }else if(tdfascim.ui8LastReqFun == MODBUS_FUNC_READ_HOLDING_REG){
        process_holding_register();
    }else if(tdfascim.ui8LastReqFun == MODBUS_FUNC_READ_INPUT_REG){
        process_input_register();
    }else if(tdfascim.ui8LastReqFun == MODBUS_FUNC_WRITE_SINGLE_COIL){
        process_preset_coil();
    }else if(tdfascim.ui8LastReqFun == MODBUS_FUNC_WRITE_SINGLE_REG){
        process_write_register();
    }else if(tdfascim.ui8LastReqFun == MODBUS_FUNC_WRITE_MULTI_COIL){
        process_multiple_coils();
    }else if(tdfascim.ui8LastReqFun == MODBUS_FUNC_WRITE_MULTI_REG){
        process_multiple_register();
    }else{
        //devare kapadu
    }
}   

void ModbusASCIIMaster::process_received_data_error_crc(void){
    elog.Write(LogLevel::Trace,"ASCIIMaster","recieved crc error");
}

void ModbusASCIIMaster::process_received_data_error_timeout(void){
    elog.Write(LogLevel::Trace,"ASCIIMaster","no rx data timeout error");
}

void ModbusASCIIMaster::process_received_data_error_invalid_reg_addr(void){
    elog.Write(LogLevel::Trace,"ASCIIMaster","invalid register address");
}

void ModbusASCIIMaster::process_received_data_error_invalid_resp_length(void){
    elog.Write(LogLevel::Trace,"ASCIIMaster","invalid register length");
}

void ModbusASCIIMaster::process_received_data_error_invalid_command(void){
    elog.Write(LogLevel::Trace,"ASCIIMaster","modbus invalid command");
}

void ModbusASCIIMaster::process_received_data_error_invalid_function(void){
    elog.Write(LogLevel::Trace,"ASCIIMaster","invalid function");
}

void ModbusASCIIMaster::process_received_data_error_invalid_data_value(void){
    elog.Write(LogLevel::Trace,"ASCIIMaster","invalid data value");
}

void ModbusASCIIMaster::process_received_data_error_gateway_failed(void){
    elog.Write(LogLevel::Trace,"ASCIIMaster","gateway failure");
}

void ModbusASCIIMaster::process_received_data_error_no_new_data(void){
    elog.Write(LogLevel::Trace,"ASCIIMaster","no new data ");
}

void ModbusASCIIMaster::process_received_data_error_slaveid_error(void){
    elog.Write(LogLevel::Trace,"ASCIIMaster","wrong slave id");
}

void ModbusASCIIMaster::process_received_data_error_unknow(void){
    elog.Write(LogLevel::Trace,"ASCIIMaster","modbus error unknow ");
}   

void ModbusASCIIMaster::write(void){
    uint8_t ui8Result=0;
    ui8Result = tdfascim.ui16PrepareResult >> 8;
    if(ui8Result){
        uint8_t ui8ByteSize=0;
        uint16_t ui16LoopCounter=0;
        uint16_t ui16ASCIIByte=0;
        ui8ByteSize = tdfascim.ui16PrepareResult;
        if(ui8ByteSize > 0){
            ModUtil.pin_control(1);
            tdfascim.ui16PrepareResult = 0;
            tdfascim.ui16WriteResult = 1;
            tdfascim.ui16WriteResult <<= 8;
            tdfascim.ui16WriteResult |= ui8ByteSize;
            tdfascim.ui8LastDeviceId = ModUtil.tdfParameter.aui8ModbusTxBuffer[0];
            tdfascim.ui8ExpectedBytes = ModUtil.expected_bytes_rtu(ModUtil.tdfParameter.aui8ModbusTxBuffer);
            elog.Write(LogLevel::Trace,"ASCIIMaster", elog.get_array_to_string("rtu",ModUtil.tdfParameter.aui8ModbusTxBuffer,ui8ByteSize,PRINT_HEX).c_str());
            ui16ASCIIByte = ModUtil.convert_rtu_to_ascii(ModUtil.tdfParameter.aui8ModbusTxBuffer,tdfascim.aui8ASCIIBuffer,ui8ByteSize,1,1); 
            elog.Write(LogLevel::Trace,"ASCIIMaster", elog.get_array_to_string("tx",tdfascim.aui8ASCIIBuffer,ui16ASCIIByte,PRINT_HEX).c_str());
            for(ui16LoopCounter=0;ui16LoopCounter<ui16ASCIIByte;ui16LoopCounter++){
                ModbusUART.write(tdfascim.aui8ASCIIBuffer[ui16LoopCounter]);
                while(Serial.availableForWrite() < 127);
            }
            delay(1);
            ModUtil.pin_control(0);
            
            tdfascim.ui16ModbusRxCounter = 0;
            tdfascim.ui16ModbusASCIIRxCounter = 0;
            tdfascim.ui16ModbusRxCounter = MODBUS_ERR_NONE;
            tdfascim.bCommunicationDone = false;
            tdfascim.bCommunicationInit = true;
            tdfascim.ulModbusTxTime = millis();
        }
    }
}

void ModbusASCIIMaster::prepare_packet(uint8_t* aui8Data, uint8_t ui8ArraySize, uint8_t ui8DataWithCRC){
    uint8_t ui8LoopCounter=0;

    for(ui8LoopCounter=0;ui8LoopCounter<ui8ArraySize;ui8LoopCounter++){
        ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8LoopCounter] = aui8Data[ui8LoopCounter];
    }

    if(ui8DataWithCRC == 0){
        uint16_t ui16CRC;
        ui16CRC = ModUtil.get_crc16(aui8Data,ui8ArraySize);
        ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArraySize++] = ui16CRC >> 8;
        ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArraySize++] = ui16CRC;
        tdfascim.ui16PrepareResult = 1;
        tdfascim.ui16PrepareResult <<= 8;
        tdfascim.ui16PrepareResult = tdfascim.ui16PrepareResult | ui8ArraySize;
        elog.Write(LogLevel::Trace,"ASCIIMaster", elog.get_array_to_string("PP",ModUtil.tdfParameter.aui8ModbusTxBuffer,(ui8ArraySize+2),PRINT_HEX).c_str());
    }else{
        tdfascim.ui16PrepareResult = 1;
        tdfascim.ui16PrepareResult <<= 8;
        tdfascim.ui16PrepareResult = tdfascim.ui16PrepareResult | ui8ArraySize;
        elog.Write(LogLevel::Trace,"ASCIIMaster", elog.get_array_to_string("PP",ModUtil.tdfParameter.aui8ModbusTxBuffer,(ui8ArraySize),PRINT_HEX).c_str());
    }
}

bool ModbusASCIIMaster::read_ascii_bytes(void){
    
    while(ModbusUART.available()){
        tdfascim.aui8ASCIIBuffer[tdfascim.ui16ModbusASCIIRxCounter++] = ModbusUART.read();
    }

    if(tdfascim.aui8ASCIIBuffer[0] != ':'){
        tdfascim.ui16ModbusASCIIRxCounter = 0;
    }
    
    if((tdfascim.aui8ASCIIBuffer[tdfascim.ui16ModbusASCIIRxCounter-2] == 0x0D) && (tdfascim.aui8ASCIIBuffer[tdfascim.ui16ModbusASCIIRxCounter-1] == 0x0A)){
        tdfascim.ui16ModbusRxCounter = ModUtil.convert_ascii_to_rtu(tdfascim.aui8ASCIIBuffer,ModUtil.tdfParameter.aui8ModbusRxBuffer,tdfascim.ui16ModbusASCIIRxCounter,1,1,1);
        elog.Write(LogLevel::Trace,"ASCIIMaster", elog.get_array_to_string("Rx",tdfascim.aui8ASCIIBuffer,tdfascim.ui16ModbusASCIIRxCounter,PRINT_HEX).c_str());
        tdfascim.ui16ModbusASCIIRxCounter = 0;
    }
}

bool ModbusASCIIMaster::is_data_received(void){
    
    if(tdfascim.bCommunicationDone)
        return tdfascim.bCommunicationDone;

    read_ascii_bytes();

    if(tdfascim.ui16ModbusRxCounter > 0){
        if(ModUtil.tdfParameter.aui8ModbusRxBuffer[0] != tdfascim.ui8LastDeviceId){
            tdfascim.ui16ModbusRxCounter = 0;
            tdfascim.bCommunicationDone = true;
            tdfascim.ui8ModbusError = MODBUS_SLAVEID_ERROR;
            tdfascim.ui16ResponseTime = millis() - tdfascim.ulModbusTxTime;
            String sPrintData = "MODBUS_SLAVEID_ERROR Time: " + String(tdfascim.ui16ResponseTime) + " ms";
            elog.Write(LogLevel::Error,"ASCIIMaster",sPrintData.c_str());
        }else{
            if(tdfascim.ui16ModbusRxCounter >= MODBUS_RTU_MANDATORY_RESPONSE_BYTES){
                uint8_t ui8FunCodeResponse=0;
                ui8FunCodeResponse = ModUtil.verify_function_code(ModUtil.tdfParameter.aui8ModbusRxBuffer[1]);
                if(ui8FunCodeResponse == 0){
                    tdfascim.bCommunicationDone = true;
                    tdfascim.ui8ModbusError = MODBUS_INVALID_FUNC;
                    tdfascim.ui16ResponseTime = millis() - tdfascim.ulModbusTxTime;
                    String sPrintData = "MODBUS_INVALID_FUNC Time: " + String(tdfascim.ui16ResponseTime) + " ms bytes:" + String(tdfascim.ui16ModbusRxCounter);
                    elog.Write(LogLevel::Error,"ASCIIMaster",sPrintData.c_str());
                }else if(ui8FunCodeResponse == 2){
                    tdfascim.bCommunicationDone = true;
                    tdfascim.ui8ModbusError = ModUtil.get_exception(ModUtil.tdfParameter.aui8ModbusRxBuffer[2]);
                    tdfascim.ui16ResponseTime = millis() - tdfascim.ulModbusTxTime;
                    String sPrintData = "MODBUS_EXCEPTION " + String(tdfascim.ui8ModbusError) + " Time: " + String(tdfascim.ui16ResponseTime) + " ms bytes:" + String(tdfascim.ui16ModbusRxCounter);
                    elog.Write(LogLevel::Error,"ASCIIMaster",sPrintData.c_str());
                }else{
                    if(tdfascim.ui16ModbusRxCounter >= tdfascim.ui8ExpectedBytes){
                        uint16_t ui16CRC;
                        uint8_t  ui8CRCLow;
                        uint8_t  ui8CRCHigh;
                        uint8_t  ui8CRCCompLow;
                        uint8_t  ui8CRCCompHigh;

                        ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusRxBuffer,tdfascim.ui16ModbusRxCounter-2);
                        ui8CRCLow = ui16CRC >> 8;
                        ui8CRCHigh = ui16CRC;
                        ui8CRCCompLow = ModUtil.tdfParameter.aui8ModbusRxBuffer[tdfascim.ui16ModbusRxCounter-2];
                        ui8CRCCompHigh = ModUtil.tdfParameter.aui8ModbusRxBuffer[tdfascim.ui16ModbusRxCounter-1];

                        if((ui8CRCLow == ui8CRCCompLow) && (ui8CRCHigh == ui8CRCCompHigh)){
                            tdfascim.bCommunicationDone = true;
                            tdfascim.ui8ModbusError = MODBUS_ERR_NONE;
                            tdfascim.ui16ResponseTime = millis() - tdfascim.ulModbusTxTime;
                            String sPrintData =" Time: " + String(tdfascim.ui16ResponseTime) + " ms bytes:" + String(tdfascim.ui16ModbusRxCounter) + " rtu";
                            elog.Write(LogLevel::Trace,"ASCIIMaster", elog.get_array_to_string(sPrintData,ModUtil.tdfParameter.aui8ModbusRxBuffer,tdfascim.ui16ModbusRxCounter,PRINT_HEX).c_str());
                        }else{
                            tdfascim.bCommunicationDone = true;
                            tdfascim.ui8ModbusError = MODBUS_INVALID_RESPONSE_LEN;
                            tdfascim.ui16ResponseTime = millis() - tdfascim.ulModbusTxTime;
                            String sPrintData ="MODBUS_INVALID_RESPONSE_LEN  Time: " + String(tdfascim.ui16ResponseTime) + " ms bytes:" + String(tdfascim.ui16ModbusRxCounter) + " rtu";
                            elog.Write(LogLevel::Error,"ASCIIMaster",sPrintData.c_str());
                        }
                    }
                }
            }
        }
    }

    if(tdfascim.bCommunicationInit == true){
        if(tdfascim.bCommunicationDone == false){
            if((millis() - tdfascim.ulModbusTxTime) > tdfascim.ui16WaitTimeValue){
                tdfascim.bCommunicationDone = true;
                tdfascim.ui8ModbusError = MODBUS_ERR_TIMEOUT;
                String sPrintData = "MODBUS_ERR_TIMEOUT " + String(tdfascim.ui16WaitTimeValue);
                elog.Write(LogLevel::Error,"ASCIIMaster",sPrintData.c_str());
            }
        }
    }
    
    
    return tdfascim.bCommunicationDone;
}

void ModbusASCIIMaster::test(void){
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
            read_coil_status(10,0,10,1000);
        }else if(ui8Type == 1){
            read_input_status(10,0,10,1000);
        }else if(ui8Type == 2){
            read_holding_register(10,0,10,1000);
        }else if(ui8Type == 3){
            read_input_register(10,0,10,1000);
        }else if(ui8Type == 4){
            write_coil(10,0,1,1000);
        }else if(ui8Type == 5){
            write_register(10,0,23,1000);
        }else if(ui8Type == 6){
            uint8_t aui8Data1[2] = {0x00,0xFF};
            write_multiple_coils(10,0,aui8Data1,10,1000);
        }else if(ui8Type == 7){
            uint8_t aui8Data1[16] = {0x00,0x00,0x00,0x01,0x00,0x02,0x00,0x03,0x00,0x04,0x00,0x05,0x00,0x06,0x00,0x07};
            write_multiple_registers(10,0,aui8Data1,8,1000);
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
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MODBUSASCIIMASTER)
ModbusASCIIMaster ascim;
#endif