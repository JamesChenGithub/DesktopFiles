// Tencent is pleased to support the open source community by making Mars available.
// Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

// Licensed under the MIT License (the "License"); you may not use this file except in 
// compliance with the License. You may obtain a copy of the License at
// http://opensource.org/licenses/MIT

// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied. See the License for the specific language governing permissions and
// limitations under the License.

//
//  log_crypt.h
//  mars-ext
//
//  Created by yanguoyue on 16/6/14.
//  Copyright © 2016年 Tencent. All rights reserved.
//

#ifndef TXCLOGCRYPT_H
#define TXCLOGCRYPT_H

#include <stdint.h>
#include <string>

class TXCLogCrypt {
public:
    TXCLogCrypt(): seq_(0) {}
    virtual ~TXCLogCrypt() {}
    
private:
    TXCLogCrypt(const TXCLogCrypt&);
    TXCLogCrypt& operator=(const TXCLogCrypt&);

public:
    uint32_t GetHeaderLen();
    uint32_t GetTailerLen();
    
    void SetHeaderInfo(char* _data, bool _is_async);
    void SetTailerInfo(char* _data);
    
    uint32_t GetLogLen(const char* const _data, size_t _len);
    void UpdateLogLen(char* _data, uint32_t _add_len);
    
    bool GetLogHour(const char* const _data, size_t _len, int& _begin_hour, int& _end_hour);
    void UpdateLogHour(char* _data);
    
    bool GetPeriodLogs(const char* const _log_path, int _begin_hour, int _end_hour, unsigned long& _begin_pos, unsigned long& _end_pos, std::string& _err_msg);
    
    void CryptSyncLog(const char* const _log_data, size_t _input_len, char* _output, size_t& _output_len);
    void CryptAsyncLog(const char* const _log_data, size_t _input_len, char* _output, size_t& _output_len);
    
    bool Fix(char* _data, size_t _data_len, bool& _is_async, uint32_t& _raw_log_len);
    
private:
    uint16_t seq_;

};



#endif /* TXCLOGCRYPT_H */
