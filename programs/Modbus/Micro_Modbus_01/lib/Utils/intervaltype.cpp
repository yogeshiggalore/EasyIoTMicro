#include<string>
#include<intervaltype.h>
using namespace std;

IntervalType IntervalType::Parse(int code)
{
    if (code == Month) {
        return Month;
    }
    else if (code == Year) {
        return Year;
    }
    else if (code == Hour) {
        return Hour;
    }
    else if (code == Minute) {
        return Minute;
    }
    else if (code == Second) {
        return Second;
    }
    else if (code == MicroSecond) {
        return MicroSecond;
    }
    else if (code == MilliSecond) {
        return MilliSecond;
    }

    return Day;
}

string IntervalType::ToString()
{
    return this->GetLabel();
}

uint8 IntervalType::GetCode()
{
    return m_iCode;
}

string IntervalType::GetLabel()
{
    if (m_iCode == Month) {
        return "Month";
    }
    else if (m_iCode == Year) {
        return "Year";
    }
    else if (m_iCode == Hour) {
        return "Hour";
    }
    else if (m_iCode == Minute) {
        return "Minute";
    }
    else if (m_iCode == Second) {
        return "Second";
    }
    else if (m_iCode == MicroSecond) {
        return "MicroSecond";
    }
    else if (m_iCode == MilliSecond) {
        return "MilliSecond";
    }

    return "Day";
}
