//
// Created by alderzhang on 2017/5/24.
//

#include <functional>
#include "TXCAbstractThread.h"

TXCAbstractThread::TXCAbstractThread()
		: _exitPending(false), _running(false), _thread(NULL){

}

TXCAbstractThread::~TXCAbstractThread() {
	TXCScopedLock lock(_mutex);
	if(_thread != nullptr){
		delete _thread;
	}
}

int TXCAbstractThread::run(const char *name, size_t stack) {
	TXCScopedLock lock(_mutex);
	if(_running){
		return -1;
	}

	_exitPending = false;
	_running = true;

	_thread = new TXCThread(std::bind(_threadLoop, this), name);
//	_thread->stack_size(stack);

    _thread->start();
	return 0;
}

void TXCAbstractThread::requestExit() {
	TXCScopedLock lock(_mutex);
	_exitPending = true;
}

bool TXCAbstractThread::isRunning() const {
	TXCScopedLock lock(_mutex);
	return _running;
}

bool TXCAbstractThread::exitPending() const {
	TXCScopedLock lock(_mutex);
	return _exitPending;
}

int TXCAbstractThread::readyToRun() {
	return 0;
}

int TXCAbstractThread::finishRun() {
	return 0;
}

#if defined(ANDROID)
static inline void *objc_autoreleasePoolPush() { return 0; };
static inline void objc_autoreleasePoolPop(void*) {}
#else
extern "C" void *objc_autoreleasePoolPush();
extern "C" void objc_autoreleasePoolPop(void*);
#endif

void *TXCAbstractThread::_threadLoop(void *user) {
	TXCAbstractThread* const self = static_cast<TXCAbstractThread*>(user);
	bool first = true;
	do {
		void *context = objc_autoreleasePoolPush();
		bool result = false;
		if (first) {
			first = false;
			self->readyToRun();

			if (!self->exitPending()) {
				result = self->threadLoop();
			}
		} else {
			result = self->threadLoop();
		}
		objc_autoreleasePoolPop(context);
		{
			TXCScopedLock lock(self->_mutex);
			if (!result || self->_exitPending) {
				self->_exitPending = true;
				self->_running = false;
				break;
			}
		}
	} while(self->_running);

	self->finishRun();
	delete self;
	return nullptr;
}
