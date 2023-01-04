#ifdef __cplusplus
extern "C" {
#include<unistd.h>
#include<stdio.h>
#include <string.h>
}
#endif
#include <accesscontrol.h>
#include <cdate.h>

void CDate::SetTime( time_t value)
{
	this->Clear();
	this->m_oTime.tv_sec = value;
	this->m_oTime.tv_usec = 0;

	localtime_r( &this->m_oTime.tv_sec, &this->m_oLocalTime);
}

void CDate::SetTime( struct timeval value)
{
	this->Clear();
	this->m_oTime = value;
	localtime_r( &this->m_oTime.tv_sec, &this->m_oLocalTime);
}

bool CDate::ParseDate( string value, CDate& out)
{
	return CDate::ParseDate( value, "%Y-%m-%d %H:%M:%S", out);
}

bool CDate::ParseDate( string value, string format, CDate& out)
{
	struct tm l_oTimeBuff;

	out.Clear();

	memset( &l_oTimeBuff, 0, sizeof( struct tm));

	if ( strptime( value.c_str(), format.c_str(), &l_oTimeBuff) == NULL )
	{
		return false;
	}

	l_oTimeBuff.tm_isdst = -1;

	time_t l_oTime = mktime( &l_oTimeBuff);

	if ( l_oTime == -1 ) return false;

	out.SetTime( l_oTime);

	return true;
}

double CDate::TimeElapsed( CDate oTimeVal, IntervalType iInterval)
{
	double l_eRetVal = 0.0;

	if ( (IntervalType::MicroSecond == iInterval) || (IntervalType::MilliSecond == iInterval) )
	{
		double l_eEndTime = (double) oTimeVal.m_oTime.tv_sec * 1000000 + (double) oTimeVal.m_oTime.tv_usec;
		double l_eStartTime = (double) this->m_oTime.tv_sec * 1000000 + (double) this->m_oTime.tv_usec;

		l_eRetVal = l_eEndTime - l_eStartTime;
		if (IntervalType::MilliSecond == iInterval) l_eRetVal /= 1000;
	}
	else
	{
		l_eRetVal = difftime( oTimeVal.m_oTime.tv_sec, this->m_oTime.tv_sec);

		switch( iInterval)
		{
		case IntervalType::Day:
			l_eRetVal /= 86400;
			break;
		case IntervalType::Hour:
			l_eRetVal /= 3600;
			break;
		case IntervalType::Minute:
			l_eRetVal /= 60;
			break;
		case IntervalType::Second:
			break;
		case IntervalType::Month:
		case IntervalType::Year:
		default:
			l_eRetVal = 0;
			break;
		};
	}

	return l_eRetVal;
}

CDate CDate::Add(IntervalType iInterval, long value)
{
	CDate l_oRetVal;

	tm l_oLocalTime;

	l_oLocalTime = this->m_oLocalTime;

	switch( iInterval)
	{
	case IntervalType::Day:
		l_oLocalTime.tm_mday += value;
		break;
	case IntervalType::Month:
		l_oLocalTime.tm_mon += value;
		break;
	case IntervalType::Year:
		l_oLocalTime.tm_year += value;
		break;
	case IntervalType::Hour:
		l_oLocalTime.tm_hour += value;
		break;
	case IntervalType::Minute:
		l_oLocalTime.tm_min += value;
		break;
	case IntervalType::Second:
	default:
		l_oLocalTime.tm_sec += value;
		break;
	};

	l_oLocalTime.tm_isdst = -1;

	l_oRetVal.SetTime( mktime( &l_oLocalTime));

	return l_oRetVal;
}

CDate CDate::Sub(IntervalType iInterval, long value)
{
	return this->Add( iInterval, -1 * value);
}

CDate CDate::GetDate()
{
	CDate l_oRetVal;

	tm l_oLocalTime;

	l_oLocalTime = this->m_oLocalTime;

	l_oLocalTime.tm_hour = 0;
	l_oLocalTime.tm_min = 0;
	l_oLocalTime.tm_sec = 0;

	l_oLocalTime.tm_isdst = -1;

	l_oRetVal.SetTime( mktime( &l_oLocalTime));

	return l_oRetVal;
}

