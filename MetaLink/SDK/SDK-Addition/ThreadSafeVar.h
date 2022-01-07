#pragma once

// https://stackoverflow.com/questions/40645108/is-there-a-c-standard-for-thread-safe-class-variables

#include "../BaseIncludes.h"
#include "../SDK/tier0/threadtools.h"
#include <mutex>

template <class T>
class ThreadSafeVar
{
private:
    T safeVar;
    std::mutex accessMutex;

public:
    ThreadSafeVar(T initialValue) { safeVar = initialValue; }
    ThreadSafeVar<T>& operator=(const T& source) { set(source); return *this; }
    ThreadSafeVar<T>& operator=(ThreadSafeVar<T>& source) { set(source.get()); return *this; };

    void set(T newValue)
    {
        std::lock_guard<std::mutex> lock(accessMutex);
        safeVar = newValue;
    };

    T const get(void)
    {
        std::lock_guard<std::mutex> lock(accessMutex);
        return safeVar;
    };
};


class LockInstance
{
public:
    LockInstance(std::mutex* pMutex) : m_pMutex(pMutex) { m_pMutex->lock(); }
    ~LockInstance() { if (m_pMutex) m_pMutex->unlock(); }
private:
    std::mutex* m_pMutex;
};

class CThreadFastMutexLockInstance
{
public:
    CThreadFastMutexLockInstance(CThreadFastMutex* pMutex) : m_pMutex(pMutex) { m_pMutex->Lock(); }
    ~CThreadFastMutexLockInstance() { if (m_pMutex) m_pMutex->Unlock(); }
private:
    CThreadFastMutex* m_pMutex;
};
