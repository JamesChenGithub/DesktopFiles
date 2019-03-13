//
// Created by alderzhang on 2017/8/10.
// Copyright (c) 2017 Tencent. All rights reserved.
//

#include "TXCStatusRecorder.h"
#include "txg_log.h"

TXCStatusRecorder::TXCStatusRecorder() {

}

TXCStatusRecorder::~TXCStatusRecorder() {
    TXCScopedLock scopedLock(_mutex);
    while(_records.begin() != _records.end()){
        _releaseRecord(_records.begin()->first.c_str());
    }
}

void TXCStatusRecorder::_createRecord(const char *id){
    if(_records.count(id) == 0){
        _records.insert(std::make_pair(id, new SC_KeyMap()));//创建id对应的键值表记录
    }
}

void TXCStatusRecorder::_clearRecord(const char *id){
    if(_records.count(id) != 0){
        for(SC_KeyMap::iterator iter = _records[id]->begin(); iter != _records[id]->end(); iter++){
            if(iter->second){
                if(iter->second->type == SC_VT_STR && iter->second->value.strVal){
                    delete iter->second->value.strVal;
                    iter->second->value.strVal = nullptr;
                }
                delete iter->second;
                iter->second = nullptr;
            }
        }
    }
}

void TXCStatusRecorder::_releaseRecord(const char *id){
    _clearRecord(id);//清除键值表
    if(_records.count(id) != 0){
        if(_records[id]){
            delete _records[id];//释放键值表
            _records[id] = nullptr;
        }
        _records.erase(id);//删除id对应的键值表记录
    }
}

TXCStatusRecorder* TXCStatusRecorder::s_instance = new TXCStatusRecorder();

TXCStatusRecorder *TXCStatusRecorder::sharedInstance() {
    return s_instance;
}

void TXCStatusRecorder::startRecord(const char *id) {
    if(id == nullptr) return;
    TXCScopedLock scopedLock(_mutex);
    if(_retains.count(id) == 0){
        _retains.insert(std::make_pair(id, 0));//初始化引用计数
        _createRecord(id);//创建记录
    }
    _retains[id] = _retains[id] + 1;//增加引用计数
//    LOGD("startRecord ID = %s, ratains counter = %d", id, _retains[id]);
}

void TXCStatusRecorder::stopRecord(const char *id) {
    if(id == nullptr) return;
    TXCScopedLock scopedLock(_mutex);
    if(_retains.count(id) != 0){
        _retains[id] = _retains[id] - 1;//减少引用计数
    }
    else{
        return; //异常调用（计数为0后再次调用stopRecord方法时发生）
    }
//    LOGD("stopRecord ID = %s, ratains counter = %d", id, _retains[id]);
    if(_retains[id] == 0) {//引用计数已归0
        _releaseRecord(id);//释放记录
        _retains.erase(id);//清除引用计数
    }
}

bool TXCStatusRecorder::setValue(const char *id, int key, long long value) {
    if(id == nullptr) return false;
    TXCScopedLock scopedLock(_mutex);
//    LOGD("setValue %d = %lld for ID = %s", key, value, id);
    if(_records.count(id) != 0) {
        if(_records[id]->count(key) == 0){
            _records[id]->insert(std::make_pair(key, new SC_Value));
            _records[id]->at(key)->type = SC_VT_NULL;
        }
        if(_records[id]->at(key)->type == SC_VT_STR){
            delete _records[id]->at(key)->value.strVal;
            _records[id]->at(key)->value.strVal = nullptr;
        }
        _records[id]->at(key)->type = SC_VT_INT;
        _records[id]->at(key)->value.intVal = value;
        return true;
    }
    return false;
}

bool TXCStatusRecorder::setValue(const char *id, int key, double value) {
    if(id == nullptr) return false;
    TXCScopedLock scopedLock(_mutex);
//    LOGD("setValue %d = %f for ID = %s", key, value, id);
    if(_records.count(id) != 0) {
        if(_records[id]->count(key) == 0){
            _records[id]->insert(std::make_pair(key, new SC_Value));
            _records[id]->at(key)->type = SC_VT_NULL;
        }
        if(_records[id]->at(key)->type == SC_VT_STR){
            delete _records[id]->at(key)->value.strVal;
            _records[id]->at(key)->value.strVal = nullptr;
        }
        _records[id]->at(key)->type = SC_VT_DOUBLE;
        _records[id]->at(key)->value.doubleVal = value;
        return true;
    }
    return false;
}

bool TXCStatusRecorder::setValue(const char *id, int key, const char *value) {
    if(id == nullptr) return false;
    TXCScopedLock scopedLock(_mutex);
//    LOGD("setValue %d = %s for ID = %s", key, value, id);
    if(_records.count(id) != 0) {
        if(_records[id]->count(key) == 0){
            _records[id]->insert(std::make_pair(key, new SC_Value));
            _records[id]->at(key)->type = SC_VT_NULL;
        }
        if(_records[id]->at(key)->type == SC_VT_STR){
            delete _records[id]->at(key)->value.strVal;
            _records[id]->at(key)->value.strVal = nullptr;
        }
        _records[id]->at(key)->type = SC_VT_STR;
        _records[id]->at(key)->value.strVal = new char[strlen(value) + 1];
        strcpy(_records[id]->at(key)->value.strVal, value);
        return true;
    }
    return false;
}

long long TXCStatusRecorder::getIntValue(const char *id, int key){
    if(id == nullptr) return 0;
    TXCScopedLock scopedLock(_mutex);
    if(_records.count(id) != 0) {
        if(_records[id]->count(key) != 0){
            if(_records[id]->at(key)->type == SC_VT_INT){
                return _records[id]->at(key)->value.intVal;
            }
        }
    }
    return 0;
}

double TXCStatusRecorder::getDoubleValue(const char *id, int key){
    if(id == nullptr) return 0;
    TXCScopedLock scopedLock(_mutex);
    if(_records.count(id) != 0) {
        if(_records[id]->count(key) != 0){
            if(_records[id]->at(key)->type == SC_VT_DOUBLE){
                return _records[id]->at(key)->value.doubleVal;
            }
        }
    }
    return 0.0;
}

const char *TXCStatusRecorder::getStrValue(const char *id, int key){
    if(id == nullptr) return "";
    TXCScopedLock scopedLock(_mutex);
    if(_records.count(id) != 0) {
        if(_records[id]->count(key) != 0){
            if(_records[id]->at(key)->type == SC_VT_STR){
                return _records[id]->at(key)->value.strVal;
            }
        }
    }
    return "";
}
