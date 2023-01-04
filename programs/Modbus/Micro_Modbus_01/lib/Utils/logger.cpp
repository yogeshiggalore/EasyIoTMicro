#include <Arduino.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iomanip>
#include <cstdlib>
#include <stdarg.h>
#include <stdlib.h>
#include <stdexcept>
#include <logger.h>
#include <utils.h>
#include <cdate.h>
#include <ESPUtils.h>
#include <sys/time.h>

using namespace std;


Logger::Logger(){}

void MakePath( string sPath);

void Logger::Write( LogLevel p_oLevel, string p_sMethod, string p_sMesg)
{
	string	l_sTime;
	if ( m_oLogLevel > p_oLevel) return;
	l_sTime = CDate::Now().ToString();

	if (this->m_bConsoleEnabled) {
		if (LogLevel::Warn <= p_oLevel) {
			this->PrintToConsole(stderr, l_sTime.c_str(), p_oLevel.ToString().c_str(), p_sMethod.c_str(), p_sMesg.c_str());
		}
		else {
			this->PrintToConsole(stdout, l_sTime.c_str(), p_oLevel.ToString().c_str(), p_sMethod.c_str(), p_sMesg.c_str());
		}
	}

	if (this->m_bFileEnabled) {
		this->PrintToFile(l_sTime.c_str(), p_oLevel.ToString().c_str(), p_sMethod.c_str(), p_sMesg.c_str());
	}
}

void Logger::Write( LogLevel p_oLevel, string p_sMethod, const char* p_pPattern, ...) {
	va_list	l_pArgList;
	string	l_sTime;

	if ( this->m_oLogLevel > p_oLevel) return;

	l_sTime = CDate::Now().ToString();
	if (this->m_bConsoleEnabled) {
		va_start(l_pArgList, p_pPattern);
		if (LogLevel::Warn <= p_oLevel)
		{
			this->PrintToConsole(stderr, l_sTime.c_str(), p_oLevel.ToString().c_str(), p_sMethod.c_str(), p_pPattern, l_pArgList);
		}
		else
		{
			this->PrintToConsole(stdout, l_sTime.c_str(), p_oLevel.ToString().c_str(), p_sMethod.c_str(), p_pPattern, l_pArgList);
		}
		va_end(l_pArgList);
	}

	if (this->m_bFileEnabled) {
		va_start(l_pArgList, p_pPattern);
		this->PrintToFile(l_sTime.c_str(), p_oLevel.ToString().c_str(), p_sMethod.c_str(), p_pPattern, l_pArgList);
		va_end(l_pArgList);
	}
}

void Logger::PrintToFile(const char* p_pTime, const char* p_pLevel, const char* p_pMethod, const char* p_pMessage)
{
	this->OpenFile();
	if (this->m_fPtr == NULL) return;
	fprintf(this->m_fPtr, "%s|%s|%s|%s\n"
						, p_pTime
						, p_pLevel
						, p_pMethod
						, p_pMessage);
	fflush(this->m_fPtr);
}

void Logger::PrintToFile(const char* p_pTime, const char* p_pLevel, const char* p_pMethod, const char* p_pPattern, va_list p_pArgList)
{
	this->OpenFile();
	if (this->m_fPtr == NULL) return;

	fprintf(this->m_fPtr, "%s|%s|%s|"
		, p_pTime
		, p_pLevel
		, p_pMethod
	);

	vfprintf(this->m_fPtr, p_pPattern, p_pArgList);
	fprintf(this->m_fPtr, "\n");
	fflush(this->m_fPtr);
}

void Logger::PrintToConsole(FILE* p_pOutput, const char* p_pTime, const char* p_pLevel, const char* p_pMethod, const char* p_pMessage)
{
	Serial.printf( "%s|%s|%s|%s\n"
					, p_pTime
					, p_pLevel
					, p_pMethod
					, p_pMessage);
}

void Logger::PrintToConsole(FILE* p_pOutput, const char* p_pTime, const char* p_pLevel, const char* p_pMethod, const char* p_pPattern, va_list p_pArgList)
{
	fprintf(p_pOutput, "%s|%s|%s|"
		, p_pTime
		, p_pLevel
		, p_pMethod
	);

	vfprintf(p_pOutput, p_pPattern, p_pArgList);
	fprintf(p_pOutput, "\n");
}

