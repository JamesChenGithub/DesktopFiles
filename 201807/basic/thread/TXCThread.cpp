//
// Created by alderzhang on 2017/5/23.
//

#include "TXCThread.h"

TXCThread::TXCRunnableReference::TXCRunnableReference(TXCRunnable *ntarget)
		: target(ntarget), count(0), thread(nullptr), isJoined(false), isEnded(true), afterTime(LONG_MAX),
		  periodicTime(LONG_MAX), isCancelDelayStart(false), condTime(), spLock() {
	memset(thread_name, 0, sizeof(thread_name));
}

TXCThread::TXCRunnableReference::~TXCRunnableReference() {
	delete target;
	delete thread;
	ASSERT(0 == count);
	ASSERT(isEnded);
}

void TXCThread::TXCRunnableReference::AddRef() {
	count++;
}

void TXCThread::TXCRunnableReference::RemoveRef(TXCScopedSpinLock &lock) {
	ASSERT(0 < count);
	ASSERT(lock.islocked());

	bool willdel = false;
	count--;

	if (0 == count) willdel = true;

	lock.unlock();

	if (willdel) delete this;
}

TXCThread::TXCThread(const char *thread_name)
		: _runable_ref(NULL) {
	_init(new TXCRunnableReference(NULL), thread_name);
}

TXCThread::~TXCThread() {
	TXCScopedSpinLock lock(_runable_ref->spLock);
	_runable_ref->RemoveRef(lock);
}

bool TXCThread::start(bool *newone) {
	return _start(newone, NULL);
}

bool TXCThread::start_after(long after) {
	TXCScopedSpinLock lock(_runable_ref->spLock);

	if (isruning())return false;

	ASSERT(_runable_ref->target);
	_runable_ref->condTime.cancelAnyWayNotify();
	_runable_ref->isCancelDelayStart = false;
	_runable_ref->isEnded = false;
	_runable_ref->afterTime = after;
	_runable_ref->AddRef();

	_runable_ref->thread = new std::thread(_StartRoutineAfter, _runable_ref);

//	if (0 != ret) {
//		_runable_ref->_isEnded = true;
//		_runable_ref->afterTime = LONG_MAX;
//		_runable_ref->RemoveRef(lock);
//	}

	return true;
}

void TXCThread::cancel_after() {
	TXCScopedSpinLock lock(_runable_ref->spLock);

	if (!isruning()) return;

	_runable_ref->isCancelDelayStart = true;
	_runable_ref->condTime.notifyAll(true);
}

bool TXCThread::start_periodic(long after, long periodic) {
	TXCScopedSpinLock lock(_runable_ref->spLock);

	if (isruning()) return 0;

	ASSERT(_runable_ref->target);
	_runable_ref->condTime.cancelAnyWayNotify();
	_runable_ref->isCancelDelayStart = false;
	_runable_ref->isEnded = false;
	_runable_ref->afterTime = after;
	_runable_ref->periodicTime = periodic;
	_runable_ref->AddRef();

	_runable_ref->thread = new std::thread(_StartRoutinePeriodic, _runable_ref);

//	if (0 != ret) {
//		_runable_ref->_isEnded = true;
//		_runable_ref->afterTime = LONG_MAX;
//		_runable_ref->_periodicTime = LONG_MAX;
//		_runable_ref->RemoveRef(lock);
//	}

	return true;
}

void TXCThread::cancel_periodic() {
	TXCScopedSpinLock lock(_runable_ref->spLock);

	if (!isruning()) return;

	_runable_ref->isCancelDelayStart = true;
	_runable_ref->condTime.notifyAll(true);
}

bool TXCThread::join() const {
	TXCScopedSpinLock lock(_runable_ref->spLock);
	ASSERT(!_runable_ref->isJoined);

	if (tid() == std::this_thread::get_id()) return false;

	if (isruning()) {
		_runable_ref->isJoined = true;
		lock.unlock();
		if (!_runable_ref->thread->joinable()) return false;
		_runable_ref->thread->join();
	}

	return true;
}

void TXCThread::outside_join() const {
	TXCScopedSpinLock lock(_runable_ref->spLock);
	ASSERT(!_runable_ref->isJoined);
	ASSERT(!isruning());
	if (_runable_ref->isJoined || isruning()) return;

	_runable_ref->isJoined = true;
}

