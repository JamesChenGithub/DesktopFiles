// Tencent is pleased to support the open source community by making Mars available.
// Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

// Licensed under the MIT License (the "License"); you may not use this file except in 
// compliance with the License. You may obtain a copy of the License at
// http://opensource.org/licenses/MIT

// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied. See the License for the specific language governing permissions and
// limitations under the License.

/*
 * log_buffer.h
 *
 *  Created on: 2015-7-28
 *      Author: yanguoyue
 */

#ifndef TXCLOGBUFFER_H
#define TXCLOGBUFFER_H

#include <zlib.h>
#include <string>
#include <stdint.h>

#include "TXCPtrBuffer.h"
#include "TXCAutoBuffer.h"

class TXCLogCrypt;

class TXCLogBuffer {
public:
    TXCLogBuffer(void* _pbuffer, size_t _len, bool _is_compress);
    ~TXCLogBuffer();
    
public:
    static bool GetPeriodLogs(const char* _log_path, int _begin_hour, int _end_hour, unsigned long& _begin_pos, unsigned long& _end_pos, std::string& _err_msg);
    static bool Write(const void* _data, size_t _inputlen, void* _output, size_t& _len);

public:
    TXCPtrBuffer& GetData();
    

    void Flush(TXCAutoBuffer& _buff);
    bool Write(const void* _data, size_t _length);

private:
    
    bool __Reset();
    void __Flush();
    void __Clear();
    
    void __Fix();

private:
    TXCPtrBuffer buff_;
    bool is_compress_;
    z_stream cstream_;
    
    static class TXCLogCrypt* s_log_crypt;

};


#endif /* TXCLOGBUFFER_H */