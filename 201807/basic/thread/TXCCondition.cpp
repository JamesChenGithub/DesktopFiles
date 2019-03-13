//
// Created by alderzhang on 2017/6/23.
//

#include "TXCCondition.h"

TXCCondition::TXCCondition()
		: _condition(), _mutex() {
	_anyway_notify.store(false);
}

TXCCondition::~TXCCondition() {
}

void TXCCondition::wait(TXCScopedLock &lock) {
	ASSERT(lock.islocked());

	if (!_anyway_notify.exchange(false)) {
		_condition.wait(lock);
	}
}

bool TXCCondition::wait(TXCScopedLock &lock, long millisecond) {
	ASSERT(lock.islocked());

	std::cv_status ret = std::cv_status::timeout;

	if (!_anyway_notify.exchange(false)) {
		ret = _condition.wait_for(lock, std::chrono::milliseconds(millisecond));
	}

	return ret == std::cv_status::no_timeout;
}

void TXCCondition::wait() {
	TXCScopedLock scopedLock(_mutex);
	wait(scopedLock);
}

int TXCCondition::wait(long millisecond) {
	TXCScopedLock scopedLock(_mutex);
	return wait(scopedLock, millisecond);
}

void TXCCondition::notifyOne() {
	_condition.notify_one();
}

void TXCCondition::notifyOne(TXCScopedLock &lock) {
	ASSERT(lock.islocked());
	notifyOne();
}

void TXCCondition::notifyAll(bool anywaynotify) {
	if (anywaynotify) _anyway_notify.store(true);

	_condition.notify_all();
}

void TXCCondition::notifyAll(TXCScopedLock &lock, bool anywaynotify) {
	ASSERT(lock.islocked());
	notifyAll(anywaynotify);
}

void TXCCondition::cancelAnyWayNotify() {
	_anyway_notify.store(false);
}
