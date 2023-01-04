#include <Arduino.h>
#include <unistd.h>
#include <iostream>
#include <cthread.h>
#include <logger.h>

using namespace std;

CThread::CThread()
{
	this->Init();
}

void CThread::Init()
{
	this->m_bThreadRunning = false;

	this->m_bRunThread = false;

	this->m_bSuspendThread = false;

	this->m_iThreadHandle = 0;

	this->SetSleepInterval( DEFAULT_SLEEP_INTERVAL * 1000);

	this->m_pMethod = NULL;

	this->m_pArgs = NULL;

	this->m_bDetached = false;
}

CThread::~CThread()
{
	this->Join();
}

bool CThread::IsRunning( void)
{
	return this->m_bThreadRunning;
}

void CThread::Initialize( void (*Method)(void*), void* Args)
{
	this->m_pMethod = Method;

	this->m_pArgs = Args;
}

bool CThread::Run()
{
	return this->Run(true);
}

bool CThread::Run( bool Joinable)
{
	int l_iResult = 0;

	pthread_attr_t l_oAttr;

	if ( this->IsRunning() ) return false;

	if ( this->m_pMethod == NULL ) return false;

	this->m_bThreadRunning = false;
	this->m_bRunComplete = false;

	this->m_bRunThread = true;

	this->m_bSuspendThread = false;

	this->m_bDetached = !Joinable;

	pthread_attr_init( &l_oAttr);
	pthread_attr_setdetachstate(&l_oAttr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setdetachstate(&l_oAttr, PTHREAD_CREATE_JOINABLE);

	l_iResult = pthread_create(&this->m_iThreadHandle, &l_oAttr, ThreadRun, (void*)this);

	if ( l_iResult )
	{
		return false;
	}
	pthread_attr_destroy(&l_oAttr);
	this->m_bRunComplete = true;

	return true;
}

bool CThread::Join( void)
{
	// if ( this->m_bDetached ) return false;

	if ( !this->IsRunning() ) return true;

	this->m_bRunThread = false;

	while ( this->IsRunning() ) usleep( this->m_lSleepInterval);

	return true;
}

bool CThread::Suspend( void)
{
	if ( this->m_bDetached ) return false;

	if ( !this->IsRunning() ) return true;

	this->m_bSuspendThread = true;

	sleep( 1);

	return true;
}

void CThread::SetSleepInterval( int Interval)
{
	this->m_lSleepInterval = Interval; // Interval * 1000;
}

void* CThread::ThreadRun( void* pArgs)
{
	CThread* l_pThreadObj = (CThread*)pArgs;

	if ( l_pThreadObj->m_bDetached )
	{
		l_pThreadObj->m_pMethod( l_pThreadObj->m_pArgs);
	}
	else
	{
		while( true)
		{
			if ( !l_pThreadObj->m_bRunThread ) break;
	
			if ( l_pThreadObj->m_bSuspendThread ) continue;
	
			l_pThreadObj->m_pMethod( l_pThreadObj->m_pArgs);
	
			usleep( l_pThreadObj->m_lSleepInterval);
		}
	}

	l_pThreadObj->m_bThreadRunning = false;

	pthread_exit(NULL);

	return NULL;
}











