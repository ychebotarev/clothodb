#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <Synchapi.h>

namespace cdb{

class SRWLockShared
{
public:

    SRWLockShared(RTL_SRWLOCK& srwLock):m_srwLock(srwLock)
    {
        ::AcquireSRWLockShared(&m_srwLock);
    }

    ~SRWLockShared()
    {
        ::ReleaseSRWLockShared(&m_srwLock);
    }

private:
    RTL_SRWLOCK& m_srwLock;
};

class SRWLockExclusive
{
public:

    SRWLockExclusive(RTL_SRWLOCK& srwLock):m_srwLock(srwLock)
    {
        ::AcquireSRWLockExclusive(&m_srwLock);
    }

    ~SRWLockExclusive()
    {
        ::ReleaseSRWLockExclusive(&m_srwLock);
    }

private:
    RTL_SRWLOCK& m_srwLock;
};

}