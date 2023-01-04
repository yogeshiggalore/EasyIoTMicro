#ifndef CDATE_H
#define CDATE_H 1

#ifdef __cplusplus
extern "C" {
#include<time.h>
#include<sys/time.h>
}
#endif
#include<string>
#include<intervaltype.h>

using namespace std;

class CDate
{
public:
	CDate();

	CDate( time_t);

	CDate( struct timeval);

	~CDate();

	void Clear( void);

	void SetTime( time_t);
	void SetTime( struct timeval);

	double TimeElapsed( CDate, IntervalType);

	int GetYear( void);
	int GetMonth( void);
	int GetDay( void);
	int GetHour( void);
	int GetMinute( void);
	int GetSecond( void);
	CDate GetDate();
	CDate GetEOD();
	time_t GetTimeStruct();
	long int GetUnixTimestamp();

	CDate Add(IntervalType, long);
	CDate Sub(IntervalType, long);

	static CDate Now( void);
	static CDate Today( void);

	static bool ParseDate( string value, CDate& out);
	static bool ParseDate( string value, string format, CDate& out);

	string ToString();

	string ToString( string);

	static string GetUniqueID();
private:
	// time_t m_oTime;
	struct timeval m_oTime;
	tm m_oLocalTime;
};

#endif

