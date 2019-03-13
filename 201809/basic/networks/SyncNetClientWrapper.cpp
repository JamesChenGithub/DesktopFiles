#include "SyncNetClientWrapper.h"

#include "txg_log.h"
#include "tx_dr_def.h"
#include "tx_dr_api.h"
//#include "local_ipstack.h"
//#include "socket_address.h"

#include "NetClientContextWrapper.h"

#undef _MODULE_
#define _MODULE_ "SyncNetClientWrapper"

CTXSyncNetClientWrapper::CTXSyncNetClientWrapper(bool useQuic
                                               , bool nodelay):
m_useQuic(useQuic),
m_nSendTimeOut(3500),
m_nRecvTimeOut(3000),
m_bConnClose(false),
m_bConnected(false)
{
    if (!useQuic) {
        nodelay = true;
    }
    m_pNetClient = CTXNetClientContextWrapper::GetInstance()->CreateSyncNetClient(!m_useQuic, nodelay);
    m_nError = 0;
    memset(m_pDescription, 0, sizeof(m_pDescription));
    memset(m_pConnectionStats, 0, sizeof(m_pConnectionStats));
}

CTXSyncNetClientWrapper::~CTXSyncNetClientWrapper()
{
    if (m_pNetClient)
    {
        std::map<std::string, std::string> mapInfo;
        m_pNetClient->GetStatisticInfo(mapInfo);
        reportStatisticInfo(mapInfo);
        
        CTXNetClientContextWrapper::GetInstance()->ReleaseSyncNetClient(m_pNetClient);
        
        m_pNetClient = NULL;
    }

    m_nError = 0;
    memset(m_pDescription, 0, sizeof(m_pDescription));
}

bool CTXSyncNetClientWrapper::isConnected()
{
    return m_bConnected && !m_bConnClose;
}

bool CTXSyncNetClientWrapper::connect(const char * host, short port, long timeout)
{
    if (m_pNetClient == NULL)
    {
        LOGE("netclient wrapper connect error: netclient == null");
        return false;
    }

//#ifdef ANDROID    //Public Reset + 移动4G 神坑，先屏蔽  可解决  但麻烦
//    TLocalIPStack ip_stack = local_ipstack_detect();
//    LOGI("netclient current network is ipv6: %d, ip: %s, port: %d", ip_stack, host, port);
//    bool nat64 = (ip_stack == ELocalIPStack_Dual) ? true : false;
//    if (nat64) {
//        const char * host2 = socket_address(host, port).v4tov6_address(nat64).ipv6();
//        LOGI("netclient current network convert to nat64 ip: %s", host2);
//        int result = m_pNetClient->Connect(host2, port, timeout);
//        if (result == 0)
//        {
//            LOGE("netclient connect success");
//            m_bConnected = (result == 0);
//            return m_bConnected;
//        }
//    }
//#endif

    LOGE("netclient connect to: %s port: %d is_tcp: %d", host, port, !m_useQuic);
    
    int result = m_pNetClient->Connect(host, port, timeout);
    if (result == 0)
    {
        LOGE("netclient connect success");
    }
    else
    {
        
        if (m_useQuic) {
            unsigned long long connection_id = 0;
            unsigned int       stream_id = 0;
            int                close_reason = 0;
            getErrorNo(connection_id, stream_id, close_reason);
            LOGE("netclient connect failed, connection_id = %llu stream_id = %u close_reason = %d", connection_id, stream_id, close_reason);
            
            m_nError = result;
            memset(m_pDescription, 0, sizeof(m_pDescription));
            sprintf(m_pDescription, "Q通道连接失败 返回码[%d] 通道ID[%llu] 流ID[%u] 错误码[%d]", result, connection_id, stream_id, close_reason);
        }
        else {
            LOGE("netclient wrapper connect error, result = %d", result);
            
            m_nError = result;
            memset(m_pDescription, 0, sizeof(m_pDescription));
            sprintf(m_pDescription, "tcp通道连接失败 错误码[%d] ", result);
        }
    }
    
    m_bConnected = (result == 0);
    
    return m_bConnected;
}

bool CTXSyncNetClientWrapper::close()
{
    LOGE("Q channel close connection");
    
    if (m_bConnClose == false)
    {
        m_bConnClose = true;
        
        m_bConnected = false;
        
        if (m_pNetClient)
        {
            m_pNetClient->CloseConn();
        }
    }
    
    return true;
}

int CTXSyncNetClientWrapper::send(const void *buffer, unsigned int size)
{
    if (m_bConnected == false || m_bConnClose == true)
    {
        LOGE("netclient wrapper end error: connected = %d connClose = %d", m_bConnected, m_bConnClose);
        return SE_BROKEN;
    }
    
    if (m_pNetClient == NULL)
    {
        LOGE("netclient wrapper send error: netclient == null");
        return SE_BROKEN;
    }
    
    int result = m_pNetClient->Write((char*)buffer, size, m_nSendTimeOut);
    if (result < 0)
    {
        if (m_useQuic) {
            unsigned long long connection_id = 0;
            unsigned int       stream_id = 0;
            int                close_reason = 0;
            getErrorNo(connection_id, stream_id, close_reason);
            
            LOGE("netclient wrapper send error, result = %d connection_id = %llu stream_id = %u close_reason = %d",
                 result, connection_id, stream_id, close_reason);
            
            m_nError = result;
            memset(m_pDescription, 0, sizeof(m_pDescription));
            sprintf(m_pDescription, "Q通道发送失败 返回码[%d] 通道ID[%llu] 流ID[%u] 错误码[%d]", result, connection_id, stream_id, close_reason);
        }
        else {
            if (result == qcloud::ERR_CONNECTION_RESET) {
                result = SE_REFUSE;
            }
            
            LOGE("netclient wrapper send error, result = %d", result);
            
            
            m_nError = result;
            memset(m_pDescription, 0, sizeof(m_pDescription));
            sprintf(m_pDescription, "tcp通道发送失败 错误码[%d] ", result);
        }
    }
    
    return result;
}

