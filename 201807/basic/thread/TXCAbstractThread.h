//
// Created by alderzhang on 2017/5/24.
//

#ifndef LITEAV_TXCABSTRACTTHREAD_H
#define LITEAV_TXCABSTRACTTHREAD_H

#include "TXCThread.h"

class TXCAbstractThread {
public:
	TXCAbstractThread();

	virtual ~TXCAbstractThread();//该类会在线程退出前析构自己，所以不需要用户自行delete它

	virtual int run(const char* name = 0, size_t stack = 0);

	virtual void requestExit();

	bool isRunning() const;

protected:
	bool exitPending() const;

	virtual int readyToRun();

	virtual int finishRun();

private:
	// Derived class must implement threadLoop(). The thread starts its life
	// here. There are two ways of using the Thread object:
	// 1) loop: if threadLoop() returns true, it will be called again if
	//          requestExit() wasn't called.
	// 2) once: if threadLoop() returns false, the thread will exit upon return.
	virtual bool threadLoop() = 0;

	TXCAbstractThread &operator=(const TXCAbstractThread &);

	static void *_threadLoop(void *user);

protected:
	mutable TXCMutex _mutex;

private:
	volatile bool _exitPending;
	volatile bool _running;
	TXCThread *_thread;

};


#endif //LITEAV_TXCABSTRACTTHREAD_H
