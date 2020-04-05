/*
 *	File: common/unnamedsemaphorelite.hpp
 *
 *	Created on: 14 Dec 2016
 *	Created by: Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file semaphore for interprocess usage
 *  This semaphore implemets only 2 functions wait() and post()
 *
 */
#ifndef COMMON_UNNAMEDSEMAPHORELITE_HPP
#define COMMON_UNNAMEDSEMAPHORELITE_HPP

#include <stddef.h>
#if defined(_WIN32)
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#elif defined(__APPLE__)
#include <dispatch/dispatch.h>
#ifndef INFINITE
#define INFINITE	-1
#endif
#else
#include <semaphore.h>
#include <sys/time.h>
#define SHARING_TYPE	0/* 0 means semaphores is shared between threads in same process */
#ifndef INFINITE
#define INFINITE	-1
#endif
#endif

#include <time.h>

namespace common{

#ifndef TYPE_SEMA_defined
#define TYPE_SEMA_defined
#if defined(_WIN32)
typedef HANDLE TYPE_SEMA;
#elif defined(__APPLE__)
typedef dispatch_semaphore_t TYPE_SEMA;
#else
typedef sem_t TYPE_SEMA;
#endif
#endif  // #ifndef TYPE_SEMA_defined

class UnnamedSemaphoreLite
{
public:
    UnnamedSemaphoreLite()
    {
#if defined(WIN32)
        m_Semaphore = CreateSemaphore( 0, (LONG)0, (LONG)100, 0 );
#elif defined(__APPLE__)
        m_Semaphore = dispatch_semaphore_create(0); // init with value of 0
#else
        sem_init( &m_Semaphore, SHARING_TYPE, 0 );
#endif
    }

    ~UnnamedSemaphoreLite()
    {
#if defined(WIN32)
        CloseHandle( m_Semaphore );
#elif defined(__APPLE__)
        dispatch_release(m_Semaphore);
#else
        sem_destroy( &m_Semaphore );
#endif
    }

    void post()
    {
#if defined(WIN32)
        ReleaseSemaphore( m_Semaphore, 1, NULL  );
#elif defined(__APPLE__)
        dispatch_semaphore_signal(m_Semaphore);
#else
        sem_post( &m_Semaphore );
#endif
    }


    int wait(int a_WaitMs= INFINITE)
    {
#if defined(WIN32)
        return WaitForSingleObjectEx( m_Semaphore, a_WaitMs,TRUE )== WAIT_OBJECT_0 ? 0 : -1;
#elif defined(__APPLE__)
        dispatch_semaphore_wait(m_Semaphore, DISPATCH_TIME_FOREVER);
#else
        if(a_WaitMs<0){
            return sem_wait( &m_Semaphore );
        }
        else{
            struct timespec finalAbsTime;
            struct timeval currentTime;
            long long int nExtraNanoSeconds;
            gettimeofday(&currentTime,NULL);
            nExtraNanoSeconds = (long long int)currentTime.tv_usec*1000 + (long long int)((a_WaitMs%1000)*1000000);
            finalAbsTime.tv_sec = currentTime.tv_sec + a_WaitMs / 1000 + nExtraNanoSeconds/1000000000;
            finalAbsTime.tv_nsec = nExtraNanoSeconds%1000000000;
            return sem_timedwait(&m_Semaphore,&finalAbsTime);
        }
        return -1;
#endif
    }

private:
    TYPE_SEMA m_Semaphore;
};

}

#endif // COMMON_UNNAMEDSEMAPHORELITE_HPP
