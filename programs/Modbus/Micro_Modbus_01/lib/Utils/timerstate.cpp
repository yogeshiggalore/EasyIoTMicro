#include<string>
#include<timerstate.h>

using namespace std;

TimerState TimerState::Parse(int code)
{
    if(code == Start)
    {
        return Start;
    }
    else if(code == Running)
    {
        return Running;
    }
    else if (code == TimedOut)
    {
        return TimedOut;
    }
    
    return Idle;
}

string TimerState::GetLabel()
{
    if (m_iCode == Start)
    {
        return "Start";
    }
    else if (m_iCode == Running)
    {
        return "Running";
    }
    else if (m_iCode == TimedOut)
    {
        return "TimedOut";
    }

    return "Idle";
}

string TimerState::ToString()
{
    return this->GetLabel();
}

uint8 TimerState::GetCode()
{
    return m_iCode;
}

