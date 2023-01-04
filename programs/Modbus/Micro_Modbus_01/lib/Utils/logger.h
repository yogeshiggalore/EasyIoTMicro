#ifndef _LOGGER_H
#define _LOGGER_H

#include<iostream>
#include<stdio.h>
#include<string>
#include<errno.h>
#include<unistd.h>
#include<stdlib.h>
#include<iomanip>
#include<cstdlib>

#include <defs.h>
#include <loglevel.h>
#include <logsplittype.h>

#define PRINT_HEX	0
#define PRINT_DEC	1

typedef __gnuc_va_list va_list;

using namespace std;

class Logger {
protected:
public:
	Logger();
	void OpenFile( void);
	void OpenFile( string);
	void CloseFile(void);

	void Write( LogLevel, string, string);
	void Write( LogLevel, string, const char*, ...);
	void WritePlain( string, const char*, ...);
	void set_time(void);

	void PrintToConsole(FILE* p_pOutput, const char* p_pTime, const char* p_pLevel, const char* p_pMethod, const char* p_pPattern, va_list p_pArgList);
	void PrintToConsole(FILE* p_pOutput, const char* p_pTime, const char* p_pLevel, const char* p_pMethod, const char* p_pMessage);
	void PrintToFile(const char* p_pTime, const char* p_pLevel, const char* p_pMethod, const char* p_pPattern, va_list p_pArgList);
	void PrintToFile(const char* p_pTime, const char* p_pLevel, const char* p_pMethod, const char* p_pMessage);

	//Logger();
	//Logger(LogLevel p_oLevel);
	//Logger(LogLevel p_oLevel, LogSplitType p_oSplitOn, string p_sPath, string p_sFileName);
	//~Logger();
	void start(LogLevel p_oLevel);
	bool IsFileEnabled();
	bool IsConsoleEnabled();
	void EnableConsole();
	void DisableConsole();
	void EnableFile(string p_sPath, string p_sFileName, LogSplitType p_oSplitOn);
	void DisableFile();

	String get_array_to_string(String sTitle,uint8_t* aui8Data, uint8_t ui8ByteSize, uint8_t ui8Type);
	String get_array_to_string(String sTitle,char* acData, uint8_t ui8ByteSize, uint8_t ui8Type);
	String get_array_to_string(String sTitle,uint16_t* aui16Data, uint8_t ui8ByteSize, uint8_t ui8Type);
	String get_array_to_string(String sTitle,uint32_t* aui32Data, uint8_t ui8ByteSize, uint8_t ui8Type);
	String get_array_to_string(String sTitle,float* afData, uint8_t ui8ByteSize, uint8_t ui8Type);

private:
	bool			m_bFileEnabled;
	bool			m_bConsoleEnabled;
	string			m_sLogPath;
	string			m_sBaseFileName;
	string			m_sCurrFileName;
	LogLevel		m_oLogLevel;
	LogSplitType	m_oLogSplit;
	FILE*			m_fPtr;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_LOGGER)
extern Logger elog;
#endif

#endif

