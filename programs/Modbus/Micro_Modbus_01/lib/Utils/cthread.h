#ifndef THREAD_H_INCLUDED

#define THREAD_H_INCLUDED 1

#include <pthread.h>

using namespace std;

const long DEFAULT_SLEEP_INTERVAL = 1; // 1 millisec

class CThread
{
public:
	// Methods

	CThread();

	~CThread();

	bool IsRunning( void);

	void Initialize( void (*Method)(void*), void* Args);

	bool Run( bool Joinable);
	bool Run();

	bool Join( void);

	bool Suspend();

	void SetSleepInterval( int Interval);
private:
	void Init();

	static void* ThreadRun( void*);
	
protected:
	long m_lSleepInterval;

	bool m_bThreadRunning;
	bool m_bRunComplete;

	bool m_bRunThread;

	bool m_bSuspendThread;

	bool m_bDetached;

	pthread_t m_iThreadHandle;

	void (*m_pMethod) (void*);

	void* m_pArgs;

};

#endif
