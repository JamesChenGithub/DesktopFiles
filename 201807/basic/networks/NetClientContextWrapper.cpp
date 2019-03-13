#include "NetClientContextWrapper.h"

#include "txg_log.h"
#include "TXCLock.h"


#undef _MODULE_
#define _MODULE_ "NetClientContextWrapper"


bool LogMessageHandlerFunction(int severity, const char* file, int line, size_t message_start, const std::string& message)
{
    const int maxLength = 500;
    if (message.size() > maxLength)
    {
        LOGE("【QUIC-LOG】%s", message.substr(0, maxLength).c_str());
    }
    else
    {
        LOGE("【QUIC-LOG】%s", message.c_str());
    }
    
    return true;
}

CTXNetClientContextWrapper::CTXNetClientContextWrapper()
{
    qcloud::QcloudLiveNetClientContext::SetLogLevel(qcloud::QLOG_INFO);
    qcloud::LogMessageHandlerFunction f = LogMessageHandlerFunction;
    qcloud::QcloudLiveNetClientContext::SetLogMessageHandler(f);
    
    qcloud::QcloudLiveNetClientContext::Params params;
    params.quic_connection_options.push_back(qcloud::kTLPR);
    m_pQuicClientContext = new qcloud::QcloudLiveNetClientContext(params);
    m_pQuicClientContext->Initialize();
}

CTXNetClientContextWrapper::~CTXNetClientContextWrapper()
{
    delete m_pQuicClientContext;
    m_pQuicClientContext = NULL;
}

static TXCMutex* s_oMutex = new TXCMutex();

CTXNetClientContextWrapper * CTXNetClientContextWrapper::GetInstance()
{
    TXCScopedLock lock(*s_oMutex);
    static CTXNetClientContextWrapper* pInstance = NULL;
    if (pInstance == NULL)
    {
        pInstance = new CTXNetClientContextWrapper();
    }
    return pInstance;
}

qcloud::QcloudLiveSyncNetClient* CTXNetClientContextWrapper::CreateSyncNetClient(bool use_tcp, bool nodelay)
{
    return m_pQuicClientContext->CreateSyncNetClient(use_tcp, nodelay);
}

void CTXNetClientContextWrapper::ReleaseSyncNetClient(qcloud::QcloudLiveSyncNetClient* client)
{
    m_pQuicClientContext->ReleaseSyncNetClient(client);
}

qcloud::QcloudLiveAsyncNetClient* CTXNetClientContextWrapper::CreateAsyncNetClient(qcloud::AsyncNetClientCallBack* callback,
                                                       bool is_tcp, bool nodelay) {
    return m_pQuicClientContext->CreateAsyncNetClient(callback, is_tcp, nodelay);
}

void CTXNetClientContextWrapper::ReleaseAsyncNetClient(qcloud::QcloudLiveAsyncNetClient* client) {
    m_pQuicClientContext->ReleaseAsyncNetClient(client);
}
