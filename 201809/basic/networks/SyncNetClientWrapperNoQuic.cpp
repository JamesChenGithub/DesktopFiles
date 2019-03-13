//
// Created by alderzhang on 2017/6/29.
//

#include <netinet/tcp.h>
#include <chrono>
#include <string.h>
#include "SyncNetClientWrapper.h"
#include "txg_log.h"
#include "../utils/txg_time_util.h"


#define GetSockError()	errno


CTXSyncNetClientWrapper::CTXSyncNetClientWrapper(bool useQuic, bool nodelay):
m_useQuic(false),
m_nRecvBufSize(64 * 1024),
m_nSendBufSize(48 * 1024),
m_nSendTimeOut(3500),
m_nRecvTimeOut(3000),
m_bConnected(false),
m_nRwFlags(0)
{
#if defined(WIN32) || defined(WIN64) || defined(_CONSOLE)
    //Init WSA
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    WSAStartup(sockVersion, &wsaData);
#endif
            
    m_nError = 0;
    memset(m_pDescription, 0, sizeof(m_pDescription));
}

CTXSyncNetClientWrapper::~CTXSyncNetClientWrapper() {
    closeSocket();
#if defined(WIN32) || defined(WIN64) || defined(_CONSOLE)
    WSACleanup();
#endif
    
    m_nError = 0;
    memset(m_pDescription, 0, sizeof(m_pDescription));
}

bool CTXSyncNetClientWrapper::newSocket(int sin_family) {
    m_nSock = ::socket(sin_family, SOCK_STREAM, IPPROTO_TCP);
    if (m_nSock == -1) {
        return false;
    }
#ifdef __APPLE__
    int set = 1;
    ::setsockopt(m_nSock, SOL_SOCKET, SO_NOSIGPIPE, (const char*) &set, sizeof(set));
#endif

    ::setsockopt(m_nSock, SOL_SOCKET, SO_RCVBUF, (const char*) &m_nRecvBufSize, sizeof(int));

    ::setsockopt(m_nSock, SOL_SOCKET, SO_SNDBUF, (const char*) &m_nSendBufSize, sizeof(int));

    int on = 0;
    ::setsockopt(m_nSock, IPPROTO_TCP, TCP_NODELAY, (const char*) &on, sizeof(on));

    int optval = 1;
    ::setsockopt(m_nSock, SOL_SOCKET, SO_REUSEADDR, (const char*) &optval, sizeof(optval));
    return true;
}

