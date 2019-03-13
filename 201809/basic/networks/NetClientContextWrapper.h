#ifndef __NETCLIENTCONTEXTWRAPPER_H__
#define __NETCLIENTCONTEXTWRAPPER_H__


#include "qcloud_live_net_client.h"
#include "qcloud_live_net_client_context.h"


class CTXNetClientContextWrapper
{
protected:
    CTXNetClientContextWrapper();

    
public:
    static CTXNetClientContextWrapper * GetInstance();
    
    virtual ~CTXNetClientContextWrapper();

    qcloud::QcloudLiveSyncNetClient* CreateSyncNetClient(bool use_tcp, bool nodelay = false);
    
    void ReleaseSyncNetClient(qcloud::QcloudLiveSyncNetClient* client);
    
    qcloud::QcloudLiveAsyncNetClient* CreateAsyncNetClient(qcloud::AsyncNetClientCallBack* callback,
                                                           bool is_tcp = false, bool nodelay = false);
    
    void ReleaseAsyncNetClient(qcloud::QcloudLiveAsyncNetClient* client);
    
private:
    qcloud::QcloudLiveNetClientContext*         m_pQuicClientContext;
};


#endif /* __NETCLIENTCONTEXTWRAPPER_H__ */
