#pragma once

#include <windows.h>
#include <Synchapi.h>

namespace incolun{
namespace clothodb{

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

class ConditionalSRWLockExclusive
{
public:

    ConditionalSRWLockExclusive(bool needLock, RTL_SRWLOCK& srwLock) :
        m_srwLock(srwLock),
        m_needLock(needLock)
    {
        if(m_needLock)
            ::AcquireSRWLockExclusive(&m_srwLock);
    }

    ~ConditionalSRWLockExclusive()
    {
        if(m_needLock)
            ::ReleaseSRWLockExclusive(&m_srwLock);
    }

private:
    RTL_SRWLOCK& m_srwLock;
    bool m_needLock;
};

}}