bool CTXSyncNetClientWrapper::makeBlocking() {
    unsigned long ul = 0;
#if defined(WIN32) || defined(WIN64) || defined(_CONSOLE)
    if(::ioctlsocket(m_nSock, FIONBIO, (unsigned long*)&ul) == SOCKETm_nError){
#else
    if (::ioctl(m_nSock, FIONBIO, &ul) == -1) {
#endif
        return false;
    }
    m_nRwFlags = 0;
    return true;
}

bool CTXSyncNetClientWrapper::makeNonblocking() {
    unsigned long ul = 1;
#if defined(WIN32) || defined(WIN64) || defined(_CONSOLE)
    if(::ioctlsocket(m_nSock, FIONBIO, (unsigned long*)&ul) == SOCKETm_nError){
        return false;
    }
    m_nRwFlags = 0;
#else
    if (::ioctl(m_nSock, FIONBIO, &ul) == -1) {
        return false;
    }
    m_nRwFlags = MSG_DONTWAIT;
#endif
    return true;
}

bool CTXSyncNetClientWrapper::closeSocket() {
    m_bConnected = false;
#if defined(WIN32) || defined(WIN64) || defined(_CONSOLE)
    return ::closesocket(m_nSock) >= 0;
#else
    return ::close(m_nSock) >= 0;
#endif
}

bool CTXSyncNetClientWrapper::isConnected() {
    return m_bConnected;
}
    
int CTXSyncNetClientWrapper::getLastError(){
#if defined(WIN32) || defined(WIN64) || defined(_CONSOLE)
    return WSAGetLastError();
#else
    return errno;
#endif
}

bool CTXSyncNetClientWrapper::connect(const char * host, short port, long timeout) {
    if (m_bConnected) return true;
//    std::chrono::high_resolution_clock::time_point startTimePoint = std::chrono::high_resolution_clock::now();
    uint64_t startTimePoint = txf_gettickcount();
    
    bool ipv6 = false;
    //host是点分十进制的IP地址
    if (strchr(host, '.') != NULL) {        //IP V4用.分隔
        ipv6 = false;
    }
    else if (strchr(host, ':') != NULL) {   //IP V6用：分隔
        ipv6 = true;
    }
    else {
        LOGE("%s, invalid ip address %s", __FUNCTION__, host);
    }
    
    if (ipv6) {
        newSocket(AF_INET6);
        
        sockaddr_in6 toAddr;
        memset(&toAddr, 0, sizeof(toAddr));
        toAddr.sin6_family = AF_INET6;
        toAddr.sin6_port = htons(port);
        inet_pton(AF_INET6, host, &(toAddr.sin6_addr));
        
        makeNonblocking();
        m_bConnected = ::connect(m_nSock, reinterpret_cast<sockaddr *>(&toAddr), sizeof(toAddr)) != -1;
    }
    else {
        newSocket(AF_INET);
        
        sockaddr_in toAddr;
        toAddr.sin_addr.s_addr = inet_addr(host);
        toAddr.sin_family = AF_INET;
        toAddr.sin_port = htons(port);
        if (toAddr.sin_addr.s_addr == INADDR_NONE) {
            struct hostent *hp = gethostbyname(host);
            if (!hp || !hp->h_addr) return false;
            toAddr.sin_addr = *(struct in_addr *) hp->h_addr;
        }
        
        makeNonblocking();
        m_bConnected = ::connect(m_nSock, reinterpret_cast<sockaddr *>(&toAddr), sizeof(toAddr)) != -1;
    }
    
    if (!m_bConnected) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(m_nSock, &fds);//将Socket加入集合
        struct timeval tv;
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = static_cast<int>(timeout % 1000 * 1000);
        if (::select(m_nSock + 1, nullptr, &fds, nullptr, &tv) > 0) {
            int error = -1;
            int len = sizeof(int);
#if defined(WIN32) || defined(WIN64) || defined(_CONSOLE)
            ::getsockopt(m_nSock, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&error), &len);
#else
            ::getsockopt(m_nSock, SOL_SOCKET, SO_ERROR, &error, reinterpret_cast<socklen_t *>(&len));
#endif
            if (error == 0) {
                m_bConnected = true;
            }
        }
    }
    makeBlocking();
    if (!m_bConnected) {
        close();
        uint64_t consumeDuration = txf_gettickspan(startTimePoint);
        if(consumeDuration < timeout){//超时时间未消耗完
            uint64_t remainDuration = timeout - consumeDuration;
            struct timeval tv;
            tv.tv_sec = remainDuration / 1000;
            tv.tv_usec = static_cast<int>(remainDuration % 1000 * 1000);
            
            select(0, NULL, NULL, NULL, &tv);
        }
    } else {
        setSendTimeout(-1);
        setRecvTimeout(-1);
    }
    
    if (!m_bConnected) {
        LOGE("%s, connect error %d", __FUNCTION__, GetSockError());
        
        m_nError = GetSockError();
        memset(m_pDescription, 0, sizeof(m_pDescription));
        sprintf(m_pDescription, "TCP通道连接失败，错误码[%d]", m_nError);
    }
    
    return m_bConnected;
}

bool CTXSyncNetClientWrapper::close() {
    return closeSocket();
}

