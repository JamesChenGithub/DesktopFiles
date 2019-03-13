// Tencent is pleased to support the open source community by making Mars available.
// Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

// Licensed under the MIT License (the "License"); you may not use this file except in 
// compliance with the License. You may obtain a copy of the License at
// http://opensource.org/licenses/MIT

// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied. See the License for the specific language governing permissions and
// limitations under the License.



#include "TXCTickCount.h"
#include "txg_time_util.h"

TXCTickCountDiff::TXCTickCountDiff(int64_t diff)
		: _tickCountDiff(diff) {

}

TXCTickCountDiff::operator int64_t() const {
	return _tickCountDiff;
}

TXCTickCountDiff &TXCTickCountDiff::operator+=(int64_t factor) {
	_tickCountDiff += factor;
	return *this;
}

TXCTickCountDiff &TXCTickCountDiff::operator-=(int64_t factor) {
	_tickCountDiff -= factor;
	return *this;
}

TXCTickCountDiff &TXCTickCountDiff::operator*=(int64_t factor) {
	_tickCountDiff *= factor;
	return *this;
}

static uint64_t sg_tick_start = ::txf_gettickcount();
static const uint64_t sg_tick_init = 2000000000;

TXCTickCount::TXCTickCount(bool now)
		: _tickCount(0) {
	if (now) getTickCount();
}

TXCTickCountDiff TXCTickCount::operator-(const TXCTickCount &tc) const {
	return TXCTickCountDiff(_tickCount - tc._tickCount);
}

TXCTickCount TXCTickCount::operator+(const TXCTickCountDiff &tc_diff) const {
	return TXCTickCount(*this).operator+=(tc_diff);
}

TXCTickCount TXCTickCount::operator-(const TXCTickCountDiff &tc_diff) const {
	return TXCTickCount(*this).operator-=(tc_diff);
}

TXCTickCount &TXCTickCount::operator+=(const TXCTickCountDiff &tc_diff) {
	_tickCount += (int64_t) tc_diff;
	return *this;
}

TXCTickCount &TXCTickCount::operator-=(const TXCTickCountDiff &tc_diff) {
	_tickCount -= (int64_t) tc_diff;
	return *this;
}

uint64_t TXCTickCount::get() const {
	return _tickCount;
}

TXCTickCount &TXCTickCount::getTickCount() {
	_tickCount = sg_tick_init + ::txf_gettickcount() - sg_tick_start;
	return *this;
}

TXCTickCountDiff TXCTickCount::getTickSpan() const {
	return TXCTickCount(true) - (*this);
}

bool TXCTickCount::operator<(const TXCTickCount &tc) const {
	return _tickCount < tc._tickCount;
}

bool TXCTickCount::operator<=(const TXCTickCount &tc) const {
	return _tickCount <= tc._tickCount;
}

bool TXCTickCount::operator==(const TXCTickCount &tc) const {
	return _tickCount == tc._tickCount;
}

bool TXCTickCount::operator!=(const TXCTickCount &tc) const {
	return _tickCount != tc._tickCount;
}

bool TXCTickCount::operator>(const TXCTickCount &tc) const {
	return _tickCount > tc._tickCount;
}

bool TXCTickCount::operator>=(const TXCTickCount &tc) const {
	return _tickCount >= tc._tickCount;
}

bool TXCTickCount::isValid() const{
	return _tickCount != 0;
}
