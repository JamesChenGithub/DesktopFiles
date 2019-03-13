//
// Created by alderzhang on 2017/7/10.
// Copyright (c) 2017 Tencent. All rights reserved.
//

#ifndef TXLITEAVBASIC_TXCTHREADLOCALSTORE_H
#define TXLITEAVBASIC_TXCTHREADLOCALSTORE_H

#if defined(_WIN32)
#include <windows.h>
#include <thr/threads.h>
#include <list>
#include <map>
#include <set>

struct TssResList {
    tss_dtor_t         pfnFree;
    std::map<DWORD, void*>  m_list;

    void add(void* res);

    void clearAll();
};

class TssGC {
public :
    static void add(void* tss, tss_dtor_t pfn , void* res);

    static void uninit();
    static void init();
private :
    static std::map<void*, TssResList>  mTssList;
    static CRITICAL_SECTION    mLock;
    static bool    mLockInitFlag;
    static DWORD   mLastCheckTime;

    static void lock();
    static void unlock();
    static void checkThread();
    static std::set<DWORD> getThreadIdAll(void);
};  // end of  TssGC

#else
#include <pthread.h>
#include <cstdint>

typedef void (*cleanup_route)(void*);
#endif

class TXCThreadLocalStore {
public:
#if defined(_WIN32)
    explicit TXCThreadLocalStore(tss_dtor_t cleanup);
#else
    explicit TXCThreadLocalStore(cleanup_route cleanup);
#endif

    ~TXCThreadLocalStore();

    void* get() const;

    void set(void* value);

private:
    TXCThreadLocalStore(const TXCThreadLocalStore&);
    TXCThreadLocalStore& operator =(const TXCThreadLocalStore&);

private:
#if defined(_WIN32)
    int  m_dwTlsIndex ;
    tss_dtor_t m_cleanup;
#else
    pthread_key_t _key;
#endif
};

class TXCScopeRecursionLimit {
public:
    TXCScopeRecursionLimit(TXCThreadLocalStore* tls);

    ~TXCScopeRecursionLimit();

    bool CheckLimit(int _maxRecursionNum = 1);

    intptr_t Get() const;

private:
    TXCScopeRecursionLimit(const TXCScopeRecursionLimit&);
    TXCScopeRecursionLimit& operator=(const TXCScopeRecursionLimit&);

private:
    TXCThreadLocalStore* _tls;
};

#ifndef __CONCAT__
#define __CONCAT_IMPL(x, y)    x##y
#define __CONCAT__(x, y)    __CONCAT_IMPL(x, y)
#endif

#define    DEFINE_TXCSCOPERECURSIONLIMIT(classname)\
    static TXCThreadLocalStore __CONCAT__(tss, __LINE__)(NULL);\
    TXCScopeRecursionLimit classname(&__CONCAT__(tss, __LINE__))


#endif //TXLITEAVBASIC_TXCTHREADLOCALSTORE_H
