#include<string>
#include<loglevel.h>

using namespace std;

LogLevel LogLevel::Parse(int code)
{
    if(code == Debug)
    {
        return Debug;
    }
    else if(code == Info)
    {
        return Info;
    }
    else if(code == Warn)
    {
        return Warn;
    }
    else if(code == Error)
    {
        return Error;
    }
    else if(code == Fatal)
    {
        return Fatal;
    }
    else if(code == Off)
    {
        return Off;
    }
    
    return Trace;
}

string LogLevel::ToString()
{
    return this->GetLabel();
}

uint8 LogLevel::GetCode()
{
    return m_iCode;
}

string LogLevel::GetLabel()
{
    if (m_iCode == Trace)
    {
        return "Trace";
    }
    else if (m_iCode == Debug)
    {
        return "Debug";
    }
    else if (m_iCode == Info)
    {
        return "Info";
    }
    else if (m_iCode == Warn)
    {
        return "Warn";
    }
    else if (m_iCode == Error)
    {
        return "Error";
    }
    else if (m_iCode == Fatal)
    {
        return "Fatal";
    }
    else if (m_iCode == Off)
    {
        return "Off";
    }
}
