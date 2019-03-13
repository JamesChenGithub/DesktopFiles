//
//  AsynNetClientWrapper.hpp
//  TXMBasic
//
//  Created by shockcao on 2017/8/18.
//  Copyright © 2017年 Tencent. All rights reserved.
//

#ifndef AsynNetClientWrapper_h
#define AsynNetClientWrapper_h

#include "qcloud_live_net.h"
#include "qcloud_live_net_client.h"

class CTXAsyncNetClientWrapper {
    
public:
    CTXAsyncNetClientWrapper(bool useQuic, qcloud::AsyncNetClientCallBack* callback, bool nodelay = false);
    
    virtual ~CTXAsyncNetClientWrapper();
    
    void Connect(const char* ip, uint16_t port);
    
    void ComplexConnect(const std::vector<std::string>& ipport_list);
    
    void TriggerWrite();
    
    void CloseConn();
    
private:
    
    bool                                               m_useQuic;
    
    qcloud::QcloudLiveAsyncNetClient*                  m_pNetClient;
};


#endif /* AsynNetClientWrapper_hpp */
