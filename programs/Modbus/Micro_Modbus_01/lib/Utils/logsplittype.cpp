#include<string>
#include<logsplittype.h>

using namespace std;

LogSplitType LogSplitType::Parse(int code)
{
    if(code == Monthly)
    {
        return Monthly;
    }
    else if(code == Hourly)
    {
        return Hourly;
    }
    
    return Daily;
}

string LogSplitType::ToString()
{
    return this->GetLabel();
}

uint8 LogSplitType::GetCode()
{
    return m_iCode;
}

string LogSplitType::GetLabel()
{
    if (m_iCode == Daily)
    {
        return "Daily";
    }
    else if (m_iCode == Monthly)
    {
        return "Monthly";
    }
    else if (m_iCode == Hourly)
    {
        return "Hourly";
    }
    return "";
}
