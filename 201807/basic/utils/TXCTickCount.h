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
 * tickcount.h
 *
 *  Created on: 2014-11-6
 *      Author: yerungui
 */

#ifndef TXCTICKCOUNT_H
#define TXCTICKCOUNT_H

#include <stdint.h>

class TXCTickCountDiff {
public:
	TXCTickCountDiff(int64_t diff);

	operator int64_t() const;

	TXCTickCountDiff &operator+=(int64_t factor);

	TXCTickCountDiff &operator-=(int64_t factor);

	TXCTickCountDiff &operator*=(int64_t factor);

public:
	// TXCTickCountDiff(const TXCTickCountDiff&);
	// TXCTickCountDiff& operator=(const TXCTickCountDiff&);

private:
	int64_t _tickCountDiff;
};

class TXCTickCount {
public:
	TXCTickCount(bool now = false);

	TXCTickCountDiff operator-(const TXCTickCount &tc) const;

	TXCTickCount operator+(const TXCTickCountDiff &tc_diff) const;

	TXCTickCount operator-(const TXCTickCountDiff &tc_diff) const;

	TXCTickCount &operator+=(const TXCTickCountDiff &tc_diff);

	TXCTickCount &operator-=(const TXCTickCountDiff &tc_diff);

	uint64_t get() const;

	TXCTickCount &getTickCount();

	TXCTickCountDiff getTickSpan() const;

	bool operator<(const TXCTickCount &tc) const;

	bool operator<=(const TXCTickCount &tc) const;

	bool operator==(const TXCTickCount &tc) const;

	bool operator!=(const TXCTickCount &tc) const;

	bool operator>(const TXCTickCount &tc) const;

	bool operator>=(const TXCTickCount &tc) const;

	bool isValid() const;

public:
	// TXCTickCount(const TXCTickCount&);
	// TXCTickCount& operator=(const TXCTickCount&);

private:
	uint64_t _tickCount;
};


#endif /* TXCTICKCOUNT_H */
