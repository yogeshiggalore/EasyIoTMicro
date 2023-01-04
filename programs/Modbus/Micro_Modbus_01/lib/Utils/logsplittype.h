#ifndef _LOG_SPLIT_TYPE_H_INCLUDED_
#define _LOG_SPLIT_TYPE_H_INCLUDED_ 1

#include<string>
#include<defs.h>

using namespace std;

class LogSplitType
{
public:
    enum Value : uint8
    {
        Monthly = 0,
        Daily = 1,
        Hourly = 2
    };

    LogSplitType() = default;
    LogSplitType(Value code) : m_iCode(code) { }

    operator Value() const { return m_iCode; }
    explicit operator bool() = delete;
    constexpr bool operator==(LogSplitType a) const { return m_iCode == a.m_iCode; }
    constexpr bool operator!=(LogSplitType a) const { return m_iCode != a.m_iCode; }
    uint8 GetCode();
    string GetLabel();
    string ToString();

    static LogSplitType Parse(int code);

private:
    Value m_iCode;
};

#endif
