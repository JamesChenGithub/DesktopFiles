//
//  AsynNetClientWrapper.cpp
//  TXMBasic
//
//  Created by shockcao on 2017/8/18.
//  Copyright © 2017年 Tencent. All rights reserved.
//

#include "AsyncNetClientWrapper.h"

#include "NetClientContextWrapper.h"

CTXAsyncNetClientWrapper::CTXAsyncNetClientWrapper(bool useQuic
                                                , qcloud::AsyncNetClientCallBack* callback
                                                , bool nodelay)
    : m_useQuic(useQuic)
{
    m_pNetClient = CTXNetClientContextWrapper::GetInstance()->CreateAsyncNetClient(callback, !m_useQuic, nodelay);
}

CTXAsyncNetClientWrapper::~CTXAsyncNetClientWrapper() {
    if (m_pNetClient)
    {
        CTXNetClientContextWrapper::GetInstance()->ReleaseAsyncNetClient(m_pNetClient);
        m_pNetClient = NULL;
    }
    
    
}

void CTXAsyncNetClientWrapper::Connect(const char* ip, uint16_t port) {
    if (!m_pNetClient) {
        return;
    }
    
    m_pNetClient->Connect(ip, port);
}

void CTXAsyncNetClientWrapper::ComplexConnect(const std::vector<std::string>& ipport_list) {
    if (!m_pNetClient) {
        return;
    }
    
    m_pNetClient->ComplexConnect(ipport_list);
}

void CTXAsyncNetClientWrapper::TriggerWrite() {
    if (!m_pNetClient) {
        return;
    }
    
    m_pNetClient->TriggerWrite();
}

void CTXAsyncNetClientWrapper::CloseConn() {
    if (!m_pNetClient) {
        return;
    }
    
    m_pNetClient->CloseConn();
}