CDate CDate::GetEOD()
{
	CDate l_oRetVal;

	tm l_oLocalTime;

	l_oLocalTime = this->m_oLocalTime;

	l_oLocalTime.tm_hour = 23;
	l_oLocalTime.tm_min = 59;
	l_oLocalTime.tm_sec = 59;

	l_oLocalTime.tm_isdst = -1;

	l_oRetVal.SetTime( mktime( &l_oLocalTime));

	return l_oRetVal;
}

int CDate::GetYear( void)
{
	return this->m_oLocalTime.tm_year + 1900;
}

int CDate::GetMonth( void)
{
	return this->m_oLocalTime.tm_mon;
}

int CDate::GetDay( void)
{
	return this->m_oLocalTime.tm_mday;
}

int CDate::GetHour( void)
{
	return this->m_oLocalTime.tm_hour;
}

int CDate::GetMinute( void)
{
	return this->m_oLocalTime.tm_min;
}

int CDate::GetSecond( void)
{
	return this->m_oLocalTime.tm_sec;
}

string CDate::ToString( void)
{
	// return this->toString( "%Y-%m-%d %H:%M:%S.%%02u");
	return this->ToString( "%Y-%m-%d %H:%M:%S");
}

string CDate::ToString( string format)
{
	char	pTimeStamp[32];
	char	pBuff[32];

	memset( pTimeStamp, 0, 32);
	memset( pBuff, 0, 32);

	strftime( pBuff, 32, format.c_str(), &this->m_oLocalTime);

	if ( strstr( pBuff, "%") != NULL )
	{
		snprintf( pTimeStamp, 32, pBuff, this->m_oTime.tv_usec);
	}
	else
	{
		strcpy( pTimeStamp, pBuff);
	}

	return pTimeStamp;
}

string CDate::GetUniqueID()
{
	string l_sTxnId = "";
	int l_iCurrentID = 0;
	char l_cBuff[32];
	static int l_iUniqueId = 0;
	static AccessControl l_oAccess;
	l_oAccess.Acquire();
	if ( l_iUniqueId < 999 )
	{
		++l_iUniqueId;
	}
	else
	{
		l_iUniqueId = 1;
	}
	l_iCurrentID = l_iUniqueId;
	l_oAccess.Release();

	CDate l_oCurrTime = Now();

	sprintf( l_cBuff
		   , "%04d%03d%05d%03d"
		   , l_oCurrTime.m_oLocalTime.tm_year + 1900
		   , l_oCurrTime.m_oLocalTime.tm_yday
		   , (l_oCurrTime.m_oLocalTime.tm_hour * 3600) + (l_oCurrTime.m_oLocalTime.tm_min * 60) + l_oCurrTime.m_oLocalTime.tm_sec
		   , l_iCurrentID
		   );

	return l_cBuff;
}

CDate CDate::Now( void)
{
	struct timeval l_oTimeBuff;

	gettimeofday( &l_oTimeBuff , NULL);

	CDate l_oBuff;

	l_oBuff.SetTime( l_oTimeBuff);

	return l_oBuff;
}

CDate CDate::Today( void)
{
	CDate l_oBuff;

	time_t l_oCurrTime;

	struct tm  l_oLocalTime;

	time( &l_oCurrTime);

	localtime_r( &l_oCurrTime, &l_oLocalTime);

	l_oLocalTime.tm_hour = 0;
	l_oLocalTime.tm_min = 0;
	l_oLocalTime.tm_sec = 0;

	l_oLocalTime.tm_isdst = -1;

	l_oBuff.SetTime( mktime( &l_oLocalTime));

	return l_oBuff;
}

CDate::CDate()
{
	this->Clear();
}

CDate::~CDate()
{
	this->Clear();
}

CDate::CDate( struct timeval value)
{
	this->SetTime( value);
}

CDate::CDate( time_t value)
{
	this->SetTime( value);
}

time_t CDate::GetTimeStruct()
{
	return mktime(&this->m_oLocalTime);
}

long int CDate::GetUnixTimestamp()
{
	return static_cast<long int> ( mktime(&this->m_oLocalTime));
}

void CDate::Clear( void)
{
	memset( &this->m_oTime, 0, sizeof( this->m_oTime));
	memset( &this->m_oLocalTime, 0, sizeof( this->m_oLocalTime));
}

