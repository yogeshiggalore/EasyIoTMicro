#if !defined(SYNC_H_INCLUDED)

#if defined(WIN32)
#include "stdafx.h"
#else
#include <pthread.h>
#endif

#define SYNC_H_INCLUDED 1

class AccessControl
{
public:
#if defined(WIN32)
	AccessControl() { 
		InitializeCriticalSection(&m_CriticalSection); 
	}
	~AccessControl() { 
		DeleteCriticalSection(&m_CriticalSection); 
	}
	void Acquire() { 
		EnterCriticalSection(&m_CriticalSection); 
	}
	void Release() { 
		LeaveCriticalSection(&m_CriticalSection); 
	}
private:
	CRITICAL_SECTION m_CriticalSection;              // Synchronization object
#else
	AccessControl() { 
		if ( ! pthread_mutex_init( &m_oAccess, NULL) ) {
			m_bInitialized = true;
		}
	}
	~AccessControl() { 
		if ( m_bInitialized ) {
			pthread_mutex_destroy( &m_oAccess);
		}
	}
	void Acquire() { 
		if ( m_bInitialized ) {
			pthread_mutex_lock( &m_oAccess);
		}
	}
	void Release() { 
		if ( m_bInitialized ) {
			pthread_mutex_unlock( &m_oAccess);
		}
	}
private:
	bool m_bInitialized = false;
	pthread_mutex_t m_oAccess;
#endif
};

#endif
