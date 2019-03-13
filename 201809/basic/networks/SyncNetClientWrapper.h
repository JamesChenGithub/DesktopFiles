#ifndef __NETCLIENTWRAPPER_H__
#define __NETCLIENTWRAPPER_H__

#define SE_BROKEN	-1		//发生不可挽回的错误
#define SE_INTR		-2		//被中断事件打断
#define SE_TIMEOUT	-3		//操作超时
#define SE_REFUSE	-4		//远端主动断开连接

#ifdef ENABLE_QUIC
#include <map>
#include "TXCCondition.h"
#include "qcloud_live_net_client.h"
#include "qcloud_live_net_client_context.h"

#else
#include <stdio.h>
#include <errno.h>

#if defined(WIN32) || defined(WIN64) || defined(_CONSOLE)
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#else

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <zconf.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netdb.h>

#endif // WIN32

#endif

class CTXSyncNetClientWrapper
{
public:
    CTXSyncNetClientWrapper(bool useQuic, bool nodelay = false);
    
    virtual ~CTXSyncNetClientWrapper();
    
    
public:
    /**
     * 检查是否已连接
     * @return 是否已连接
     */
    virtual bool isConnected();
    
    
    /**
     * 连接到主机
     * @param host		主机名称或地址
     * @param port 		主机端口
     * @param timeout 	连接超时时间，单位毫秒，-1表示永不超时
     * @return 是否连接成功（异步模式下总是返回true）
     */
    virtual bool connect(const char * host, short port, long timeout = 60000);
    
    
    /**
     * 关闭连接
     * @return 是否成功（异步模式下总是返回true）
     */
    virtual bool close();
    
    
    /**
     * 发送数据到远端（异步模式下放入发送缓冲区等待发送）
     * @param buffer	要发送的数据缓冲区
     * @param size		要发送的数据大小
     * @return 发送字节数（异步模式下为存放到发送缓冲区的字节数），-1：出现无法恢复的错误
     */
    virtual int send(const void *buffer, unsigned int size);
    
    
    /**
     * 从远端接收数据（异步模式下无效）
     * @param buffer	存放接收数据的缓冲区
     * @param size		缓冲区大小
     * @return 接收字节数，-1：出现无法恢复的错误
     */
    virtual int recv(void *buffer, unsigned int size);
    
    
    /**
     * 设置发送超时时间（异步模式下无效）
     * @param timeout	发送超时时间，单位毫秒，0表示取消超时设置，-1表示恢复上次设置
     * @return 设置是否成功
     */
    virtual bool setSendTimeout(long timeout);
    
    
    /**
     * 设置接收超时时间（异步模式下无效）
     * @param timeout	接收超时时间，单位毫秒，0表示取消超时设置，-1表示恢复上次设置
     * @return 设置是否成功
     */
    virtual bool setRecvTimeout(long timeout);

  
    /**
     * 获取错误码及错误描述信息
     * @param error 出参，错误码
     * @param description 出参，错误描述信息
     */
    virtual void getLastError(int* error, char** description);
    
    virtual void getNetSpeedAndBytesInflight(int& net_speed, int& bytes_in_flight);
    
    virtual unsigned long long getConnectionID();
    
    virtual void getConnectionStats(char ** connectionStats);
    
    virtual bool enableQuic() {return m_useQuic;}
    
protected:
#ifdef ENABLE_QUIC
    void getErrorNo(unsigned long long & connection_id, unsigned int & stream_id, int & close_reason);
    
    void reportStatisticInfo(std::map<std::string, std::string> & mapInfo);
#else
    int getLastError();
    
    bool newSocket(int sin_family);
    
    bool makeBlocking();
    
    bool makeNonblocking();
    
    bool closeSocket();
#endif
private:
    bool                                                m_useQuic;
    
    int                                                 m_nSendTimeOut;
    
    int                                                 m_nRecvTimeOut;
    
    bool                                                m_bConnected;
    
    int                                                 m_nError;
    
    char                                                m_pDescription[512];
    
    char                                                m_pConnectionStats[10240];
#ifdef ENABLE_QUIC
    
    qcloud::QcloudLiveSyncNetClient*                    m_pNetClient;
    
    bool                                                m_bConnClose;
#else
    int m_nRecvBufSize;
    
    int m_nSendBufSize;
    
    int m_nRwFlags;
    
#ifdef WIN32
    SOCKET m_nSock;
#else
    int m_nSock;
#endif

#endif
};


#endif /* __NETCLIENTWRAPPER_H__ */
