#ifndef _LOG_LEVEL_H_INCLUDED_
#define _LOG_LEVEL_H_INCLUDED_ 1

#include<string>
#include<defs.h>

using namespace std;

class LogLevel
{
public:
    enum Value : uint8
    {
        Trace = 0,	// Detailed Debugging
        Debug = 1,	// For debugging
        Info = 2,	// Runtime events like startup/ shutdown of system and db conenctivity
        Warn = 3,	// Use of unconfigured values 
        Error = 4,	// Unexpected runtime errors
        Fatal = 5,	// Exceptions causing premature termination of process
        Off = 6
    };

    
    LogLevel() = default;
    LogLevel(Value code) : m_iCode(code) { }

    operator Value() const { return m_iCode; }
    explicit operator bool() = delete;
    constexpr bool operator==(LogLevel a) const { return m_iCode == a.m_iCode; }
    constexpr bool operator!=(LogLevel a) const { return m_iCode != a.m_iCode; }
    constexpr bool operator>(LogLevel a) const { return m_iCode > a.m_iCode; }
    constexpr bool operator>=(LogLevel a) const { return m_iCode >= a.m_iCode; }
    constexpr bool operator<(LogLevel a) const { return m_iCode < a.m_iCode; }
    constexpr bool operator<=(LogLevel a) const { return m_iCode <= a.m_iCode; }

    uint8 GetCode();
    string GetLabel();
    string ToString();

    static LogLevel Parse(int code);

private:
    Value m_iCode;
};

#endif
