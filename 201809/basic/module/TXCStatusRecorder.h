//
// Created by alderzhang on 2017/8/10.
// Copyright (c) 2017 Tencent. All rights reserved.
//

#ifndef TXMBASIC_TXCSTATUSCORE_H
#define TXMBASIC_TXCSTATUSCORE_H

#include <string>
#include <map>
#include "TXCLock.h"

class TXCStatusRecorder {
private:
    TXCStatusRecorder();
    ~TXCStatusRecorder();
    TXCStatusRecorder(const TXCStatusRecorder&);
    TXCStatusRecorder& operator=(const TXCStatusRecorder&);
    
    typedef enum{
        SC_VT_NULL,
        SC_VT_INT,
        SC_VT_DOUBLE,
        SC_VT_STR,
    } SC_ValueType;
    
    typedef struct{
        SC_ValueType type;
        union{
            long long intVal;
            double doubleVal;
            char *strVal;
        } value;
    } SC_Value;
    
    typedef std::map<int, SC_Value*> SC_KeyMap;
    
    typedef std::map<const std::string, SC_KeyMap*> SC_RecordMap;

public:
    static TXCStatusRecorder *sharedInstance();

    void startRecord(const char *id);

    void stopRecord(const char *id);

    bool setValue(const char *id, int key, long long value);

    bool setValue(const char *id, int key, double value);

    bool setValue(const char *id, int key, const char *value);
    
    long long getIntValue(const char *id, int key);
    
    double getDoubleValue(const char *id, int key);

    const char *getStrValue(const char *id, int key);

private:
    void _createRecord(const char *id);
    void _clearRecord(const char *id);
    void _releaseRecord(const char *id);
    
    std::map<const std::string, int> _retains;

    SC_RecordMap _records;

    TXCMutex _mutex;
    static TXCStatusRecorder* s_instance;
};


#endif //TXMBASIC_TXCSTATUSCORE_H
