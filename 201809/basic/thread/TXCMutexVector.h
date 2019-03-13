//
// Created by alderzhang on 2017/6/23.
//

#ifndef TXCMUTEXVECTOR_H
#define TXCMUTEXVECTOR_H

#include "TXCLock.h"
#include "TXCCondition.h"

class TXCMutexVector {
	friend class TXCScopedMutexVector;

public:
	TXCMutexVector() {
		m_vec = 0;
		m_count = 0;
	}

private:
	TXCCondition m_cond;
	TXCMutex m_mutex;
	int m_vec;
	int m_count;
};

class TXCScopedMutexVector {
public:
	TXCScopedMutexVector(TXCMutexVector &mutex, int vector, bool initiallyLocked = true)
			: _mutex_vector(mutex), _vec(vector), _islocked(false) { if (initiallyLocked) Lock(); }

	~TXCScopedMutexVector() { if (_islocked) UnLock(); }

	bool IsLocked() const { return _islocked; }

	void Lock() {
		ASSERT(!_islocked);

		if (_islocked) return;

		TXCScopedLock lock(_mutex_vector.m_mutex);

		if (_mutex_vector.m_vec == _vec) {
			++_mutex_vector.m_count;
			_islocked = true;
			return;
		}

		while (_mutex_vector.m_count > 0 && _mutex_vector.m_vec != _vec) { _mutex_vector.m_cond.wait(lock); }

		ASSERT(0 <= _mutex_vector.m_count);

		++_mutex_vector.m_count;
		_islocked = true;

		if (_mutex_vector.m_vec != _vec) {
			ASSERT(1 == _mutex_vector.m_count);
			_mutex_vector.m_vec = _vec;
			_mutex_vector.m_cond.notifyAll(lock);
		}
	}

	void UnLock() {
		ASSERT(_islocked);

		if (!_islocked) return;

		TXCScopedLock lock(_mutex_vector.m_mutex);
		ASSERT(_vec == _mutex_vector.m_vec);
		--_mutex_vector.m_count;
		ASSERT(0 <= _mutex_vector.m_count);
		_islocked = false;

		if (0 >= _mutex_vector.m_count)
			_mutex_vector.m_cond.notifyAll(lock);
	}

	bool TryLock() {
		if (_islocked) return false;

		TXCScopedLock lock(_mutex_vector.m_mutex);

		if (_mutex_vector.m_vec == _vec) {
			++_mutex_vector.m_count;
			_islocked = true;
			return true;
		}

		if (_mutex_vector.m_count > 0) return false;

		ASSERT(0 == _mutex_vector.m_count);

		_mutex_vector.m_vec = _vec;
		_mutex_vector.m_count = 1;
		_islocked = true;
		_mutex_vector.m_cond.notifyAll(lock);
		return true;
	}

private:
	TXCMutexVector &_mutex_vector;
	int _vec;
	bool _islocked;
};

#endif    //TXCMUTEXVECTOR_H
