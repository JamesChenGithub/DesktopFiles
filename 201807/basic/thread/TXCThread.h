//
// Created by alderzhang on 2017/5/23.
//

#ifndef TXCTHREAD_H
#define TXCTHREAD_H

#include "TXCCondition.h"
#include "TXCRunnable.h"
#include <thread>

//注意！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
// 如果在pthread_join途中，其他线程进行pthread_detach, 会出现join函数不能退出的情况

class TXCThread {
private:
	class TXCRunnableReference {
	public:
		TXCRunnableReference(TXCRunnable *ntarget);

		~TXCRunnableReference();

		void AddRef();

		void RemoveRef(TXCScopedSpinLock &lock);

	private:
		TXCRunnableReference(const TXCRunnableReference &);

		TXCRunnableReference &operator=(const TXCRunnableReference &);

	public:
		TXCRunnable *target;
		int count;
		std::thread *thread;
		bool isJoined;
		bool isEnded;
		long afterTime;
		long periodicTime;
		bool isCancelDelayStart;
		TXCCondition condTime;
		TXCSpinLock spLock;
		char thread_name[128];
	};

public:
	template<class T>
	explicit TXCThread(const T &op, const char *thread_name = NULL)
			: _runable_ref(NULL) {
		_init(new TXCRunnableReference(txf_runnable_transform(op)), thread_name);
	}

	template<typename T>
	int start(const T &op, bool *newone = NULL) {
		return _start(newone, txf_runnable_transform(op));
	}

	TXCThread(const char *thread_name = NULL);

	virtual ~TXCThread();

	bool start(bool *newone = NULL);

	bool start_after(long after);

	void cancel_after();

	bool start_periodic(long after, long periodic); // ms

	void cancel_periodic();

	bool join() const;

	void outside_join() const;

	std::thread::id tid() const;

	bool isruning() const;

	const char *thread_name() const;

private:

	void _init(TXCRunnableReference *new_runable_ref, const char *thread_name);

	bool _start(bool *newone = NULL, TXCRunnable *new_target = NULL);

	static void _Init(void *arg);

	static void _Cleanup(void *arg);

	static void *_StartRoutine(void *arg);

	static void *_StartRoutineAfter(void *arg);

	static void *_StartRoutinePeriodic(void *arg);

private:
	TXCThread(const TXCThread &);

	TXCThread &operator=(const TXCThread &);

private:
	TXCRunnableReference *_runable_ref;
};


inline bool operator==(const TXCThread &lhs, const TXCThread &rhs);

inline bool operator!=(const TXCThread &lhs, const TXCThread &rhs);


#endif //TXCTHREAD_H