void Logger::WritePlain( string sFileName, const char* pLogLine, ...) {
	if (!this->m_bFileEnabled) return;
	va_list	argList;

	FILE* l_pOutput = NULL;
	l_pOutput = stdout;

	va_start( argList, pLogLine);
	vfprintf( l_pOutput, pLogLine, argList);
	fprintf( l_pOutput, "\n");
	va_end( argList);

	this->OpenFile( sFileName);
	if ( this->m_fPtr == NULL ) return;
	va_start( argList, pLogLine);
	// fprintf( this->m_fPtr, "%s|%s|",sTime.c_str(), TRACE_LEVEL_DESC[iLevel].c_str());
	vfprintf( this->m_fPtr, pLogLine, argList);
	va_end( argList);
	fprintf( this->m_fPtr, "\n");
	fflush( this->m_fPtr);
}

void Logger::OpenFile( string FileName)
{
	string	l_sBuff;

	try {
		l_sBuff = this->m_sLogPath;
		l_sBuff.append( "/");

		MakePath(l_sBuff);

		l_sBuff += "/" + this->m_sBaseFileName;
		l_sBuff += "-";
		l_sBuff.append( FileName);
		l_sBuff += ".log";

		if ( strcmp( this->m_sCurrFileName.c_str(), l_sBuff.c_str()) != 0 ) {
			if ( this->m_fPtr != NULL ) fclose( this->m_fPtr);
			this->m_fPtr = NULL;
			this->m_sCurrFileName = "";
			this->m_sCurrFileName = l_sBuff;
		}
		if ( this->m_fPtr == NULL ) this->m_fPtr = fopen( this->m_sCurrFileName.c_str(), "a");
	} catch( ...) {
	}
}

void Logger::OpenFile( void)
{
	string	l_sBuff;
	string	l_sSuffix;
	CDate	l_oTime;

	try {
		l_oTime = CDate::Now();

		l_sBuff = this->m_sLogPath;
		l_sBuff.append( "/");
		l_sBuff.append( l_oTime.ToString("%Y-%m-%d"));

		MakePath(l_sBuff);

		if (LogSplitType::Monthly != this->m_oLogSplit ) {
			l_sBuff.append("/");
			l_sBuff.append(l_oTime.ToString("%d"));
			MakePath(l_sBuff);
			if (LogSplitType::Hourly == this->m_oLogSplit) {
				l_sBuff.append( "/");
				l_sBuff.append(l_oTime.ToString("%H"));
				MakePath(l_sBuff);
				l_sSuffix = "%Y-%m-%d_%H";
			}
			else {
				l_sSuffix = "%Y-%m-%d";
			}
		}
		else {
			l_sSuffix = "%Y-%m";
		}

		l_sBuff += "/" + this->m_sBaseFileName;
		l_sBuff += "-";
		// l_sBuff.append( l_pBuff);
		l_sBuff.append(l_oTime.ToString(l_sSuffix));
		l_sBuff += ".log";

		if ( strcmp( this->m_sCurrFileName.c_str(), l_sBuff.c_str()) != 0 ) {
			if (this->m_fPtr != NULL) {
				fclose(this->m_fPtr);
			}
			this->m_fPtr = NULL;
			this->m_sCurrFileName = "";
			this->m_sCurrFileName = l_sBuff;
		}
		if (this->m_fPtr == NULL) {
			this->m_fPtr = fopen(this->m_sCurrFileName.c_str(), "a");
		}
	} catch( ...) {
	}
}

void MakePath( string sPath)
{
	struct stat st = {0};

	if ( stat( sPath.c_str(), &st) == -1)
	{
    	mkdir( sPath.c_str(), 0755);
	}
}

/*Logger::Logger()
{
	this->m_oLogLevel = LogLevel::Trace;
	this->m_bConsoleEnabled = true;
	this->m_fPtr = NULL;
	this->DisableFile();
}

Logger::Logger(LogLevel p_oLevel)
{
	this->m_oLogLevel = p_oLevel;
	this->m_bConsoleEnabled = true;
	this->m_fPtr = NULL;
	this->DisableFile();
}

Logger::Logger(LogLevel p_oLevel, LogSplitType p_oSplitOn, string p_sPath, string p_sFileName)
{
	this->m_oLogLevel = p_oLevel;
	this->m_bConsoleEnabled = true;
	this->m_fPtr = NULL;
	this->EnableFile(p_sPath, p_sFileName, p_oSplitOn);
}

Logger::~Logger()
{
	this->CloseFile();
}*/

void Logger::start(LogLevel p_oLevel){
	m_oLogLevel = p_oLevel;
	m_bConsoleEnabled = true;
	m_fPtr = NULL;
	DisableFile();
}

void Logger::CloseFile(void)
{
	if (this->m_fPtr != NULL) {
		fclose(this->m_fPtr);
	}
	this->m_fPtr = NULL;
}

bool Logger::IsFileEnabled()
{
	return this->m_bFileEnabled;
}