int CTXSyncNetClientWrapper::send(const void *buffer, unsigned int size) {
    if (!m_bConnected) return SE_BROKEN;
    int result = static_cast<int>(::send(m_nSock, reinterpret_cast<const char *>(buffer), size, m_nRwFlags));
    if (result < 0) {
        int err = getLastError();
        if (err == EINTR) {
            result = SE_INTR;
        } else if (err == EWOULDBLOCK || err == EAGAIN) {
            result = SE_TIMEOUT;
        } else {
            if (err == EPIPE || err == ECONNRESET) {
                result = SE_REFUSE;
            } else {
                result = SE_BROKEN;
            }
            m_bConnected = false;
        }
        
        LOGE("%s, send error %d (%d bytes)", __FUNCTION__, GetSockError(), result);
        
        m_nError = GetSockError();
        memset(m_pDescription, 0, sizeof(m_pDescription));
        sprintf(m_pDescription, "TCP通道发送失败，错误码[%d]", m_nError);
    }
    return result;
}

int CTXSyncNetClientWrapper::recv(void *buffer, unsigned int size) {
    int result = 0;
    if (!m_bConnected) return SE_BROKEN;
    result = static_cast<int>(::recv(m_nSock, reinterpret_cast<char *>(buffer), size, m_nRwFlags));
    if (result == 0) {
        m_bConnected = false;
    }
    if (result < 0) {
        int err = getLastError();
        if (err == EINTR) {
            result = SE_INTR;
        } else if (err == EWOULDBLOCK || err == EAGAIN) {
            result = SE_TIMEOUT;
        } else {
            if (err == ECONNRESET) {
                result = SE_REFUSE;
            }
            else{
                result = SE_BROKEN;
            }
            m_bConnected = false;
        }

        LOGE("%s, recv error %d (%d bytes)", __FUNCTION__, GetSockError(), result);
        
        m_nError = GetSockError();
        memset(m_pDescription, 0, sizeof(m_pDescription));
        sprintf(m_pDescription, "TCP通道接收失败，错误码[%d]", m_nError);
    }
    return result;
}

bool CTXSyncNetClientWrapper::setSendTimeout(long timeout) {
    if (timeout == -1) {
        timeout = m_nSendTimeOut;
    } else {
        m_nSendTimeOut = timeout;
    }
#if defined(WIN32) || defined(WIN64) || defined(_CONSOLE)
    int tv = timeout;
#else
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = static_cast<int>(timeout % 1000 * 1000);
#endif
    return ::setsockopt(m_nSock, SOL_SOCKET, SO_SNDTIMEO, (const char*) &tv, sizeof(tv)) == 0;
}

bool CTXSyncNetClientWrapper::setRecvTimeout(long timeout) {
    if (timeout == -1) {
        timeout = m_nRecvTimeOut;
    } else {
        m_nRecvTimeOut = timeout;
    }
#if defined(WIN32) || defined(WIN64) || defined(_CONSOLE)
    int tv = timeout;
#else
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = static_cast<int>(timeout % 1000 * 1000);
#endif
    return ::setsockopt(m_nSock, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv, sizeof(tv)) == 0;
}

void CTXSyncNetClientWrapper::getLastError(int* error, char** description) {
    if (error) {
        *error = m_nError;
    }
    if (description) {
        *description = m_pDescription;
    }
}
    
void CTXSyncNetClientWrapper::getNetSpeedAndBytesInflight(int& net_speed, int& bytes_in_flight) {
#if defined(__APPLE__)
    socklen_t len = sizeof(int);
    getsockopt(m_nSock, SOL_SOCKET, SO_NWRITE, &bytes_in_flight, &len);
#elif defined(ANDROID)
    ioctl(m_nSock, SIOCOUTQ, bytes_in_flight);
#else
    bytes_in_flight = -1;
#endif
}
    
unsigned long long CTXSyncNetClientWrapper::getConnectionID()
{
    return 0;
}
    
void CTXSyncNetClientWrapper::getConnectionStats(char ** connectionStats)
{

}