std::thread::id TXCThread::tid() const {
	return _runable_ref->thread->get_id();
}

bool TXCThread::isruning() const {
	return !_runable_ref->isEnded;
}

const char *TXCThread::thread_name() const {
	return _runable_ref->thread_name;
}

void TXCThread::_Init(void *arg) {
	volatile TXCRunnableReference *runableref = static_cast<TXCRunnableReference *>(arg);
	TXCScopedSpinLock lock((const_cast<TXCRunnableReference *>(runableref))->spLock);
	ASSERT(runableref != 0);
	ASSERT(runableref->target != 0);
	ASSERT(!runableref->isInThread);

	if (0 < strnlen((const char *) runableref->thread_name, sizeof(runableref->thread_name))) {
#ifdef __APPLE__
		pthread_setname_np((const char *) runableref->thread_name);
#elif defined(ANDROID)
		pthread_setname_np(pthread_self(), (const char*)runableref->thread_name);
#else

#endif
	}
}

void TXCThread::_Cleanup(void *arg) {
	volatile TXCRunnableReference *runableref = static_cast<TXCRunnableReference *>(arg);
	TXCScopedSpinLock lock((const_cast<TXCRunnableReference *>(runableref))->spLock);

	ASSERT(runableref != 0);
	ASSERT(runableref->target != 0);
	ASSERT(runableref->thread != nullptr);
	ASSERT(runableref->isInThread);

	runableref->isEnded = true;

	if (!runableref->isJoined) runableref->thread->detach();

//	delete runableref->thread;

	runableref->isJoined = false;
	(const_cast<TXCRunnableReference *>(runableref))->RemoveRef(lock);
}

void *TXCThread::_StartRoutine(void *arg) {
	_Init(arg);

	volatile TXCRunnableReference *runableref = static_cast<TXCRunnableReference *>(arg);
	runableref->target->run();

	_Cleanup(arg);
	return 0;
}

void *TXCThread::_StartRoutineAfter(void *arg) {
	_Init(arg);

	volatile TXCRunnableReference *runableref = static_cast<TXCRunnableReference *>(arg);
	if (!runableref->isCancelDelayStart) {
		(const_cast<TXCRunnableReference *>(runableref))->condTime.wait(runableref->afterTime);

		if (!runableref->isCancelDelayStart)
			runableref->target->run();
	}

	_Cleanup(arg);
	return 0;
}

void *TXCThread::_StartRoutinePeriodic(void *arg) {
	_Init(arg);

	volatile TXCRunnableReference *runableref = static_cast<TXCRunnableReference *>(arg);
	if (!runableref->isCancelDelayStart) {
		(const_cast<TXCRunnableReference *>(runableref))->condTime.wait(runableref->afterTime);

		while (!runableref->isCancelDelayStart) {
			runableref->target->run();

			if (!runableref->isCancelDelayStart)
				(const_cast<TXCRunnableReference *>(runableref))->condTime.wait(runableref->periodicTime);
		}
	}

	_Cleanup(arg);
	return 0;
}

void TXCThread::_init(TXCRunnableReference *new_runable_ref, const char *thread_name) {
	_runable_ref = new_runable_ref;
	TXCScopedSpinLock lock(_runable_ref->spLock);
	_runable_ref->AddRef();

	if (thread_name) strncpy(_runable_ref->thread_name, thread_name, sizeof(_runable_ref->thread_name));
}

bool TXCThread::_start(bool *newone, TXCRunnable *new_target) {
	TXCScopedSpinLock lock(_runable_ref->spLock);

	if (newone) *newone = false;

	if (isruning())return 0;

	if (new_target == NULL) {
		ASSERT(_runable_ref->target);
	} else {
		delete _runable_ref->target;
		_runable_ref->target = new_target;
	}

	_runable_ref->isEnded = false;
	_runable_ref->AddRef();

	_runable_ref->thread = new std::thread(_StartRoutine, _runable_ref);

	if (newone) *newone = true;

//	if (0 != ret) {
//		_runable_ref->_isEnded = true;
//		_runable_ref->RemoveRef(lock);
//	}

	return true;
}

bool operator==(const TXCThread &lhs, const TXCThread &rhs) {
	return lhs.tid() == rhs.tid();
}

bool operator!=(const TXCThread &lhs, const TXCThread &rhs) {
	return lhs.tid() != rhs.tid();
}