bool Logger::IsConsoleEnabled()
{
	return this->m_bConsoleEnabled;
}

void Logger::EnableConsole()
{
	this->m_bConsoleEnabled = true;
}

void Logger::DisableConsole()
{
	this->m_bConsoleEnabled = false;
}

void Logger::EnableFile( string p_sPath, string p_sFileName, LogSplitType p_oSplitOn)
{
	this->CloseFile();
	this->m_sBaseFileName = p_sFileName;
	this->m_sCurrFileName = "";
	this->m_sLogPath = p_sPath;
	this->m_oLogSplit = p_oSplitOn;

	this->m_bFileEnabled = true;
}

void Logger::DisableFile()
{
	this->m_bFileEnabled = false;
	this->CloseFile();
	this->m_sBaseFileName = "";
	this->m_sCurrFileName = "";
	this->m_sLogPath = "";
	this->m_oLogSplit = LogSplitType::Daily;
	this->m_fPtr = NULL;
}


String Logger::get_array_to_string(String sTitle,uint8_t* aui8Data, uint8_t ui8ByteSize, uint8_t ui8Type){
	char acArray[32];
	String sString;
	uint8_t ui8LoopCounter=0;
	
	sString = sTitle + ": ";
	
	for(ui8LoopCounter=0;ui8LoopCounter<ui8ByteSize;ui8LoopCounter++){
		if(ui8Type == PRINT_HEX){
			sprintf(acArray,"%X ", aui8Data[ui8LoopCounter]);
		}else{
			sprintf(acArray,"%d ", aui8Data[ui8LoopCounter]);
		}

		sString = sString + String(acArray);
	}

	return sString;
}

String Logger::get_array_to_string(String sTitle,char* acData, uint8_t ui8ByteSize, uint8_t ui8Type){
	char acArray[32];
	String sString;
	uint8_t ui8LoopCounter=0;
	
	sString = sTitle + ": ";
	
	for(ui8LoopCounter=0;ui8LoopCounter<ui8ByteSize;ui8LoopCounter++){
		if(ui8Type == PRINT_HEX){
			sprintf(acArray,"%X ", acData[ui8LoopCounter]);
		}else{
			sprintf(acArray,"%d ", acData[ui8LoopCounter]);
		}
		//sprintf(acArray,"%c ", acData[ui8LoopCounter]);

		sString = sString + String(acArray);
	}

	return sString;
}

String Logger::get_array_to_string(String sTitle,uint16_t* aui16Data, uint8_t ui8ByteSize, uint8_t ui8Type ){
	char acArray[32];
	String sString;
	uint8_t ui8LoopCounter=0;
	
	sString = sTitle + ": ";

	for(ui8LoopCounter=0;ui8LoopCounter<ui8ByteSize;ui8LoopCounter++){
		if(ui8Type == PRINT_HEX){
			sprintf(acArray,"%X ", aui16Data[ui8LoopCounter]);
		}else{
			sprintf(acArray,"%d ", aui16Data[ui8LoopCounter]);
		}

		sString = sString + String(acArray);
	}

	return sString;
}

String Logger::get_array_to_string(String sTitle,uint32_t* aui32Data, uint8_t ui8ByteSize, uint8_t ui8Type ){
	char acArray[32];
	String sString;
	uint8_t ui8LoopCounter=0;
	
	sString = sTitle + ": ";

	for(ui8LoopCounter=0;ui8LoopCounter<ui8ByteSize;ui8LoopCounter++){
		if(ui8Type == PRINT_HEX){
			sprintf(acArray,"%X ", aui32Data[ui8LoopCounter]);
		}else{
			sprintf(acArray,"%d ", aui32Data[ui8LoopCounter]);
		}

		sString = sString + String(acArray);
	}

	return sString;
}

String Logger::get_array_to_string(String sTitle,float* afData, uint8_t ui8ByteSize, uint8_t ui8Type){
	char acArray[32];
	String sString;
	uint8_t ui8LoopCounter=0;
	
	sString = sTitle + ": ";

	for(ui8LoopCounter=0;ui8LoopCounter<ui8ByteSize;ui8LoopCounter++){
		sprintf(acArray,"%f ", afData[ui8LoopCounter]);
		sString = sString + String(acArray);
	}

	return sString;
}

void Logger::set_time(void){
	//CDate obj;
	//obj.SetTime(EUtils.get_epoch_time());
	timeval epoch = {EUtils.get_epoch_time(), 0};
  	const timeval *tv = &epoch;
  	timezone utc = {0,0};
  	const timezone *tz = &utc;
  	settimeofday(tv, tz);
}
// Preinstantiate Objects //////////////////////////////////////////////////////
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_LOGGER)
Logger elog;
#endif

