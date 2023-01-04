#ifndef CQUEUE_H
#define CQUEUE_H 1

#include<accesscontrol.h>
#include<vector>
using namespace std;

typedef unsigned long ulong;

template <typename T>
class CQueue {
public:
	CQueue();
	~CQueue();
	void		PushBack(T);		// To put a element in the back of the queue
	void		PushFront(T);		// To put a element in the back of the queue
	bool		PopFront(T&);		// To get the first element in the queue
	bool		Front(T&);			// To get the first element in the queue
	bool		Get( int, T&);		// To get the first element in the queue
	bool		Erase( int);		// To get the first element in the queue
	ulong		Size();				// Returns the size of the queue
	void		Clear();			// Clears all emenent
	bool		IsEmpty();			// Returns whether the queue is empty or not
	void		Initialize( int);
private:
	vector<T>			m_vArray;
	AccessControl		m_oMutex;
};

template <typename T> 
void CQueue<T>::Initialize( int iSize) {
	this->m_oMutex.Acquire();
	this->m_vArray.reserve( iSize);
	this->m_oMutex.Release();
}

template <typename T> 
CQueue<T>::CQueue() {
	this->Clear();
}

template <typename T> 
void CQueue<T>::Clear() {
	
	this->m_oMutex.Acquire();
	this->m_vArray.clear();
	this->m_oMutex.Release();
}

template <typename T> 
CQueue<T>::~CQueue() {
	this->Clear();
}

template <typename T> 
void CQueue<T>::PushBack( T oData) {
	bool bAccessAcquired = false;

	try {
		this->m_oMutex.Acquire();
		bAccessAcquired = true;
		this->m_vArray.push_back( oData);
		this->m_oMutex.Release();
		bAccessAcquired = false;
	} catch(...) {
		if ( bAccessAcquired ) this->m_oMutex.Release();
	}
}

template <typename T> 
void CQueue<T>::PushFront(T oData) {
	bool bAccessAcquired = false;

	try {
		this->m_oMutex.Acquire();
		bAccessAcquired = true;
		this->m_vArray.insert(this->m_vArray.begin(), oData);
		this->m_oMutex.Release();
		bAccessAcquired = false;
	} catch(...) {
		if ( bAccessAcquired ) this->m_oMutex.Release();
	}
}


template <typename T> 
bool CQueue<T>::PopFront(T& oData) {
	bool bAccessAcquired = false;

	try {
		if ( this->m_vArray.size() == 0 ) return false;

		this->m_oMutex.Acquire();
		bAccessAcquired = true;
		oData = this->m_vArray.at(0);
		this->m_vArray.erase( this->m_vArray.begin());

		this->m_oMutex.Release();
		bAccessAcquired = false;
		return true;
	} catch(...) {
		if ( bAccessAcquired ) this->m_oMutex.Release();
		return false;
	}
}

template <typename T> 
bool CQueue<T>::Get( int iIndex, T& oData) {
	bool bAccessAcquired = false;

	try {
		if ( (iIndex < 0 ) || (iIndex >= this->m_vArray.size()) ) return false;

		this->m_oMutex.Acquire();
		bAccessAcquired = true;
		oData = this->m_vArray.at(iIndex);
		this->m_oMutex.Release();
		bAccessAcquired = false;
		return true;
	} catch(...) {
		if ( bAccessAcquired ) this->m_oMutex.Release();
		return false;
	}
}

template <typename T> 
bool CQueue<T>::Erase( int iIndex) {
	bool bAccessAcquired = false;

	try {
		if ( (iIndex < 0 ) || (iIndex >= this->m_vArray.size()) ) return false;

		this->m_oMutex.Acquire();
		bAccessAcquired = true;
		this->m_vArray.erase( this->m_vArray.begin() + iIndex);
		this->m_oMutex.Release();
		bAccessAcquired = false;
		return true;
	} catch(...) {
		if ( bAccessAcquired ) this->m_oMutex.Release();
		return false;
	}
}

template <typename T> 
bool CQueue<T>::Front(T& oData) {
	bool bAccessAcquired = false;

	try {

		if ( this->m_vArray.size() == 0 ) return false;

		this->m_oMutex.Acquire();
		bAccessAcquired = true;
		oData = this->m_vArray.at(0);
		this->m_oMutex.Release();
		bAccessAcquired = false;
		return true;
	} catch(...) {
		if ( bAccessAcquired ) this->m_oMutex.Release();
		return false;
	}
}

template <typename T> 
ulong CQueue<T>::Size(void){
	return this->m_vArray.size();
}
template <typename T> 
bool CQueue<T>::IsEmpty(void) {
	if ( this->m_vArray.size() > 0 ) return false;
	return true;
}


#endif

