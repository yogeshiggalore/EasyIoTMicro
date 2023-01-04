/*******************************************************************************
*File Name: ModbusRTUSlave.cpp
*
* Version: 1.0
*
* Description:
* In this source file for modbus rtu slave application
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
#include<ModbusRTUSlave.h>
#include<LogLevel.h>
#include<logger.h>

ModbusRTUSlave::ModbusRTUSlave(){}

void ModbusRTUSlave::update(void){
    read();
    process();
    write();
}

void ModbusRTUSlave::read(void){
    if(is_data_received()){
        tdfrtus.ui16ReadResult = 1;
        tdfrtus.ui16ReadResult <<= 8;
        tdfrtus.ui16ReadResult |= tdfrtus.ui16ModbusRxCounter;
    }
}

void ModbusRTUSlave::process(void){
    uint8_t ui8Result=0;

    ui8Result = tdfrtus.ui16ReadResult >> 8;
    if(ui8Result){
        if(ModUtil.tdfParameter.aui8ModbusRxBuffer[1] == MODBUS_FUNC_READ_COIL_REG){
            process_coil_status();
        }else if(ModUtil.tdfParameter.aui8ModbusRxBuffer[1] == MODBUS_FUNC_READ_INPUT_STATUS_REG){
            process_input_status();
        }else if(ModUtil.tdfParameter.aui8ModbusRxBuffer[1] == MODBUS_FUNC_READ_HOLDING_REG){
            process_holding_register();
        }else if(ModUtil.tdfParameter.aui8ModbusRxBuffer[1] == MODBUS_FUNC_READ_INPUT_REG){
            process_input_register();
        }else if(ModUtil.tdfParameter.aui8ModbusRxBuffer[1] == MODBUS_FUNC_WRITE_SINGLE_COIL){
            process_preset_coil();
        }else if(ModUtil.tdfParameter.aui8ModbusRxBuffer[1] == MODBUS_FUNC_WRITE_SINGLE_REG){
            process_write_register();
        }else if(ModUtil.tdfParameter.aui8ModbusRxBuffer[1] == MODBUS_FUNC_WRITE_MULTI_COIL){
            process_multiple_coils();
        }else if(ModUtil.tdfParameter.aui8ModbusRxBuffer[1] == MODBUS_FUNC_WRITE_MULTI_REG){
            process_multiple_register();
        }else{
            // do nothing
        }
    }
}

void ModbusRTUSlave::process_coil_status(void){
    uint16_t ui16StartCoil;
    uint16_t ui16NumberOfCoils;

    ui16StartCoil = ModUtil.tdfParameter.aui8ModbusRxBuffer[2];
    ui16StartCoil = ui16StartCoil  << 8;
    ui16StartCoil = ui16StartCoil | ModUtil.tdfParameter.aui8ModbusRxBuffer[3];

    ui16NumberOfCoils = ModUtil.tdfParameter.aui8ModbusRxBuffer[4];
    ui16NumberOfCoils = ui16NumberOfCoils  << 8;
    ui16NumberOfCoils = ui16NumberOfCoils | ModUtil.tdfParameter.aui8ModbusRxBuffer[5];

    if((ui16StartCoil >= MODBUS_RTU_COIL_STATUS_START_REG_INDEX) && (ui16StartCoil <= (MODBUS_RTU_COIL_STATUS_START_REG_INDEX + MODBUS_RTU_COIL_STATUS_SIZE))){
        if((ui16NumberOfCoils <= MODBUS_RTU_COIL_STATUS_SIZE)&& ((ui16NumberOfCoils+ui16StartCoil) <= (MODBUS_RTU_COIL_STATUS_START_REG_INDEX + MODBUS_RTU_COIL_STATUS_SIZE))){
            uint16_t ui16ActualCoilIndex;
            uint16_t ui16LoopCounter=0;
            uint8_t  ui8ByteCount=0;
            uint8_t  ui8Remainder=0;
            uint8_t  ui8BitCounter=0;
            uint8_t  ui8ByteValue=0;
            uint8_t ui8ArrayCounter=0;
            uint16_t ui16CRC;

            ui8ByteCount = ui16NumberOfCoils / 8;
            ui8Remainder = ui16NumberOfCoils % 8;
            if(ui8Remainder){
                ui8ByteCount = ui8ByteCount + 1;
            }

            ui16ActualCoilIndex = ui16StartCoil - MODBUS_RTU_COIL_STATUS_START_REG_INDEX;
            
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = tdfrtus.ui8DeviceId;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = MODBUS_FUNC_READ_COIL_REG;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui8ByteCount;

            for(ui16LoopCounter=ui16ActualCoilIndex;ui16LoopCounter<ui16NumberOfCoils;ui16LoopCounter++){
                ui8ByteValue <<= 1;
                ui8ByteValue |= tdfrtus.abCoilStatusbits[ui16LoopCounter];
                ui8BitCounter++;
                if(ui8BitCounter > 7){
                    ui8ByteValue = ((ui8ByteValue>>1)&0x55)|((ui8ByteValue<<1)&0xAA);
                    ui8ByteValue = ((ui8ByteValue>>2)&0x33)|((ui8ByteValue<<2)&0xCC);
                    ui8ByteValue = (ui8ByteValue>>4) | (ui8ByteValue<<4) ;
                    ui8BitCounter = 0;
                    ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui8ByteValue;
                    ui8ByteCount = 0;
                }
            }
            if(ui8BitCounter != 0){
                ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui8ByteValue;
            }
            
            ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,ui8ArrayCounter);
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CRC >> 8;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CRC;

            tdfrtus.ui16ProcessResult = true;
            tdfrtus.ui16ProcessResult <<= 8;
            tdfrtus.ui16ProcessResult |= ui8ArrayCounter;

        }else{
            process_errorCode(0x81, MODBUS_ILLEGAL_ADDRESS_CODE);
        }
    }else{
        process_errorCode(0x81, MODBUS_ILLEGAL_ADDRESS_CODE);
    }
}

void ModbusRTUSlave::process_input_status(void){
   uint16_t ui16StartInput;
    uint16_t ui16NumberOfInputs;

    ui16StartInput = ModUtil.tdfParameter.aui8ModbusRxBuffer[2];
    ui16StartInput = ui16StartInput  << 8;
    ui16StartInput = ui16StartInput | ModUtil.tdfParameter.aui8ModbusRxBuffer[3];

    ui16NumberOfInputs = ModUtil.tdfParameter.aui8ModbusRxBuffer[4];
    ui16NumberOfInputs = ui16NumberOfInputs  << 8;
    ui16NumberOfInputs = ui16NumberOfInputs | ModUtil.tdfParameter.aui8ModbusRxBuffer[5];

    if((ui16StartInput >= MODBUS_RTU_INPUT_STATUS_START_REG_INDEX) && (ui16StartInput <= (MODBUS_RTU_INPUT_STATUS_START_REG_INDEX + MODBUS_RTU_INPUT_STATUS_SIZE))){
        if((ui16NumberOfInputs <= MODBUS_RTU_INPUT_STATUS_SIZE)&& ((ui16NumberOfInputs+ui16StartInput) <= (MODBUS_RTU_INPUT_STATUS_START_REG_INDEX + MODBUS_RTU_INPUT_STATUS_SIZE))){
            uint16_t ui16ActualInputIndex;
            uint16_t ui16LoopCounter=0;
            uint8_t  ui8ByteCount=0;
            uint8_t  ui8Remainder=0;
            uint8_t  ui8BitCounter=0;
            uint8_t  ui8ByteValue=0;
            uint8_t ui8ArrayCounter=0;
            uint16_t ui16CRC;

            ui8ByteCount = ui16NumberOfInputs / 8;
            ui8Remainder = ui16NumberOfInputs % 8;
            if(ui8Remainder){
                ui8ByteCount = ui8ByteCount + 1;
            }

            ui16ActualInputIndex = ui16StartInput - MODBUS_RTU_COIL_STATUS_START_REG_INDEX;
            
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = tdfrtus.ui8DeviceId;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = MODBUS_FUNC_READ_INPUT_STATUS_REG;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui8ByteCount;

            for(ui16LoopCounter=ui16ActualInputIndex;ui16LoopCounter<ui16NumberOfInputs;ui16LoopCounter++){
                ui8ByteValue <<= 1;
                ui8ByteValue |= tdfrtus.abCoilStatusbits[ui16LoopCounter];
                ui8BitCounter++;
                if(ui8BitCounter > 7){
                    ui8ByteValue = ((ui8ByteValue>>1)&0x55)|((ui8ByteValue<<1)&0xAA);
                    ui8ByteValue = ((ui8ByteValue>>2)&0x33)|((ui8ByteValue<<2)&0xCC);
                    ui8ByteValue = (ui8ByteValue>>4) | (ui8ByteValue<<4) ;
                    ui8BitCounter = 0;
                    ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui8ByteValue;
                    ui8ByteCount = 0;
                }
            }
            if(ui8BitCounter != 0){
                ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui8ByteValue;
            }
            
            ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,ui8ArrayCounter);
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CRC >> 8;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CRC;

            tdfrtus.ui16ProcessResult = true;
            tdfrtus.ui16ProcessResult <<= 8;
            tdfrtus.ui16ProcessResult |= ui8ArrayCounter;

        }else{
            process_errorCode(0x82, MODBUS_ILLEGAL_ADDRESS_CODE);
        }
    }else{
        process_errorCode(0x82, MODBUS_ILLEGAL_ADDRESS_CODE);
    }
}

void ModbusRTUSlave::process_holding_register(void){
    uint16_t ui16StartReg=0;
    uint16_t ui16NumberOfReg=0;

    ui16StartReg = ModUtil.tdfParameter.aui8ModbusRxBuffer[2];
    ui16StartReg = ui16StartReg << 8;
    ui16StartReg = ui16StartReg | ModUtil.tdfParameter.aui8ModbusRxBuffer[3];

    ui16NumberOfReg = ModUtil.tdfParameter.aui8ModbusRxBuffer[4];
    ui16NumberOfReg = ui16NumberOfReg  << 8;
    ui16NumberOfReg = ui16NumberOfReg | ModUtil.tdfParameter.aui8ModbusRxBuffer[5];

    if((ui16StartReg >= MODBUS_RTU_HOLDING_START_REG_INDEX) && (ui16StartReg <= (MODBUS_RTU_HOLDING_START_REG_INDEX + MODBUS_RTU_HOLDING_REG_SIZE))){
        if((ui16NumberOfReg <= MODBUS_RTU_HOLDING_REG_SIZE) && ((ui16StartReg + ui16NumberOfReg) <= (MODBUS_RTU_HOLDING_START_REG_INDEX + MODBUS_RTU_HOLDING_REG_SIZE))){
            uint8_t ui8LoopCounter=0;
            uint8_t ui8StartIndex=0;
            uint8_t ui8NumberOfReg=0;
            uint8_t ui8ArrayCounter=0;
            uint16_t ui16CRC=0;

            ui8StartIndex = ui16StartReg - MODBUS_RTU_HOLDING_START_REG_INDEX;
            ui8NumberOfReg = ui16NumberOfReg;
            
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = tdfrtus.ui8DeviceId;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = MODBUS_FUNC_READ_HOLDING_REG;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16NumberOfReg * 2;
            
            for(ui8LoopCounter = ui8StartIndex;ui8LoopCounter<ui8NumberOfReg;ui8LoopCounter++){
                ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = tdfrtus.aui16HoldingReg[ui8LoopCounter] >> 8;
                ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = tdfrtus.aui16HoldingReg[ui8LoopCounter];
            }

            ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,ui8ArrayCounter);
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CRC >> 8;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CRC;

            tdfrtus.ui16ProcessResult = true;
            tdfrtus.ui16ProcessResult <<= 8;
            tdfrtus.ui16ProcessResult |= ui8ArrayCounter;

        }else{
            process_errorCode(0x83, MODBUS_ILLEGAL_ADDRESS_CODE);
        }
    }else{
        //illigal data address
        process_errorCode(0x83, MODBUS_ILLEGAL_ADDRESS_CODE);
    }
}

void ModbusRTUSlave::process_input_register(void){
    uint16_t ui16StartReg=0;
    uint16_t ui16NumberOfReg=0;

    ui16StartReg = ModUtil.tdfParameter.aui8ModbusRxBuffer[2];
    ui16StartReg = ui16StartReg << 8;
    ui16StartReg = ui16StartReg | ModUtil.tdfParameter.aui8ModbusRxBuffer[3];

    ui16NumberOfReg = ModUtil.tdfParameter.aui8ModbusRxBuffer[4];
    ui16NumberOfReg = ui16NumberOfReg  << 8;
    ui16NumberOfReg = ui16NumberOfReg | ModUtil.tdfParameter.aui8ModbusRxBuffer[5];

    if((ui16StartReg >= MODBUS_RTU_INPUT_START_REG_INDEX) && (ui16StartReg <= (MODBUS_RTU_INPUT_START_REG_INDEX + MODBUS_RTU_INPUT_REG_SIZE))){
        if((ui16NumberOfReg <= MODBUS_RTU_INPUT_REG_SIZE) && ((ui16StartReg + ui16NumberOfReg) <= (MODBUS_RTU_INPUT_START_REG_INDEX + MODBUS_RTU_INPUT_REG_SIZE))){
            uint8_t ui8LoopCounter=0;
            uint8_t ui8StartIndex=0;
            uint8_t ui8NumberOfReg=0;
            uint8_t ui8ArrayCounter=0;
            uint16_t ui16CRC=0;

            ui8StartIndex = ui16StartReg - MODBUS_RTU_INPUT_START_REG_INDEX;
            ui8NumberOfReg = ui16NumberOfReg;
            
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = tdfrtus.ui8DeviceId;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = MODBUS_FUNC_READ_INPUT_REG;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16NumberOfReg * 2;
            
            for(ui8LoopCounter = ui8StartIndex;ui8LoopCounter<ui8NumberOfReg;ui8LoopCounter++){
                ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = tdfrtus.aui16InputReg[ui8LoopCounter] >> 8;
                ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = tdfrtus.aui16InputReg[ui8LoopCounter];
            }

            ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,ui8ArrayCounter);
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CRC >> 8;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CRC;

            tdfrtus.ui16ProcessResult = true;
            tdfrtus.ui16ProcessResult <<= 8;
            tdfrtus.ui16ProcessResult |= ui8ArrayCounter;

        }else{
            process_errorCode(0x84, MODBUS_ILLEGAL_ADDRESS_CODE);
        }
    }else{
        //illigal data address
        process_errorCode(0x84, MODBUS_ILLEGAL_ADDRESS_CODE);
    }
}

void ModbusRTUSlave::process_preset_coil(void){
    uint16_t ui16CoilIndex=0;
    uint16_t ui16CoilValue=0;
    uint16_t ui16ActualCoilIndex=0;

    ui16CoilIndex = ModUtil.tdfParameter.aui8ModbusRxBuffer[2];
    ui16CoilIndex = ui16CoilIndex << 8;
    ui16CoilIndex = ui16CoilIndex | ModUtil.tdfParameter.aui8ModbusRxBuffer[3];

    ui16CoilValue = ModUtil.tdfParameter.aui8ModbusRxBuffer[4];
    ui16CoilValue = ui16CoilValue  << 8;
    ui16CoilValue = ui16CoilValue | ModUtil.tdfParameter.aui8ModbusRxBuffer[5];

    if((ui16CoilIndex >= MODBUS_RTU_COIL_STATUS_START_REG_INDEX) && (ui16CoilIndex <= (MODBUS_RTU_COIL_STATUS_START_REG_INDEX + MODBUS_RTU_COIL_STATUS_SIZE))){
        bool bActionTaken=false;
        uint8_t ui8ArrayCounter=0;
        uint16_t ui16CRC=0;

        ui16ActualCoilIndex = ui16CoilIndex - MODBUS_RTU_COIL_STATUS_START_REG_INDEX;
        if(ui16CoilValue == 0xFF00){
            tdfrtus.abCoilStatusbits[ui16ActualCoilIndex] = 1; 
            bActionTaken = true;
        }else if(ui16CoilValue == 0x0000){
            tdfrtus.abCoilStatusbits[ui16ActualCoilIndex] = 0;
            bActionTaken = true;
        }else{
            process_errorCode(0x85, MODBUS_ILLEGAL_VALUE_CODE);    
        }

        if(bActionTaken == true){
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = tdfrtus.ui8DeviceId;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = MODBUS_FUNC_WRITE_SINGLE_COIL;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CoilIndex >> 8;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CoilIndex;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CoilValue >> 8;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CoilValue;
            
            ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,ui8ArrayCounter);
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CRC >> 8;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CRC;

            tdfrtus.ui16ProcessResult = true;
            tdfrtus.ui16ProcessResult <<= 8;
            tdfrtus.ui16ProcessResult |= ui8ArrayCounter;
        }
    }else{
        process_errorCode(0x85, MODBUS_ILLEGAL_ADDRESS_CODE);
    }
}

void ModbusRTUSlave::process_write_register(void){
    uint16_t ui16RegIndex=0;
    uint16_t ui16RegValue=0;

    ui16RegIndex = ModUtil.tdfParameter.aui8ModbusRxBuffer[2];
    ui16RegIndex = ui16RegIndex << 8;
    ui16RegIndex = ui16RegIndex | ModUtil.tdfParameter.aui8ModbusRxBuffer[3];

    ui16RegValue = ModUtil.tdfParameter.aui8ModbusRxBuffer[4];
    ui16RegValue = ui16RegValue  << 8;
    ui16RegValue = ui16RegValue | ModUtil.tdfParameter.aui8ModbusRxBuffer[5];

    if((ui16RegIndex >= MODBUS_RTU_HOLDING_START_REG_INDEX) && (ui16RegIndex <= (MODBUS_RTU_HOLDING_START_REG_INDEX + MODBUS_RTU_HOLDING_REG_SIZE))){
        uint8_t ui8RegIndex=0;
        uint8_t ui8ArrayCounter=0;
        uint16_t ui16CRC=0;

        ui8RegIndex = ui16RegIndex - MODBUS_RTU_INPUT_START_REG_INDEX;
        tdfrtus.aui16HoldingReg[ui8RegIndex] = ui16RegValue;
        //store this in memory

        ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = tdfrtus.ui8DeviceId;
        ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = MODBUS_FUNC_WRITE_SINGLE_REG;
        ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16RegIndex >> 8;
        ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16RegIndex;
        ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16RegValue >> 8;
        ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16RegValue;
            
        ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,ui8ArrayCounter);
        ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CRC >> 8;
        ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CRC;

        tdfrtus.ui16ProcessResult = true;
        tdfrtus.ui16ProcessResult <<= 8;
        tdfrtus.ui16ProcessResult |= ui8ArrayCounter;

        elog.Write(LogLevel::Trace,"RTUSlave", elog.get_array_to_string("holding",tdfrtus.aui16HoldingReg,MODBUS_RTU_HOLDING_REG_SIZE,PRINT_HEX).c_str());
    }else{
        //illigal data address
        process_errorCode(0x86, MODBUS_ILLEGAL_ADDRESS_CODE);
    }
}

void ModbusRTUSlave::process_multiple_coils(void){
    uint16_t ui16StartReg=0;
    uint16_t ui16NumberOfReg=0;
    
    ui16StartReg = ModUtil.tdfParameter.aui8ModbusRxBuffer[2];
    ui16StartReg = ui16StartReg << 8;
    ui16StartReg = ui16StartReg | ModUtil.tdfParameter.aui8ModbusRxBuffer[3];

    ui16NumberOfReg = ModUtil.tdfParameter.aui8ModbusRxBuffer[4];
    ui16NumberOfReg = ui16NumberOfReg  << 8;
    ui16NumberOfReg = ui16NumberOfReg | ModUtil.tdfParameter.aui8ModbusRxBuffer[5];

    if((ui16StartReg >= MODBUS_RTU_COIL_STATUS_START_REG_INDEX) && (ui16StartReg <= (MODBUS_RTU_COIL_STATUS_START_REG_INDEX + MODBUS_RTU_COIL_STATUS_SIZE))){
        if((ui16NumberOfReg <= MODBUS_RTU_COIL_STATUS_SIZE) && ((ui16StartReg + ui16NumberOfReg) <= (MODBUS_RTU_COIL_STATUS_START_REG_INDEX + MODBUS_RTU_COIL_STATUS_SIZE))){
            uint8_t ui8LoopCounter=0;
            uint8_t ui8StartIndex=0;
            uint8_t ui8NumberOfReg=0;
            uint8_t ui8ArrayCounter=0;
            uint8_t ui8RegValueIndex=0;
            uint16_t ui16CRC=0;
            uint64_t ui64Value=0;
            uint8_t  ui8ByteValue=0;
            uint16_t ui16NumberOfBits=0;
            uint8_t  ui8InnerLoopCounter=0;
            uint16_t ui16StartBit=0;
            uint16_t ui16EndBit=0;

            ui8StartIndex = ui16StartReg - MODBUS_RTU_COIL_STATUS_START_REG_INDEX;
            ui8NumberOfReg = ui8StartIndex + ModUtil.tdfParameter.aui8ModbusRxBuffer[6];
            ui8RegValueIndex = 7;

            ui16StartBit =  ui16StartReg - MODBUS_RTU_HOLDING_START_REG_INDEX;
            ui16EndBit = ui16StartReg + ui16NumberOfReg;

            for(ui8LoopCounter=ui8NumberOfReg;ui8LoopCounter>0;ui8LoopCounter--){
                ui8ByteValue = ModUtil.tdfParameter.aui8ModbusRxBuffer[ui8RegValueIndex++];
                for(ui8InnerLoopCounter=0;ui8InnerLoopCounter<8;ui8InnerLoopCounter++){
                    if(ui16StartBit < ui16EndBit){
                        if((1<<ui8InnerLoopCounter) & ui8ByteValue){
                            tdfrtus.abCoilStatusbits[ui16StartBit++] = true;
                        }else{
                            tdfrtus.abCoilStatusbits[ui16StartBit++] = false;
                        }
                    }
                }
            }

            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = tdfrtus.ui8DeviceId;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = MODBUS_FUNC_WRITE_MULTI_COIL;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16StartReg >> 8;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16StartReg;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16NumberOfReg >> 8;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16NumberOfReg;

            ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,ui8ArrayCounter);
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CRC >> 8;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CRC;

            tdfrtus.ui16ProcessResult = true;
            tdfrtus.ui16ProcessResult <<= 8;
            tdfrtus.ui16ProcessResult |= ui8ArrayCounter;
        }else{
            process_errorCode(0x10|0x80, MODBUS_ILLEGAL_ADDRESS_CODE);
        }
    }else{
        //illigal data address
        process_errorCode(0x10|0x80, MODBUS_ILLEGAL_ADDRESS_CODE);
    }
}

void ModbusRTUSlave::process_multiple_register(void){
    uint16_t ui16StartReg=0;
    uint16_t ui16NumberOfReg=0;

    ui16StartReg = ModUtil.tdfParameter.aui8ModbusRxBuffer[2];
    ui16StartReg = ui16StartReg << 8;
    ui16StartReg = ui16StartReg | ModUtil.tdfParameter.aui8ModbusRxBuffer[3];

    ui16NumberOfReg = ModUtil.tdfParameter.aui8ModbusRxBuffer[4];
    ui16NumberOfReg = ui16NumberOfReg  << 8;
    ui16NumberOfReg = ui16NumberOfReg | ModUtil.tdfParameter.aui8ModbusRxBuffer[5];

    if((ui16StartReg >= MODBUS_RTU_HOLDING_START_REG_INDEX) && (ui16StartReg <= (MODBUS_RTU_HOLDING_START_REG_INDEX + MODBUS_RTU_HOLDING_REG_SIZE))){
        if((ui16NumberOfReg <= MODBUS_RTU_HOLDING_REG_SIZE) && ((ui16StartReg + ui16NumberOfReg) <= (MODBUS_RTU_HOLDING_START_REG_INDEX + MODBUS_RTU_HOLDING_REG_SIZE))){
            uint8_t ui8LoopCounter=0;
            uint8_t ui8StartIndex=0;
            uint8_t ui8NumberOfReg=0;
            uint8_t ui8ArrayCounter=0;
            uint8_t ui8RegValueIndex=0;
            uint16_t ui16RegValue=0;
            uint16_t ui16CRC=0;

            ui8StartIndex = ui16StartReg - MODBUS_RTU_HOLDING_START_REG_INDEX;
            ui8NumberOfReg = ui8StartIndex +  ModUtil.tdfParameter.aui8ModbusRxBuffer[6];
            ui8RegValueIndex = 7;

            for(ui8LoopCounter=ui8StartIndex;ui8LoopCounter<ui8NumberOfReg;ui8LoopCounter++){
                ui16RegValue = ModUtil.tdfParameter.aui8ModbusRxBuffer[ui8RegValueIndex++];
                ui16RegValue <<= 8;
                ui16RegValue |= ModUtil.tdfParameter.aui8ModbusRxBuffer[ui8RegValueIndex++];
                tdfrtus.aui16HoldingReg[ui8LoopCounter] = ui16RegValue;
            }

            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = tdfrtus.ui8DeviceId;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = MODBUS_FUNC_WRITE_MULTI_REG;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16StartReg >> 8;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16StartReg;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16NumberOfReg >> 8;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16NumberOfReg;

            ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,ui8ArrayCounter);
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CRC >> 8;
            ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16CRC;

            tdfrtus.ui16ProcessResult = true;
            tdfrtus.ui16ProcessResult <<= 8;
            tdfrtus.ui16ProcessResult |= ui8ArrayCounter;

            elog.Write(LogLevel::Trace,"RTUSlave", elog.get_array_to_string("holding",tdfrtus.aui16HoldingReg,MODBUS_RTU_HOLDING_REG_SIZE,PRINT_HEX).c_str());

        }else{
            process_errorCode(0x10|0x80, MODBUS_ILLEGAL_ADDRESS_CODE);
        }
    }else{
        //illigal data address
        process_errorCode(0x10|0x80, MODBUS_ILLEGAL_ADDRESS_CODE);
    }

}

void ModbusRTUSlave::exception_handling(uint8_t ui8ErrorCode, uint8_t ui8ErrorValue){
    uint16_t ui16Result=0;
    
    memset(ModUtil.tdfParameter.aui8ModbusTxBuffer, 0, MODBUS_TX_BUFFER_SIZE);
    ModUtil.tdfParameter.aui8ModbusTxBuffer[0] = tdfrtus.ui8DeviceId;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[1] = ui8ErrorCode;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[2] = ui8ErrorValue;
    
    ui16Result = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusTxBuffer,3);
    ModUtil.tdfParameter.aui8ModbusTxBuffer[3] = ui16Result >> 8;
    ModUtil.tdfParameter.aui8ModbusTxBuffer[4] = ui16Result;
}

void ModbusRTUSlave::process_errorCode(uint8_t ui8FunctionError, uint8_t ui8ErrorCode){
    tdfrtus.ui16ProcessResult = true;
    tdfrtus.ui16ProcessResult <<= 8;
    tdfrtus.ui16ProcessResult |= 5;//number of bytes to send...

    exception_handling(ui8FunctionError,ui8ErrorCode);
}

void ModbusRTUSlave::write(void){
    uint8_t ui8Result=0;
    ui8Result = tdfrtus.ui16ProcessResult >> 8;
    if(ui8Result){
        uint8_t ui8ByteSize=0;
        uint8_t ui8LoopCounter=0;
        ui8ByteSize = tdfrtus.ui16ProcessResult;
        if(ui8ByteSize > 0){
            ModUtil.pin_control(1);
            elog.Write(LogLevel::Trace,"RTUSlave", elog.get_array_to_string("tx",ModUtil.tdfParameter.aui8ModbusTxBuffer,ui8ByteSize,PRINT_HEX).c_str());
            for(ui8LoopCounter=0;ui8LoopCounter<ui8ByteSize;ui8LoopCounter++){
                ModbusUART.write(ModUtil.tdfParameter.aui8ModbusTxBuffer[ui8LoopCounter]);
                while(Serial.availableForWrite() < 127);
            }
            delay(1);
            ModUtil.pin_control(0);
            tdfrtus.ui16ProcessResult=0;
            tdfrtus.ui16ReadResult=0;
            tdfrtus.ui16WriteResult=0;
            tdfrtus.ui16ModbusRxCounter = 0;
        }
    }
}


bool ModbusRTUSlave::is_data_received(void){
    uint8_t ui8Result=0;

    while(ModbusUART.available()){
        ModUtil.tdfParameter.aui8ModbusRxBuffer[tdfrtus.ui16ModbusRxCounter] = ModbusUART.read();
        tdfrtus.ui16ModbusRxCounter++;
    }

    if(tdfrtus.ui16ModbusRxCounter > 0){
        if(ModUtil.tdfParameter.aui8ModbusRxBuffer[0] != tdfrtus.ui8DeviceId){
            tdfrtus.ui16ModbusRxCounter = 0;
        }else{
            uint8_t ui8FunCodeResponse=0;
            ui8FunCodeResponse = ModUtil.verify_function_code(ModUtil.tdfParameter.aui8ModbusRxBuffer[1]);
            if(ui8FunCodeResponse == 0){
                tdfrtus.ui16ModbusRxCounter = 0;
            }
            if(ui8FunCodeResponse == 2){
                tdfrtus.ui16ModbusRxCounter = 0;
            }

            if(tdfrtus.ui16ModbusRxCounter >= MODBUS_RTU_DEFAULT_BYTE_SIZE){
                
                ui8FunCodeResponse = ModUtil.verify_function_code(ModUtil.tdfParameter.aui8ModbusRxBuffer[1]);
                if(ui8FunCodeResponse == 0){
                    tdfrtus.ui16ModbusRxCounter = 0;
                }else if(ui8FunCodeResponse == 2){
                    tdfrtus.ui16ModbusRxCounter = 0;
                }else{
                    if(ModUtil.tdfParameter.aui8ModbusRxBuffer[1] == MODBUS_FUNC_WRITE_MULTI_COIL){
                        tdfrtus.ui8AdditionalBytes = MODBUS_RTU_DEFAULT_BYTE_SIZE + 1 + ModUtil.tdfParameter.aui8ModbusRxBuffer[6];
                    }else if(ModUtil.tdfParameter.aui8ModbusRxBuffer[1] == MODBUS_FUNC_WRITE_MULTI_REG){
                        tdfrtus.ui8AdditionalBytes = MODBUS_RTU_DEFAULT_BYTE_SIZE + 1 + ModUtil.tdfParameter.aui8ModbusRxBuffer[6];
                    }else{
                        tdfrtus.ui8AdditionalBytes = MODBUS_RTU_DEFAULT_BYTE_SIZE;
                    }

                    if(tdfrtus.ui16ModbusRxCounter >= tdfrtus.ui8AdditionalBytes){
                        uint16_t ui16CRC;
                        uint8_t  ui8CRCLow;
                        uint8_t  ui8CRCHigh;
                        uint8_t  ui8CRCCompLow;
                        uint8_t  ui8CRCCompHigh;

                        ui16CRC = ModUtil.get_crc16(ModUtil.tdfParameter.aui8ModbusRxBuffer,tdfrtus.ui16ModbusRxCounter-2);
                        ui8CRCLow = ui16CRC >> 8;
                        ui8CRCHigh = ui16CRC;
                        ui8CRCCompLow = ModUtil.tdfParameter.aui8ModbusRxBuffer[tdfrtus.ui16ModbusRxCounter-2];
                        ui8CRCCompHigh = ModUtil.tdfParameter.aui8ModbusRxBuffer[tdfrtus.ui16ModbusRxCounter-1];

                        if((ui8CRCLow == ui8CRCCompLow) && (ui8CRCHigh == ui8CRCCompHigh)){
                            ui8Result = true;
                            String sPrintData ="bytes:" + String(tdfrtus.ui16ModbusRxCounter) + " Rx";
                            elog.Write(LogLevel::Trace,"RTUSlave", elog.get_array_to_string(sPrintData,ModUtil.tdfParameter.aui8ModbusRxBuffer,tdfrtus.ui16ModbusRxCounter,PRINT_HEX).c_str());
                        }else{
                            String sPrintData ="MODBUS_INVALID_RX bytes:" + String(tdfrtus.ui16ModbusRxCounter) + " Rx";
                            elog.Write(LogLevel::Error,"RTUSlave",sPrintData.c_str());
                            tdfrtus.ui16ModbusRxCounter = 0;
                        }
                    }
                }
            }
        }
    }
    
    return ui8Result;
    
}

void ModbusRTUSlave::test(void){
    
}

// Preinstantiate Objects //////////////////////////////////////////////////////
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MODBUSRTUSLAVE)
ModbusRTUSlave rtus;
#endif