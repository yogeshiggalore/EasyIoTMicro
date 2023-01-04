#ifndef _INTERVAL_TYPE_H_INCLUDED_
#define _INTERVAL_TYPE_H_INCLUDED_ 1

#include<string>
#include<defs.h>

using namespace std;

class IntervalType
{
public:
    enum Value : uint8
    {
        Day = 0,
        Month = 1,
        Year = 2,

        Hour = 3,
        Minute = 4,
        Second = 5,
        MicroSecond = 6,
        MilliSecond = 7
    };

    IntervalType() = default;
    IntervalType(Value code) : m_iCode(code) { }

    operator Value() const { return m_iCode; }
    explicit operator bool() = delete;
    constexpr bool operator==(IntervalType a) const { return m_iCode == a.m_iCode; }
    constexpr bool operator!=(IntervalType a) const { return m_iCode != a.m_iCode; }
    uint8 GetCode();
    string GetLabel();
    string ToString();

    static IntervalType Parse(int code);

private:
    Value m_iCode;
};

#endif
