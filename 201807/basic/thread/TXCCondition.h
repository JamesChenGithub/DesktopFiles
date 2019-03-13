//
// Created by alderzhang on 2017/6/23.
//

#ifndef TXCCONDITION_H
#define TXCCONDITION_H

#include <condition_variable>
#include <atomic>
#include "TXCLock.h"

class TXCCondition {
public:
	TXCCondition();

	~TXCCondition();

	void wait(TXCScopedLock& lock);

	bool wait(TXCScopedLock& lock, long millisecond);

	void wait();

	int wait(long millisecond);

	void notifyOne();

	void notifyOne(TXCScopedLock& lock);

	void notifyAll(bool anywaynotify = false);

	void notifyAll(TXCScopedLock& lock, bool anywaynotify = false);

	void cancelAnyWayNotify();

private:
	TXCCondition(const TXCCondition&);
	TXCCondition& operator=(const TXCCondition&);

private:
	std::condition_variable_any _condition;
	TXCMutex _mutex;
	std::atomic_bool _anyway_notify;
};


#endif //TXCCONDITION_H