int CTXSyncNetClientWrapper::recv(void *buffer, unsigned int size)
{
    if (m_bConnected == false || m_bConnClose == true)
    {
        LOGE("netclient wrapper recv error: connected = %d connClose = %d", m_bConnected, m_bConnClose);
        return SE_BROKEN;
    }
    
    if (m_pNetClient == NULL)
    {
        LOGE("netclient wrapper recv error: netclient == null");
        return SE_BROKEN;
    }
    
    int result = m_pNetClient->Recv((char*)buffer, size, m_nRecvTimeOut);
    if (result < 0)
    {
        if (m_useQuic) {
            unsigned long long connection_id = 0;
            unsigned int       stream_id = 0;
            int                close_reason = 0;
            getErrorNo(connection_id, stream_id, close_reason);
            
            LOGE("netclient wrapper recv error, result = %d connection_id = %llu stream_id = %u close_reason = %d", result, connection_id, stream_id, close_reason);
            
            m_nError = result;
            memset(m_pDescription, 0, sizeof(m_pDescription));
            sprintf(m_pDescription, "Q通道接收失败 返回码[%d] 通道ID[%llu] 流ID[%u] 错误码[%d]", result, connection_id, stream_id, close_reason);
        }
        else {
            if (result == qcloud::ERR_CONNECTION_RESET) {
                result = SE_REFUSE;
            }
            
            LOGE("netclient wrapper recv error, result = %d", result);
            
            m_nError = result;
            memset(m_pDescription, 0, sizeof(m_pDescription));
            sprintf(m_pDescription, "tcp通道接收失败 错误码[%d] ", result);
        }
    }
    
    return result;
}

bool CTXSyncNetClientWrapper::setSendTimeout(long timeout)
{
    m_nSendTimeOut = timeout;
    
    return true;
}

bool CTXSyncNetClientWrapper::setRecvTimeout(long timeout)
{
    m_nRecvTimeOut = timeout;
    
    return true;
}

void CTXSyncNetClientWrapper::getLastError(int* error, char** description)
{
    if (error)
    {
        *error = m_nError;
    }
    if (description)
    {
        *description = m_pDescription;
    }
}

void CTXSyncNetClientWrapper::getNetSpeedAndBytesInflight(int& net_speed, int& bytes_in_flight)
{
    if (m_pNetClient)
    {
        net_speed = (int)m_pNetClient->EstimateBandWidth();
        bytes_in_flight = (int)m_pNetClient->BytesInFlight();
    }
}

void CTXSyncNetClientWrapper::getErrorNo(unsigned long long & connection_id, unsigned int & stream_id, int & close_reason)
{
    if (m_pNetClient)
    {
        m_pNetClient->GetErrorNo(connection_id, stream_id, close_reason);
    }
}

unsigned long long CTXSyncNetClientWrapper::getConnectionID()
{
    unsigned long long connection_id = 0;
    unsigned int       stream_id = 0;
    int                close_reason = 0;
    getErrorNo(connection_id, stream_id, close_reason);
    return connection_id;
}

void CTXSyncNetClientWrapper::getConnectionStats(char ** connectionStats)
{
    if (connectionStats != NULL)
    {
        if (m_pNetClient)
        {
            std::string stats = m_pNetClient->GetConnectionStats();
            memset(m_pConnectionStats, 0, sizeof(m_pConnectionStats));
            memcpy(m_pConnectionStats, stats.c_str(), stats.length());
        }
        *connectionStats = m_pConnectionStats;
    }
}

void CTXSyncNetClientWrapper::reportStatisticInfo(std::map<std::string, std::string> & mapInfo)
{
    char token[512] = {0};
    txCreateEventToken(token, sizeof(token));

    int module_id = MODULE_PUSH_SDK;

    int command_id = COMMAND_ID_QUIC;

    stExtInfo extInfo = {0};
    strncpy(extInfo.command_id_comment, COMMAND_ID_COMMENT_QUIC_40403, strlen(COMMAND_ID_COMMENT_QUIC_40403));
    extInfo.report_common = true;
    extInfo.report_status = false;
    
    txInitEvent(token, command_id, module_id, extInfo);

    for (std::map<std::string, std::string>::iterator it = mapInfo.begin(); it != mapInfo.end(); ++it)
    {
        std::string key = it->first;
        std::string val = it->second;
        if (key.length() > 0)
        {
            txSetEventValue(token, command_id, key.c_str(), val.c_str());
        }
    }

    txReportEvent(token, command_id);
}
