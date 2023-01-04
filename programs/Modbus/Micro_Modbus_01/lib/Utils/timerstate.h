#ifndef _TIMER_STATE_H_INCLUDED_
#define _TIMER_STATE_H_INCLUDED_ 1

#include<string>
#include<defs.h>

using namespace std;

class TimerState
{
public:
    enum Value : uint8
    {
        Idle = 0,
        Start = 1,
        Running = 2,
        TimedOut = 3
    };

    TimerState() = default;
    TimerState(Value code) : m_iCode(code) { }

    operator Value() const { return m_iCode; }
    explicit operator bool() = delete;
    constexpr bool operator==(TimerState a) const { return m_iCode == a.m_iCode; }
    constexpr bool operator!=(TimerState a) const { return m_iCode != a.m_iCode; }
    uint8 GetCode();
    string GetLabel();
    string ToString();

    static TimerState Parse(int code);

private:
    Value m_iCode;
};

#endif
