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
//  ptrbuffer.cc
//
//  Created by yerungui on 13-4-4.
//

#include "TXCPtrBuffer.h"

#include <string.h>

#include "txg_assert.h"

const TXCPtrBuffer KNullPtrBuffer(0, 0, 0);

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

TXCPtrBuffer::TXCPtrBuffer(void* _ptr, size_t _len, size_t _maxlen)
    : parray_((unsigned char*)_ptr)
    , pos_(0)
    , length_(_len)
    , max_length_(_maxlen) {
    ASSERT(length_ <= max_length_);
}

TXCPtrBuffer::TXCPtrBuffer(void* _ptr, size_t _len)
    : parray_((unsigned char*)_ptr)
    , pos_(0)
    , length_(_len)
    , max_length_(_len) {
    ASSERT(length_ <= max_length_);
}

TXCPtrBuffer::TXCPtrBuffer() {
    Reset();
}

TXCPtrBuffer::~TXCPtrBuffer() {
}

void TXCPtrBuffer::Write(const void* _pBuffer, size_t _nLen) {
    Write(_pBuffer, _nLen, Pos());
    Seek(_nLen, kSeekCur);
}

void TXCPtrBuffer::Write(const void* _pBuffer, size_t _nLen, off_t _nPos) {
    ASSERT(NULL != _pBuffer);
    ASSERT(0 <= _nPos);
    ASSERT((unsigned int) _nPos <= Length());
    size_t copylen = min(_nLen, max_length_ - _nPos);
    length_ = max(length_, copylen + _nPos);
    memcpy((unsigned char*)Ptr() + _nPos, _pBuffer, copylen);
}

size_t TXCPtrBuffer::Read(void* _pBuffer, size_t _nLen) {
    size_t nRead = Read(_pBuffer, _nLen, Pos());
    Seek(nRead, kSeekCur);
    return nRead;
}

size_t TXCPtrBuffer::Read(void* _pBuffer, size_t _nLen, off_t _nPos) const {
    ASSERT(NULL != _pBuffer);
    ASSERT(0 <= _nPos);
    ASSERT((unsigned int) _nPos < Length());

    size_t nRead = Length() - _nPos;
    nRead = min(nRead, _nLen);
    memcpy(_pBuffer, PosPtr(), nRead);
    return nRead;
}

void  TXCPtrBuffer::Seek(off_t _nOffset,  TSeek _eOrigin) {
    switch (_eOrigin) {
    case kSeekStart:
        pos_ = _nOffset;
        break;

    case kSeekCur:
        pos_ += _nOffset;
        break;

    case kSeekEnd:
        pos_ = length_ + _nOffset;
        break;

    default:
        ASSERT(false);
        break;
    }

    if (pos_ < 0)
        pos_ = 0;

    if ((unsigned int)pos_ > length_)
        pos_ = length_;
}

void TXCPtrBuffer::Length(off_t _nPos, size_t _nLenght) {
    ASSERT(0 <= _nPos);
    ASSERT((size_t) _nPos <= _nLenght);
    ASSERT(_nLenght <= MaxLength());

    length_ = max_length_ < _nLenght ? max_length_ : _nLenght;
    Seek(_nPos, kSeekStart);
}

void*  TXCPtrBuffer::Ptr() {
    return parray_;
}

const void*  TXCPtrBuffer::Ptr() const {
    return parray_;
}

void* TXCPtrBuffer::PosPtr() {
    return ((unsigned char*)Ptr()) + Pos();
}

const void* TXCPtrBuffer::PosPtr() const {
    return ((unsigned char*)Ptr()) + Pos();
}

off_t TXCPtrBuffer::Pos() const {
    return pos_;
}

size_t TXCPtrBuffer::PosLength() const {
    return length_ - pos_;
}

size_t TXCPtrBuffer::Length() const {
    return length_;
}

size_t TXCPtrBuffer::MaxLength() const {
    return max_length_;
}

void TXCPtrBuffer::Attach(void* _pBuffer, size_t _nLen, size_t _maxlen) {
    Reset();
    parray_ = (unsigned char*)_pBuffer;
    length_ = _nLen;
    max_length_ = _maxlen;
}

void TXCPtrBuffer::Attach(void* _pBuffer, size_t _nLen) {
    Attach(_pBuffer, _nLen, _nLen);
}

void TXCPtrBuffer::Reset() {
    parray_ = NULL;
    pos_ = 0;
    length_ = 0;
    max_length_ = 0;
}

