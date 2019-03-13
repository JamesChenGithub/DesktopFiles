//
// Created by alderzhang on 2017/7/10.
// Copyright (c) 2017 Tencent. All rights reserved.
//

#include "TXCThreadLocalStore.h"

#if defined(_WIN32)

#include <TlHelp32.h>

void TssResList::add(void *res) {
    m_list[::GetCurrentThreadId()] = res;
}

void TssResList::clearAll() {
    if (m_list.size() <= 0)
        return;

    for (auto itor = m_list.begin() ; itor != m_list.end() ; itor++) {
        void* res = (*itor).second;

        if (res != NULL) {
            pfnFree(res);
        }
    }

    m_list.clear();
}

std::map<void*, TssResList> TssGC::mTssList;
CRITICAL_SECTION            TssGC::mLock;

bool    TssGC::mLockInitFlag = false;
DWORD   TssGC::mLastCheckTime;

void TssGC::add(void* tss, tss_dtor_t pfn , void* res) {
    if (!tss || !pfn)
        return;

    lock();

    int thread_id = ::GetCurrentThreadId();

    mTssList[tss].pfnFree = pfn;
    mTssList[tss].m_list[thread_id] = res;

    checkThread();
    unlock();
}

void TssGC::uninit() {
    lock();

    //±éÀúËùÓÐ tss
    for (auto itor = mTssList.begin() ; itor != mTssList.end() ; itor++) {
        TssResList* tss = &((*itor).second);

        // ±éÀútss res
        for (auto tss_item = tss->m_list.begin();  tss_item != tss->m_list.end();) {
            DWORD id = (*tss_item).first;
            void*  res = (*tss_item).second;
            tss->pfnFree(res);
            tss->m_list.erase(tss_item++);
        }
    }

    mTssList.clear();
    unlock();
}

void TssGC::init() {

}

void TssGC::lock() {
    if (!mLockInitFlag) {
        mLockInitFlag = true;
        ::InitializeCriticalSection(&mLock);
    }

    ::EnterCriticalSection(&mLock);
}

void TssGC::unlock() {
    ::LeaveCriticalSection(&mLock);
}

#define   TIME_MIN_30    ((30 * 60 * 1000))

void TssGC::checkThread() {
    if (mLastCheckTime == 0) {
        mLastCheckTime = ::GetTickCount();
        return;
    }

    DWORD time_span = ::GetTickCount() - mLastCheckTime;

    if (time_span  <  TIME_MIN_30)
        return;

    mLastCheckTime = ::GetTickCount();

    auto threadIds = getThreadIdAll();

    //�������� tss
    for (auto itor = mTssList.begin() ; itor != mTssList.end() ; itor++) {
        TssResList* tss = &((*itor).second);

        // ±éÀútss res
        for (auto tss_item = tss->m_list.begin();  tss_item != tss->m_list.end();) {
            DWORD id = (*tss_item).first;
            void*  res = (*tss_item).second;

            if (threadIds.find(id) == threadIds.end()) {  //É¾³ý res
                tss->pfnFree(res);
                tss->m_list.erase(tss_item++);
            } else {
                tss_item++;
            }
        }
    }
}

static std::set<DWORD> TssGC::getThreadIdAll() {
    std::set<DWORD> idList;
    DWORD processId = ::GetCurrentProcessId();
    HANDLE hmeSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, processId);

    if (hmeSnapshot == NULL)
        return idList;

    THREADENTRY32 te = { sizeof(te) };
    BOOL fOK1 = Thread32First(hmeSnapshot, &te);

    for (; fOK1; fOK1 = Thread32Next(hmeSnapshot, &te)) {
        if (te.th32OwnerProcessID != processId)
            continue;

        idList.insert(te.th32ThreadID);
    }

    return idList;
}

TXCThreadLocalStore::TXCThreadLocalStore(tss_dtor_t cleanup) {
    m_dwTlsIndex = TlsAlloc();
    m_cleanup = cleanup;
}

TXCThreadLocalStore::~TXCThreadLocalStore() {
    TlsFree(m_dwTlsIndex);
}

void *TXCThreadLocalStore::get() const {
    return TlsGetValue(m_dwTlsIndex);
}

void TXCThreadLocalStore::set(void *value) {
    TlsSetValue(m_dwTlsIndex, value);
    TssGC::add(this, m_cleanup, value);
}

#else
TXCThreadLocalStore::TXCThreadLocalStore(cleanup_route cleanup) {
    pthread_key_create(&_key, cleanup);
}

TXCThreadLocalStore::~TXCThreadLocalStore() {
    pthread_key_delete(_key);
}

void *TXCThreadLocalStore::get() const {
    return pthread_getspecific(_key);
}

void TXCThreadLocalStore::set(void *value) {
    pthread_setspecific(_key, value);
}

#endif

TXCScopeRecursionLimit::TXCScopeRecursionLimit(TXCThreadLocalStore *tls)
        : _tls(tls) {
    _tls->set((void*)((intptr_t)_tls->get() + 1));
}

TXCScopeRecursionLimit::~TXCScopeRecursionLimit() {
    _tls->set((void*)((intptr_t)_tls->get() - 1));
}

bool TXCScopeRecursionLimit::CheckLimit(int _maxRecursionNum) {
    return (intptr_t)_tls->get() > _maxRecursionNum;
}

intptr_t TXCScopeRecursionLimit::Get() const {
    return (intptr_t)_tls->get();
}
